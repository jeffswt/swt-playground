def try_install_deps() -> None:
    # ruff: noqa: E402
    import os
    import sys

    try:
        import dfindexeddb
        import pydantic
        import typer

        _ = dfindexeddb, pydantic, typer
    except ImportError as _:
        cmd = f"{sys.executable} -m pip install dfindexeddb[plugins] pydantic typer"
        print(f"> {cmd}")
        os.system(cmd)

    return


try_install_deps()


import datetime
import io
import json
import pathlib
import re
import shutil
import subprocess
import sys
import tempfile
from typing import Any, Callable, Dict, Iterable, List, Literal, Optional, TypeVar
from uuid import UUID

import pydantic
import typer
from dfindexeddb.indexeddb.cli import App


def get_home_dir() -> pathlib.Path:
    # we need to get the home dir of the current windows installation
    if sys.platform == "win32":
        return pathlib.Path.home().resolve()
    elif sys.platform == "linux":
        proc = subprocess.Popen(
            ["/mnt/c/Windows/System32/whoami.exe"], stdout=subprocess.PIPE
        )
        stdout, _ = proc.communicate()
        username = stdout.decode("utf-8").strip().replace("\\", "/")
        username = username.split("/")[-1]
        root = pathlib.Path(f"/mnt/c/Users/{username}")
        assert root.exists()
        return root
    raise NotImplementedError(f"unsupported platform: {sys.platform}")


def get_postman_db_dirs() -> List[pathlib.Path]:
    home_dir = get_home_dir()
    partitions = home_dir / "AppData/Roaming/Postman/Partitions/"
    guid_pattern = r"^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$"
    guid_pattern = re.compile(guid_pattern, flags=re.IGNORECASE)
    dbs = []
    for p in partitions.iterdir():
        if not p.is_dir() or not guid_pattern.match(p.name):
            continue
        for db in (p / "IndexedDB/").iterdir():
            if not db.is_dir() or not db.name.endswith(".indexeddb.leveldb"):
                continue
            for fn in db.iterdir():  # file mode
                exts = {".ldb", ".log"}
                if not fn.is_file() or not any(fn.name.endswith(i) for i in exts):
                    continue
                dbs.append(fn)
            # dbs.append(db)  # 'db' mode
    dbs.sort(key=lambda p: p.stat().st_mtime)
    return dbs


def dump_db(db: pathlib.Path) -> List[Dict[str, Any]]:
    # clone to avoid modifying the original db
    tmp_dir = pathlib.Path(tempfile.gettempdir())  # 'db' mode
    # tmp_dir = tmp_dir / f".db_dump_{random.randint(0, 999999):06d}"
    # if tmp_dir.exists():
    #     shutil.rmtree(tmp_dir, ignore_errors=True)
    # shutil.copytree(db, tmp_dir)
    tmp_dir = tmp_dir / ".db_dump_114514/"  # file mode
    if tmp_dir.exists():
        shutil.rmtree(tmp_dir, ignore_errors=True)
    tmp_dir.mkdir(parents=True, exist_ok=True)
    shutil.copy(db, tmp_dir / db.name)

    # capture original output
    old_stdout = sys.stdout
    old_argv = sys.argv
    hack_stdout = io.StringIO()
    hack_argv = (
        "dfindexeddb",
        # "db",  # 'db' mode
        {".ldb": "ldb", ".log": "log"}[db.suffix.lower()],  # file mode
        "-s",
        db.as_posix(),
        "-o",
        "jsonl",
        # "--format",  # 'db' mode
        # "chrome",  # 'db' mode
    )
    sys.stdout = hack_stdout
    sys.argv = hack_argv
    App()
    sys.stdout = old_stdout
    sys.argv = old_argv

    # parse jsonl output
    jsonl = hack_stdout.getvalue()
    result = []
    for line in jsonl.splitlines():
        if line:
            try:
                result.append(json.loads(line))
            except json.JSONDecodeError:
                pass
    shutil.rmtree(tmp_dir, ignore_errors=True)
    return result


class CollectionRecord(pydantic.BaseModel):
    kind: Literal["collection"] = "collection"
    id: UUID
    name: str
    values: Dict[str, List[str]]


class HistoryRecord(pydantic.BaseModel):
    kind: Literal["history"] = "history"
    timestamp: datetime.datetime
    # http_version: str
    method: Literal["GET", "POST", "PUT", "DELETE", "PATCH"]
    url: str
    headers: Dict[str, str]
    # _body: Dict[str, Any]
    collection: Optional[UUID]


def take_network_record(record: Dict[str, Any]) -> HistoryRecord:
    # network records are cleared upon each subsequent postman app launch so
    # does not contain all of the records
    assert record["__type__"] == "IndexedDBRecord"
    value = record["value"]
    assert value["__type__"] == "ObjectStoreDataValue"
    value = value["value"]
    request = value["details"]["request"]
    timestamp = datetime.datetime.fromtimestamp(value["timestamp"] / 1000)
    headers = {i["key"]: i["value"] for i in request["headers"]["values"]}

    return HistoryRecord(
        timestamp=timestamp,
        # http_version=request["httpVersion"],
        method=request["method"],
        url=request["url"],
        headers=headers,
        # _body=value,
        collection=None,
    )


