import datetime
import pathlib
import re
import subprocess
from typing import Dict, Iterable, List, Optional

import pydantic
import typer


###############################################################################
#   git utilities


class GitCommit(pydantic.BaseModel):
    """A Git commit."""

    # tree structure
    hash: str
    is_head: bool
    refs: List[str]
    tags: List[str]

    # metadata
    author_name: str
    author_email: str
    author_date: datetime.datetime
    committer_name: str
    committer_email: str
    committer_date: datetime.datetime
    msg_subject: str
    msg_body: str

    pass


class GitHistory(pydantic.BaseModel):
    """A Git history."""

    head: str
    """HEAD pointer, may be commit hash or ref name."""
    commits: List[GitCommit]

    pass


def _parse_git_history(repo: pathlib.Path, end_hash: str) -> GitHistory:
    return


def __get_raw_git_logs(repo: pathlib.Path, end_hash: str) -> str:
    fmt_rule = (
        "--pretty=format:"
        "commit %H\n"
        "refs %D\n"
        "author-name %an\n"
        "author-email %ae\n"
        "author-date %ai\n"
        "committer-name %cn\n"
        "committer-email %ce\n"
        "committer-date %ci\n"
        "message %B\n"
        "end-of-commit %H\n\n"
    )
    proc = subprocess.Popen(
        args=["git", "log", end_hash, fmt_rule],
        cwd=repo,
        stdout=subprocess.PIPE,
    )
    if not proc.stdout:
        return ""
    stdout = proc.stdout.read()
    proc.wait()
    return stdout.decode("utf-8", "ignore")


def __split_raw_git_logs(raw: str) -> Iterable[List[str]]:
    buffer: List[str] = []
    current_head: Optional[str] = None

    for line in raw.splitlines():
        if current_head is None:
            if not line.startswith("commit "):
                continue
            current_head = line
            buffer.append(line)
        else:
            if line == f"end-of-{current_head}":
                yield buffer
                buffer = []
                current_head = None
            else:
                buffer.append(line)
        pass
    return


def __parse_raw_git_commit(lines: List[str]) -> GitCommit:
    kvs: Dict[str, str] = {}
    message: List[str] = []

    # analyze raw text
    ptr = 0
    while ptr < len(lines):
        line = lines[ptr]
        key, value = line.split(" ", 1)
        if key == "message":
            message.append(value)
            ptr += 1
            break
        else:
            kvs[key] = value
        ptr += 1
    while ptr < len(lines):
        message.append(lines[ptr])
        ptr += 1

    # parse refs
    is_head = False
    refs: List[str] = []
    tags: List[str] = []
    for r in kvs.get("refs", "").split(", "):
        if not r:
            continue
        if r.startswith("HEAD -> "):
            is_head = True
            r = r[len("HEAD -> ") :]
        if r.startswith("tag: "):
            tags.append(r[len("tag: ") :])
        else:
            refs.append(r)
        pass

    return GitCommit(
        hash=kvs["commit"],
        is_head=is_head,
        refs=refs,
        tags=tags,
        author_name=kvs["author-name"],
        author_email=kvs["author-email"],
        author_date=datetime.datetime.fromisoformat(kvs["author-date"]),
        committer_name=kvs["committer-name"],
        committer_email=kvs["committer-email"],
        committer_date=datetime.datetime.fromisoformat(kvs["committer-date"]),
        msg_subject=message[0],
        msg_body="\n".join(message[1:]).strip(),
    )


###############################################################################
#   main

if __name__ == "__main__":
    x = __get_raw_git_logs(pathlib.Path("."), "HEAD")
    for i in __split_raw_git_logs(x):
        print(__parse_raw_git_commit(i))
    pass
