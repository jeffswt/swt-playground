import datetime
import glob
import pathlib
import re
import shutil
import subprocess
from typing import Callable, Iterable, Literal

import pydantic
import typer


###############################################################################
#   git utilities


CommitHash = str


class GitCommit(pydantic.BaseModel):
    """A Git commit."""

    # tree structure
    hash: CommitHash
    is_head: bool
    refs: list[str]
    tags: list[str]

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
    head_hash: CommitHash
    """HEAD commit hash."""

    commits: list[GitCommit]

    pass


def _parse_git_history(repo: pathlib.Path, end_hash: CommitHash) -> GitHistory:
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


def __get_raw_git_logs(repo: pathlib.Path, end_hash: CommitHash) -> str:
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


def __split_raw_git_logs(raw: str) -> Iterable[list[str]]:
    buffer: list[str] = []
    current_head: str | None = None

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


def __parse_raw_git_commit(lines: list[str]) -> GitCommit:
    kvs = dict[str, str]()
    message = list[str]()

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
    refs = list[str]()
    tags = list[str]()
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
#   mutation plan


class FilterRule(pydantic.BaseModel):
    """Cherry-pick commits with regular expressions or predicates. Those not
    matched by this rule will not be copied into the new repository. Any field
    that is not assigned a value will be ignored and matches all commits."""

    kind: Literal["filter"] = "filter"

    tags: Callable[[list[str]], bool] | None = None
    author_name: str | Callable[[str], bool] | None = None
    author_email: str | Callable[[str], bool] | None = None
    author_date: Callable[[datetime.datetime], bool] | None = None
    committer_name: str | Callable[[str], bool] | None = None
    committer_email: str | Callable[[str], bool] | None = None
    committer_date: Callable[[datetime.datetime], bool] | None = None
    msg_subject: str | Callable[[str], bool] | None = None
    msg_body: str | Callable[[str], bool] | None = None

    pass


class ReCommitRule(pydantic.BaseModel):
    """Change commit message with regular expressions -- runs against all
    commits in the history. All fields without suffixes are by default regular
    expressions, corresponding to whom with `_sub` suffixes. Attempting to
    substitute a field with no matching pattern will be ignored.

    Alternatively, a function may be specified for each field, that takes in
    the exact original value and returns the new value, in which case the `_sub`
    would be overridden. This would not be available in JSON-parsed configs."""

    kind: Literal["recommit"] = "recommit"

    tags: Callable[[list[str]], list[str]] | None = None

    author_name: str | Callable[[str], str] | None = None
    author_name_sub: str | None = None

    author_email: str | Callable[[str], str] | None = None
    author_email_sub: str | None = None

    author_date: Callable[[datetime.datetime], datetime.datetime] | None = None

    committer_name: str | Callable[[str], str] | None = None
    committer_name_sub: str | None = None

    committer_email: str | Callable[[str], str] | None = None
    committer_email_sub: str | None = None

    committer_date: Callable[[datetime.datetime], datetime.datetime] | None = None

    msg_subject: str | Callable[[str], str] | None = None
    msg_subject_sub: str | None = None

    msg_body: str | Callable[[str], str] | None = None
    msg_body_sub: str | None = None

    pass


class CloneRule(pydantic.BaseModel):
    """Clone items and re-link them under a same parent directory."""

    kind: Literal["clone"] = "clone"

    pattern: str
    """Glob pattern matching files or directories to be cloned. Path relative
    to the source repository root."""

    target: str
    """Target directory, relative to the target repository root. All cloned
    files will be placed **under** this directory."""

    rename: str | None = None
    """Regular expression pattern matching whichever files to be renamed. This
    will only match the 'filename' part and not the whole path."""
    rename_sub: str | None = None
    """Substitution pattern for renaming. Effective only with `rename`."""

    pass


class RemoveRule(pydantic.BaseModel):
    """Remove files or directories recursively in the target repository during
    the process. Remove rules are executed in order or definition."""

    kind: Literal["remove"] = "remove"

    pattern: str
    """Glob pattern matching files or directories to be removed. The path must
    be relative to the source repository root."""

    pass


MutationRule = FilterRule | ReCommitRule | CloneRule | RemoveRule


class CommitDef(pydantic.BaseModel):
    """Define such a commit in the source repository, that corresponds to a
    known commit in the target repository. This is only useful when you're
    performing a 'rewrite migration' against a partially migrated repository.
    When the new repository is empty, this field is not at all useful."""

    source: CommitHash
    """Source commit hash."""

    target: CommitHash
    """Target commit hash."""

    pass


class CommitChain(pydantic.BaseModel):
    """Describe such a chain of commits in the source repository that shall be
    mutated and thus cloned to the target repository."""

    begin: CommitHash | CommitDef | None
    """The (chronologically) earliest non-occurring commit (exclusive) in the
    chain. This may be viewed as the base or root commit of the mutation. If
    `None` is set, the chain will be considered as a 'dangling' chain where
    root is NIL."""

    end: CommitHash | None
    """The (chronologically) last occurring commit (inclusive) in the chain."""

    pass


class MutationPlan(pydantic.BaseModel):
    """Describe a mutation plan, i.e. what to mutate, in which way and what
    commits will be included in the new repository."""

    source: str
    """Source repository path, may be either absolute or relative."""

    target: str
    """Target repository path, may be either absolute or relative."""

    chain: list[CommitChain]
    """Specify which commits would be copied to the target repo."""

    rules: list[MutationRule]
    """Specify how the history would be mutated."""

    pass


###############################################################################
#   mutation implementation