def take_history_record(record: Dict[str, Any]) -> HistoryRecord:
    # variables here are not replaced to the proper counterparts
    assert record["__type__"] == "IndexedDBRecord"
    value = record["value"]
    assert value["__type__"] == "ObjectStoreDataValue"
    value = value["value"]
    url = value["url"]
    timestamp = value.get("updatedAt", value["createdAt"])
    if isinstance(timestamp, (int, float)):
        timestamp = datetime.datetime.fromtimestamp(timestamp / 1000)
    else:
        timestamp = datetime.datetime.strptime(timestamp, "%Y-%m-%dT%H:%M:%S.%fZ")
    # unify to UTC
    timestamp = timestamp.astimezone().astimezone(datetime.timezone.utc)
    # timestamp = timestamp.replace(tzinfo=None)
    method = value["method"]
    url = value["url"]
    headers = {i["key"]: i["value"] for i in value["headerData"]["values"]}
    return HistoryRecord(
        timestamp=timestamp,
        method=method,
        url=url,
        headers=headers,
        # _body=value["data"],
        collection=value.get("collection", None),
    )


def take_collection_delta_record(key: str, record: Dict[str, Any]) -> CollectionRecord:
    assert record["__type__"] == "IndexedDBRecord"
    value = record["value"]
    assert value["__type__"] == "ObjectStoreDataValue"
    value = value["value"]
    assert value["id"].startswith(f"{key}/")
    assert value["model"] == key
    vs = {i["key"]: [i["value"]] for i in value["values"]["values"]}
    return CollectionRecord(
        id=value["modelId"],
        name="",
        values=vs,
    )


def take_collection_def_record(record: Dict[str, Any]) -> CollectionRecord:
    assert record["__type__"] == "IndexedDBRecord"
    value = record["value"]
    assert value["__type__"] == "ObjectStoreDataValue"
    value = value["value"]
    assert value["uid"] == value["owner"] + "-" + value["id"]
    assert "favorite" in value
    assert "protocolProfileBehavior" in value
    assert "createdAt" in value
    return CollectionRecord(
        id=value["id"],
        name=value["name"],
        values={},
    )


def take_record(item: Dict[str, Any]) -> Optional[HistoryRecord]:
    try:
        return take_network_record(item)
    except Exception as _:
        pass
    try:
        return take_history_record(item)
    except Exception as _:
        pass
    return None


def take_collection_record(item: Dict[str, Any]) -> Optional[CollectionRecord]:
    try:
        return take_collection_delta_record("globals", item)
    except Exception as _:
        pass
    try:
        return take_collection_delta_record("collection", item)
    except Exception as _:
        pass
    try:
        return take_collection_def_record(item)
    except Exception as _:
        pass
    return None


T = TypeVar("T")


def deduplicate(items: Iterable[T], key: Callable[[T], Any]) -> List[T]:
    seen = set()
    result = []
    for item in items:
        k = key(item)
        if k not in seen:
            seen.add(k)
            result.append(item)
    return result


def dedup_collect(items: List[CollectionRecord]) -> List[CollectionRecord]:
    ids: Dict[str, List[CollectionRecord]] = {}
    for item in items:
        ids.setdefault(str(item.id), []).append(item)
    result: List[CollectionRecord] = []
    for k, vs in ids.items():
        id = vs[0].id
        name = [i.name for i in vs if i.name]
        name = name[0] if name else ""
        values = {}
        for i in vs:
            for k, v in i.values.items():
                values.setdefault(k, []).extend(v)
        values = {k: sorted(set(v)) for k, v in values.items()}
        result.append(CollectionRecord(id=id, name=name, values=values))
    return result


app = typer.Typer()


@app.command("extract")
def main(
    db_path: Optional[str] = typer.Option(None),
    output: pathlib.Path = typer.Option(...),
):
    dbs = get_postman_db_dirs()
    ls_history: List[HistoryRecord] = []
    ls_collect: List[CollectionRecord] = []
    # ret = []
    for db in dbs:
        for item in dump_db(db):
            # ret.append(item)
            # continue
            if history := take_record(item):
                ls_history.append(history)
            elif collection := take_collection_record(item):
                ls_collect.append(collection)
        pass
    ls_collect = dedup_collect(ls_collect)
    # with open("dump.json", "w", encoding="utf-8") as f:
    #     json.dump(ret, f, indent=2, ensure_ascii=False)
    # return

    # print
    ls_history.sort(key=lambda x: x.timestamp, reverse=True)
    ls_history = deduplicate(
        ls_history, key=lambda x: (x.method, x.url, x.timestamp, x.collection)
    )
    ls: List[pydantic.BaseModel] = ls_collect + ls_history  # type: ignore
    for item in ls:
        print(item.model_dump_json(indent=None))

    # save
    if output.suffix.lower() == ".json":
        with open(output, "w") as f:
            out = []
            for item in ls:
                r = item.model_dump_json(indent=None)
                j = json.loads(r)
                out.append(j)
            r = json.dumps(out, indent=2, ensure_ascii=True)
            f.write(r + "\n")
    elif output.suffix.lower() == ".jsonl":
        with open(output, "w") as f:
            for item in ls:
                r = item.model_dump_json(indent=None)
                j = json.loads(r)
                r = json.dumps(j, indent=None, ensure_ascii=True)
                f.write(r + "\n")
    else:
        raise ValueError(f"unsupported output format: {output.suffix}")
    return


if __name__ == "__main__":
    app()
