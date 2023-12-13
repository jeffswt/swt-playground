import pathlib

import typer


app = typer.Typer()


@app.command()
def shift_regions(
    path: pathlib.Path = typer.Option(..., file_okay=False, dir_okay=True, exists=True),
    delta_cx: int = typer.Option(...),
    delta_cz: int = typer.Option(...),
):
    """Shifts all region files in a directory by a given amount of chunks."""

    if delta_cx % 32 != 0 or delta_cz % 32 != 0:
        raise typer.BadParameter("delta_cx and delta_cz must be multiples of 32")
    delta_rx = delta_cx // 32
    delta_rz = delta_cz // 32

    subdirectories = ["entities", "poi", "region"]
    for subdirectory in subdirectories:
        __shift_regions(path / subdirectory, delta_rx, delta_rz)
    return


def __shift_regions(path: pathlib.Path, delta_rx: int, delta_rz: int) -> None:
    fns = [p.parts[-1] for p in path.glob("*.mca")]
    locs = [fn.split(".")[1:3] for fn in fns]
    locs = [(int(rx), int(rz)) for rx, rz in locs]
    cx_multiplier = -1 if delta_rx >= 0 else 1
    cz_multiplier = -1 if delta_rz >= 0 else 1
    locs.sort(key=lambda loc: (loc[0] * cx_multiplier, loc[1] * cz_multiplier))
    for rx, rz in locs:
        src_path = __fmt_mca_path(path, rx, rz)
        dst_path = __fmt_mca_path(path, rx + delta_rx, rz + delta_rz)
        src_path.rename(dst_path)
        print(f"{src_path} -> {dst_path}")
    pass


def __fmt_mca_path(path: pathlib.Path, rx: int, rz: int) -> pathlib.Path:
    fn = f"r.{rx}.{rz}.mca"
    return path / fn


if __name__ == "__main__":
    app()