def __apply_rule(
    rule: MutationRule,
    src_path: pathlib.Path,
    src_parent: CommitHash | None,
    dst_path: pathlib.Path,
    dst_parent: CommitHash | None,
) -> CommitHash | None:
    return


def __rule_exec_filter(rule: FilterRule, commit: GitCommit) -> bool:
    if rule.tags is not None:
        if not rule.tags(commit.tags):
            return False

    if isinstance(rule.author_name, str):
        if not __rule_match(rule.author_name, commit.author_name):
            return False
    elif rule.author_name is not None:
        if not rule.author_name(commit.author_name):
            return False

    if isinstance(rule.author_email, str):
        if not __rule_match(rule.author_email, commit.author_email):
            return False
    elif rule.author_email is not None:
        if not rule.author_email(commit.author_email):
            return False

    if rule.author_date is not None:
        if not rule.author_date(commit.author_date):
            return False

    if isinstance(rule.committer_name, str):
        if not __rule_match(rule.committer_name, commit.committer_name):
            return False
    elif rule.committer_name is not None:
        if not rule.committer_name(commit.committer_name):
            return False

    if isinstance(rule.committer_email, str):
        if not __rule_match(rule.committer_email, commit.committer_email):
            return False
    elif rule.committer_email is not None:
        if not rule.committer_email(commit.committer_email):
            return False

    if rule.committer_date is not None:
        if not rule.committer_date(commit.committer_date):
            return False

    if isinstance(rule.msg_subject, str):
        if not __rule_match(rule.msg_subject, commit.msg_subject):
            return False
    elif rule.msg_subject is not None:
        if not rule.msg_subject(commit.msg_subject):
            return False

    if isinstance(rule.msg_body, str):
        if not __rule_match(rule.msg_body, commit.msg_body):
            return False
    elif rule.msg_body is not None:
        if not rule.msg_body(commit.msg_body):
            return False

    return True


def __rule_exec_recommit(rule: ReCommitRule, commit: GitCommit) -> GitCommit:
    tags = commit.tags
    if rule.tags is not None:
        tags = rule.tags(tags)

    author_name = commit.author_name
    if isinstance(rule.author_name, str):
        author_name = __rule_sub(rule.author_name, rule.author_name_sub, author_name)
    elif rule.author_name is not None:
        author_name = rule.author_name(commit.author_name)

    author_email = commit.author_email
    if isinstance(rule.author_email, str):
        author_email = __rule_sub(
            rule.author_email, rule.author_email_sub, author_email
        )
    elif rule.author_email is not None:
        author_email = rule.author_email(commit.author_email)

    author_date = commit.author_date
    if rule.author_date is not None:
        author_date = rule.author_date(commit.author_date)

    committer_name = commit.committer_name
    if isinstance(rule.committer_name, str):
        committer_name = __rule_sub(
            rule.committer_name, rule.committer_name_sub, committer_name
        )
    elif rule.committer_name is not None:
        committer_name = rule.committer_name(commit.committer_name)

    committer_email = commit.committer_email
    if isinstance(rule.committer_email, str):
        committer_email = __rule_sub(
            rule.committer_email, rule.committer_email_sub, committer_email
        )
    elif rule.committer_email is not None:
        committer_email = rule.committer_email(commit.committer_email)

    committer_date = commit.committer_date
    if rule.committer_date is not None:
        committer_date = rule.committer_date(commit.committer_date)

    msg_subject = commit.msg_subject
    if isinstance(rule.msg_subject, str):
        msg_subject = __rule_sub(rule.msg_subject, rule.msg_subject_sub, msg_subject)
    elif rule.msg_subject is not None:
        msg_subject = rule.msg_subject(commit.msg_subject)

    msg_body = commit.msg_body
    if isinstance(rule.msg_body, str):
        msg_body = __rule_sub(rule.msg_body, rule.msg_body_sub, msg_body)
    elif rule.msg_body is not None:
        msg_body = rule.msg_body(commit.msg_body)

    return GitCommit(
        hash=commit.hash,
        is_head=commit.is_head,
        refs=commit.refs,
        tags=tags,
        author_name=author_name,
        author_email=author_email,
        author_date=author_date,
        committer_name=committer_name,
        committer_email=committer_email,
        committer_date=committer_date,
        msg_subject=msg_subject,
        msg_body=msg_body,
    )


def __rule_exec_clone(rule: CloneRule, source: pathlib.Path, target: pathlib.Path) -> None:
    for g in source.glob(__rule_make_path_relative(rule.pattern)):
        fn = g.name
        if rule.rename is not None:
            fn = __rule_sub(rule.rename, rule.rename_sub, fn)
        shutil.copytree(source / g, target / rule.target / fn)
    return


def __rule_exec_remove(rule: RemoveRule, target: pathlib.Path) -> None:
    removing = list[pathlib.Path]()
    for r in target.glob(__rule_make_path_relative(rule.pattern)):
        removing.append(r)
    for r in removing:
        shutil.rmtree(r, ignore_errors=True)
    return


def __rule_match(regex: str, target: str) -> bool:
    return re.match(regex, target) is not None


def __rule_sub(regex_match: str, regex_sub: str | None, target: str) -> str:
    if regex_sub is None:
        return target
    return re.sub(regex_match, regex_sub, target)


def __rule_make_path_relative(pattern: str) -> str:
    pattern = pattern.lstrip("/")
    pattern = "./" + pattern
    return pattern


###############################################################################
#   main


if __name__ == "__main__":
    print(_parse_git_history(pathlib.Path("."), "HEAD").path)
    pass
