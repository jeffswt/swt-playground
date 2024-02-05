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
    ts_us = (
        ts_modify.microsecond if ts_modify.microsecond != 0 else ts_create.microsecond
    )
    ts = min(ts_modify, ts_create)  # use whichever earlier
    if ts.microsecond == 0:
        ts = ts.replace(microsecond=ts_us)

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
        dt_s = re.sub(r"(am|pm)$", "", dt_s)  # whoever created this crap
        # we need to retain ascii chars only, for the sake of non-en apps
        dt_s = "".join(c for c in dt_s if ord(c) < 128)
        dt = datetime.datetime.strptime(dt_s, "%Y:%m:%d %H:%M:%S")
    except Exception as err:
        raise RuntimeError(f"invalid timestamp '{dt_b}' in exif: {path}")

    # finish
    return MediaMetadata(
        date_taken=dt,
    )


def _fetch_metadata(path: pathlib.Path) -> MediaMetadata:
    exif_exts = {".jpg", ".jpeg", ".bmp", ".png", ".heic", ".heif", ".mov"}
    metadata = _fetch_fs_metadata(path)
    if path.suffix.lower() in exif_exts:
        exif_metadata = _fetch_exif_metadata(path)
        if exif_metadata is not None:
            timestamp = exif_metadata.date_taken
            if timestamp.microsecond == 0:
                timestamp = timestamp.replace(microsecond=metadata.date_taken.microsecond)
            metadata.date_taken = timestamp
        pass
    return metadata


def _fetch_datetime_from_name(name: str) -> Optional[datetime.datetime]:
    def _parse_time(digits: List[int]) -> Optional[List[int]]:
        separators = [" ", "-", "_", ".", ",", ":"]
        separator_re = r"[" + "".join(separators) + r"]?"
        datetime_re = [r"([0-9]{" + str(d) + r"})" for d in digits]
        pattern = separator_re.join(datetime_re)
        matches = re.findall(pattern, name)
        if len(matches) == 0:
            return None
        match = matches[0]
        return [int(m) for m in match]

    with_ms = _parse_time([4, 2, 2, 2, 2, 2, 3])
    if with_ms:
        return datetime.datetime(
            year=with_ms[0],
            month=with_ms[1],
            day=with_ms[2],
            hour=with_ms[3],
            minute=with_ms[4],
            second=with_ms[5],
            microsecond=with_ms[6] * 1000,
        )
    without_ms = _parse_time([4, 2, 2, 2, 2, 2])
    if without_ms:
        return datetime.datetime(
            year=without_ms[0],
            month=without_ms[1],
            day=without_ms[2],
            hour=without_ms[3],
            minute=without_ms[4],
            second=without_ms[5],
        )
    return None


###############################################################################
#   parser & formatter rules


class FilenameComponents(pydantic.BaseModel):
    original_fn: str
    date_taken: datetime.datetime
    seq_num: int
    pass


def _default_filename_components(path: pathlib.Path) -> FilenameComponents:
    original_names = re.findall(r"\[([^\]]+)\]", path.name)
    original_fn = original_names[0] if len(original_names) > 0 else path.name
    metadata = _fetch_metadata(path)
    timestamp = _fetch_datetime_from_name(path.name)
    timestamp = timestamp if timestamp else metadata.date_taken
    if timestamp.microsecond == 0:
        timestamp = timestamp.replace(microsecond=metadata.date_taken.microsecond)
    return FilenameComponents(
        original_fn=original_fn,
        date_taken=timestamp,
        seq_num=0,
    )


class FilenameScheme(enum.Enum):
    original = "original"
    sortable = "sortable"
    screenshot = "screenshot"
    std = "std"
    std_invert = "std_invert"
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
        result = _default_filename_components(path)
        _fn_part = path.parts[-1]
        seq_num = re.findall(r"[^\d](\d+)\.[^.]+$", _fn_part)
        seq_num = int(seq_num[0]) if len(seq_num) > 0 else 0
        result.seq_num = seq_num
        return result

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
        result = _default_filename_components(path)
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
        result.original_fn = original_fn
        result.date_taken = datetime.datetime(year, month, day)
        result.seq_num = seq_num
        return result

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


