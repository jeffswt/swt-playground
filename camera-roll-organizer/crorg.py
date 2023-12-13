import argparse
import datetime
import enum
import os
import pathlib
import re
import shutil
import sys
from typing import Callable, Optional

import piexif
import pydantic
import pyheif
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


def _fetch_exif_metadata(path: pathlib.Path) -> Optional[MediaMetadata]:
    # attempt to read exif from file
    try:
        heic_file = pyheif.read(path)
    except Exception as err:
        raise RuntimeError(f"cannot load exif from file: {path}")
    # extract exif from heic
    exif = [i["data"] for i in heic_file.metadata if i["type"] == "Exif"]
    if len(exif) <= 0:
        return None  # no exif in image
    exif = exif[0]
    # convert raw exif to exif dict
    try:
        ed = piexif.load(exif)
    except Exception as err:
        raise RuntimeError(f"invalid exif in file: {path}")

    # extract timestamp
    dt_b = ed.get("0th", {}).get(306, None)
    if not dt_b:
        return None  # no timestamp in exif
    # parse datetime
    try:
        dt_s = dt_b.decode("utf8", "ignore")
        dt = datetime.datetime.strptime(dt_s, "%Y:%m:%d %H:%M:%S")
    except Exception as err:
        raise RuntimeError(f"invalid timestamp '{dt_b}' in exif: {path}")

    # finish
    return MediaMetadata(
        date_taken=dt,
    )


def _fetch_metadata(path: pathlib.Path) -> MediaMetadata:
    img_exts = {"jpg", "jpeg", "bmp", "png", "heic", "heif"}
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


class FilenameScheme(pydantic.BaseModel):
    """A canonical identifier for this particular filename scheme."""

    identifier: str

    """A pattern that matches any file path against this scheme -- if any."""
    pattern: Callable[[pathlib.Path], Optional[FilenameComponents]]

    """A formatter that produces a filename from the components."""
    format: Callable[[FilenameComponents], str]

    """A sample filename for demonstration."""
    sample: str

    pass


def _fn_scheme_original() -> FilenameScheme:
    _identifier = "original"

    def _pattern(path: pathlib.Path) -> Optional[FilenameComponents]:
        metadata = _fetch_metadata(path)
        fn_part = path.parts[-1]
        seq_num = re.findall(r"_(\d+)$", fn_part)
        seq_num = int(seq_num[0]) if len(seq_num) > 0 else 0
        return FilenameComponents(
            original_fn=path.name,
            date_taken=metadata.date_taken,
            seq_num=seq_num,
        )

    def _format(components: FilenameComponents) -> str:
        return components.original_fn

    _sample = "IMG_2468.JPG"

    return FilenameScheme(
        identifier=_identifier,
        pattern=_pattern,
        format=_format,
        sample=_sample,
    )


def _fn_scheme_sortable() -> FilenameScheme:
    _identifier = "sortable"

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

    return FilenameScheme(
        identifier=_identifier,
        pattern=_pattern,
        format=_format,
        sample=_sample,
    )


_fn_schemes = [
    _fn_scheme_original(),
    _fn_scheme_sortable(),
]

FilenameSchemeKeys = enum.Enum(
    "FilenameSchemeKeys",
    {s.identifier: s for s in _fn_schemes},
)


###############################################################################
#   shell interface


app = typer.Typer()

_fn_scheme_samples = "; ".join(f"'{s.identifier}': `{s.sample}`" for s in _fn_schemes)


@app.command()
def convert(
    path: pathlib.Path = typer.Argument(
        help="Path to the file or directory to (recursively) convert.",
    ),
    _from: str = typer.Option(
        ...,
        "--from",
        help=f"Match only these filenames: {_fn_scheme_samples}.",
    ),
    _to: str = typer.Option(
        ...,
        "--to",
        help=f"Target filename format: {_fn_scheme_samples}.",
    ),
    commit: bool = typer.Option(
        False,
        help="Actually commit changes to disk.",
    ),
) -> None:
    print(path, _from, _to, commit)
    return


if __name__ == "__main__":
    app()
