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

    path: str
    """Absolute path of the repo."""
    head: str
    """HEAD pointer, may be commit hash or ref name."""
    head_hash: str
    """HEAD commit hash."""

    commits: List[GitCommit]

    pass


def _parse_git_history(repo: pathlib.Path, end_hash: str) -> GitHistory:
    head = __get_repo_head(repo)
    head_hash = __parse_git_rev_as_hash(repo, head)
    commits_raw = __get_raw_git_logs(repo, end_hash)
    commits = [__parse_raw_git_commit(i) for i in __split_raw_git_logs(commits_raw)]
    return GitHistory(
        path=str(repo.resolve()),
        head=head,
        head_hash=head_hash,
        commits=commits,
    )


def __get_repo_head(repo: pathlib.Path) -> str:
    # know current HEAD and ensure it's clean
    proc = subprocess.Popen(
        args=["git", "status"],
        cwd=repo,
        stdout=subprocess.PIPE,
    )
    if not proc.stdout:
        raise RuntimeError("cannot get repo status")
    stdout = proc.stdout.read()
    lines = stdout.decode("utf-8", "ignore").splitlines()
    proc.wait()
    if proc.returncode != 0:
        raise RuntimeError("failed to get repo status")

    # parse HEAD
    flag_head = "HEAD detached at "
    flag_branch = "On branch "
    flag_clean = "nothing to commit, working tree clean"

    if not any(line.startswith(flag_clean) for line in lines):
        raise RuntimeError("working tree is not clean")
    if lines[0].startswith(flag_head):
        return lines[0][len(flag_head) :]
    if lines[0].startswith(flag_branch):
        return lines[0][len(flag_branch) :]
    raise RuntimeError("cannot parse repo status")


def __parse_git_rev_as_hash(repo: pathlib.Path, rev: str) -> str:
    proc = subprocess.Popen(
        args=["git", "rev-parse", rev],
        cwd=repo,
        stdout=subprocess.PIPE,
    )
    if not proc.stdout:
        raise RuntimeError("cannot perform rev-parse")
    stdout = proc.stdout.read()
    proc.wait()
    the_hash = stdout.decode("utf-8", "ignore").strip()
    if len(the_hash) != 40:
        raise RuntimeError("cannot parse rev-parse")
    return the_hash


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


def _git_checkout(repo: pathlib.Path, ref: str) -> None:
    proc = subprocess.Popen(
        args=["git", "checkout", ref],
        cwd=repo,
        stdout=subprocess.PIPE,
    )
    if not proc.stdout:
        raise RuntimeError("cannot checkout")
    stdout = proc.stdout.read()
    proc.wait()
    if proc.returncode != 0:
        raise RuntimeError(f"failed to checkout: {proc.returncode}")
    return


###############################################################################
#   main


if __name__ == "__main__":
    print(_parse_git_history(pathlib.Path("."), "HEAD").path)
    pass