def _fn_scheme_screenshot() -> FilenameSchemeDef:
    def _pattern(path: pathlib.Path) -> Optional[FilenameComponents]:
        result = _default_filename_components(path)
        pattern = (
            r"^Screenshot from (\d{4})-(\d{2})-(\d{2}) \d{2}-\d{2}-\d{2}-\d{3}\.(.*?)$"
        )
        match = re.match(pattern, path.name)
        if not match:
            return None
        # extract components
        year = int(match.group(1))
        month = int(match.group(2))
        day = int(match.group(3))
        hour = int(match.group(4))
        minute = int(match.group(5))
        second = int(match.group(6))
        ext = match.group(4)
        # finish
        result.date_taken = datetime.datetime(year, month, day, hour, minute, second)
        return result

    def _format(components: FilenameComponents) -> str:
        ext = components.original_fn.split(".")[-1].lower()
        date = f"{components.date_taken:%Y-%m-%d}"
        time = f"{components.date_taken:%H-%M-%S-%f}"[:-3]
        return f"Screenshot from {date} {time}.{ext}"

    _sample = "Screenshot from 2023-03-21 22-56-30-394.png"

    return FilenameSchemeDef(
        identifier=FilenameScheme.screenshot,
        pattern=_pattern,
        format=_format,
        sample=_sample,
    )


def _fn_scheme_std() -> FilenameSchemeDef:
    def _pattern(path: pathlib.Path) -> Optional[FilenameComponents]:
        result = _default_filename_components(path)
        pattern_fn = r"^(\d{4})-(\d{2})-(\d{2})_(\d{2})-(\d{2})-(\d{2})-(\d{3})_\[([^\]]+)\]\.(.*?)$"
        pattern_no_fn = (
            r"^(\d{4})-(\d{2})-(\d{2})_(\d{2})-(\d{2})-(\d{2})-(\d{3})\.(.*?)$"
        )
        match_fn = re.match(pattern_fn, path.name)
        match_no_fn = re.match(pattern_no_fn, path.name)
        if match_fn:
            year = int(match_fn.group(1))
            month = int(match_fn.group(2))
            day = int(match_fn.group(3))
            hour = int(match_fn.group(4))
            minute = int(match_fn.group(5))
            second = int(match_fn.group(6))
            us = int(match_fn.group(7))
            original_fn = match_fn.group(8)
            ext = match_fn.group(9)
        elif match_no_fn:
            year = int(match_no_fn.group(1))
            month = int(match_no_fn.group(2))
            day = int(match_no_fn.group(3))
            hour = int(match_no_fn.group(4))
            minute = int(match_no_fn.group(5))
            second = int(match_no_fn.group(6))
            us = int(match_no_fn.group(7))
            original_fn = path.name
            ext = match_no_fn.group(8)
        else:
            return None
        # finish
        result.original_fn = original_fn
        result.date_taken = datetime.datetime(year, month, day, hour, minute, second)
        return result

    def _format(components: FilenameComponents) -> str:
        ext = components.original_fn.split(".")[-1].lower()
        date = f"{components.date_taken:%Y-%m-%d}"
        time = f"{components.date_taken:%H-%M-%S-%f}"[:-3]
        # the original filename must take the format 'ABCD_E5678.EXT'
        fn = components.original_fn
        fn_rule_has_name = len(fn) > 0
        fn_rule_extensions = len(fn.split(".")) == 2
        fn_rule_format_pattern = r"^[_A-Z]{3,4}_E?\d{3,5}$"
        fn_rule_format = bool(re.match(fn_rule_format_pattern, fn.split(".")[0]))
        if fn_rule_has_name and fn_rule_extensions and fn_rule_format:
            return f"{date}_{time}_[{fn}].{ext}"
        else:
            return f"{date}_{time}.{ext}"

    _sample = "2023-03-21_22-56-30-394_[IMG_0123.PNG].png / 2023-03-21_22-56-30-394.png"

    return FilenameSchemeDef(
        identifier=FilenameScheme.std,
        pattern=_pattern,
        format=_format,
        sample=_sample,
    )


def _fn_scheme_std_invert() -> FilenameSchemeDef:
    fn_scheme_std = _fn_scheme_std()

    def _pattern(path: pathlib.Path) -> Optional[FilenameComponents]:
        result = fn_scheme_std.pattern(path)
        if result is not None:
            return None
        return _default_filename_components(path)

    def _format(components: FilenameComponents) -> str:
        return components.original_fn

    _sample = "any_filename_not_matched_by_[std].ext"

    return FilenameSchemeDef(
        identifier=FilenameScheme.std_invert,
        pattern=_pattern,
        format=_format,
        sample=_sample,
    )


_fn_schemes = [
    _fn_scheme_original(),
    _fn_scheme_sortable(),
    _fn_scheme_screenshot(),
    _fn_scheme_std(),
    _fn_scheme_std_invert(),
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
            bar.write(f"  - skip {_path_rel_to(old_path, root_path)}", end="\r")
            continue
        # format
        new_fn = to_fns.format(components)
        new_path = old_path.parent / new_fn
        # plan
        plans.append((old_path, new_path))
        bar.write(f"convert: {_path_rel_to(old_path, root_path)} -> {new_path.name}")
    bar.close()

    if not apply:
        return
    bar = tqdm.tqdm(plans, desc="committing changes")
    for path, new_path in bar:
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
