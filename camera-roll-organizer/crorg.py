import datetime
import enum
import os
import pathlib
import re
from typing import Callable, Iterable, List, Optional, Tuple

import exifread
import pydantic
import tqdm
import typer


###############################################################################
#   metadata retrieval


class MediaMetadata(pydantic.BaseModel):
    date_taken: datetime.datetime
    pass


def _fetch_fs_metadata(path: pathlib.Path) -> MediaMetadata:
    ts_modify = datetime.datetime.fromtimestamp(os.path.getmtime(path))
    ts_create = datetime.datetime.fromtimestamp(os.path.getctime(path))
    ts = min(ts_modify, ts_create)  # use whichever earlier

    return MediaMetadata(
        date_taken=ts,
    )


def _monkey_patch_exifread():
    """https://github.com/ianare/exif-py/issues/160#issuecomment-1447158385
    Patch by `hpoul`, applied for exifread=3.0.0"""

    from exifread import HEICExifFinder
    from exifread.exif_log import get_logger
    from exifread.heic import NoParser

    logger = get_logger()
    logger.setLevel("ERROR")
    _old_get_parser = HEICExifFinder.get_parser

    def _get_parser(self, box):
        try:
            return _old_get_parser(self, box)
        except NoParser:
            logger.warning("ignoring parser %s", box.name)
            return None

    HEICExifFinder.get_parser = _get_parser


def _fetch_exif_metadata(path: pathlib.Path) -> Optional[MediaMetadata]:
    # attempt to read exif from file
    try:
        with open(path, "rb") as f:
            tags = exifread.process_file(f, debug=True)
    except Exception as err:
        raise RuntimeError(f"cannot load exif from file: {path}")
    # extract timestamp
    dt_b = tags.get("EXIF DateTimeOriginal", None)
    if not dt_b:
        return None  # no timestamp in exif
    # parse datetime
    try:
        dt_s = dt_b.values
        dt = datetime.datetime.strptime(dt_s, "%Y:%m:%d %H:%M:%S")
    except Exception as err:
        raise RuntimeError(f"invalid timestamp '{dt_b}' in exif: {path}")

    # finish
    return MediaMetadata(
        date_taken=dt,
    )


def _fetch_metadata(path: pathlib.Path) -> MediaMetadata:
    img_exts = {".jpg", ".jpeg", ".bmp", ".png", ".heic", ".heif"}
    metadata = _fetch_fs_metadata(path)
    if path.suffix.lower() in img_exts:
        metadata = _fetch_exif_metadata(path) or metadata
    return metadata


###############################################################################
#   parser & formatter rules


class FilenameComponents(pydantic.BaseModel):
    original_fn: str
    date_taken: datetime.datetime
    seq_num: int
    pass


class FilenameScheme(enum.Enum):
    original = "original"
    sortable = "sortable"
    pass


class FilenameSchemeDef(pydantic.BaseModel):
    """A canonical identifier for this particular filename scheme."""

    identifier: FilenameScheme

    """A pattern that matches any file path against this scheme -- if any."""
    pattern: Callable[[pathlib.Path], Optional[FilenameComponents]]

    """A formatter that produces a filename from the components."""
    format: Callable[[FilenameComponents], str]

    """A sample filename for demonstration."""
    sample: str

    pass


def _fn_scheme_original() -> FilenameSchemeDef:
    def _pattern(path: pathlib.Path) -> Optional[FilenameComponents]:
        metadata = _fetch_metadata(path)
        fn_part = path.parts[-1]
        seq_num = re.findall(r"[^\d](\d+)\.[^.]+$", fn_part)
        seq_num = int(seq_num[0]) if len(seq_num) > 0 else 0
        return FilenameComponents(
            original_fn=path.name,
            date_taken=metadata.date_taken,
            seq_num=seq_num,
        )

    def _format(components: FilenameComponents) -> str:
        return components.original_fn

    _sample = "IMG_2468.JPG"

    return FilenameSchemeDef(
        identifier=FilenameScheme.original,
        pattern=_pattern,
        format=_format,
        sample=_sample,
    )


def _fn_scheme_sortable() -> FilenameSchemeDef:
    def _pattern(path: pathlib.Path) -> Optional[FilenameComponents]:
        pattern = r"^(\d+) (\d{4})-(\d{2})-(\d{2}) \[([^\]]*?)\]\.(.*?)$"
        match = re.match(pattern, path.name)
        if not match:
            return None
        # extract components
        seq_num = int(match.group(1))
        year = int(match.group(2))
        month = int(match.group(3))
        day = int(match.group(4))
        original_fn = match.group(5)
        ext = match.group(6)
        # finish
        return FilenameComponents(
            original_fn=original_fn,
            date_taken=datetime.datetime(year, month, day),
            seq_num=seq_num,
        )

    def _format(components: FilenameComponents) -> str:
        ext = components.original_fn.split(".")[-1].lower()
        return f"{components.seq_num:04d} {components.date_taken:%Y-%m-%d} [{components.original_fn}].{ext}"

    _sample = "2468 1970-01-01 [IMG_2468.JPG].jpg"

    return FilenameSchemeDef(
        identifier=FilenameScheme.sortable,
        pattern=_pattern,
        format=_format,
        sample=_sample,
    )


_fn_schemes = [
    _fn_scheme_original(),
    _fn_scheme_sortable(),
]


###############################################################################
#   shell interface


def _enumerate_files(path: pathlib.Path) -> Iterable[pathlib.Path]:
    if path.is_file():
        yield path
    elif path.is_dir():
        for subpath in path.iterdir():
            yield from _enumerate_files(subpath)
    else:
        raise RuntimeError(f"invalid path: {path}")
    return


def _path_rel_to(path: pathlib.Path, base: pathlib.Path) -> str:
    if base.is_file():
        base = base.parent
    return str(path.relative_to(base))


def _convert(
    root_path: pathlib.Path,
    from_fns_id: FilenameScheme,
    to_fns_id: FilenameScheme,
    apply: bool,
) -> None:
    fn_schemes = dict((s.identifier, s) for s in _fn_schemes)
    from_fns = fn_schemes[from_fns_id]
    to_fns = fn_schemes[to_fns_id]

    files: List[pathlib.Path] = []
    bar = tqdm.tqdm(desc="populating files")
    for path in _enumerate_files(root_path):
        files.append(path)
        bar.update()
    bar.close()

    plans: List[Tuple[pathlib.Path, pathlib.Path]] = []
    bar = tqdm.tqdm(files, desc="analyzing files")
    for old_path in bar:
        # parse
        components = from_fns.pattern(old_path)
        if not components:
            bar.write(f"  - skip {_path_rel_to(old_path, root_path)}")
            continue
        # format
        new_fn = to_fns.format(components)
        new_path = old_path.parent / new_fn
        # plan
        plans.append((old_path, new_path))
        bar.write(
            f"convert: {_path_rel_to(old_path, root_path)} -> {_path_rel_to(new_path, root_path)}"
        )
    bar.close()

    if not apply:
        return
    bar = tqdm.tqdm(plans, desc="committing changes")
    for path, new_path in plans:
        path.rename(new_path)
    bar.close()
    return


app = typer.Typer()

_fn_scheme_samples = "; ".join(
    f"'{s.identifier.value}': `{s.sample}`" for s in _fn_schemes
)


@app.command()
def convert(
    path: pathlib.Path = typer.Argument(
        help="Path to the file or directory to (recursively) convert.",
        exists=True,
    ),
    _from: FilenameScheme = typer.Option(
        ...,
        "--from",
        help=f"Match only these filenames: {_fn_scheme_samples}.",
    ),
    _to: FilenameScheme = typer.Option(
        ...,
        "--to",
        help=f"Target filename format: {_fn_scheme_samples}.",
    ),
    apply: bool = typer.Option(
        False,
        help="Actually commit changes to disk.",
    ),
) -> None:
    _convert(path, _from, _to, apply)
    return


if __name__ == "__main__":
    _monkey_patch_exifread()
    app()
