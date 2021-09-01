
import argparse
import os
import re
import shutil
import subprocess
import time
from typing import Callable, List, Tuple, Union
import win32api


Path = str
CommitHash = str
CommitTime = float
AuthorName = str
AuthorEmail = str
CommitMsg = str
Commit = Tuple[AuthorName, AuthorEmail, CommitMsg]
FullCommit = Tuple[CommitHash, CommitTime, Commit]
CommitModifier = Callable[[Commit], Commit]

class Amender:
    """ Amend commits of a Git repository into an another EMPTY Git repository
    (must be an already-initialized repo). Currently does not support Git
    submodules. """

    def __init__(self, src: Path, dest: Path, modifier: CommitModifier):
        self._src = src
        self._dest = dest
        self._modifier = modifier
        return

    def _set_time(self, tm: float) -> None:
        """ Sets system time. """
        t = time.gmtime(tm)
        win32api.SetSystemTime(t.tm_year, t.tm_mon, t.tm_wday, t.tm_mday,
                               t.tm_hour, t.tm_min, t.tm_sec, 15)
        return

    def _regpath(self, *paths):
        """ Join paths into a regularized format. ('/' as delimiters and no
        consequent duplicate delimiters, also have no trailing '/'s.) """
        path = re.sub(r'[\\/]+', r'/', '/'.join(paths))
        return re.sub(r'/*$', r'', path)

    def fullcopy(self) -> None:
        """ Empties target repo and copies all files from source repo to target
        repo. Leaving '.git' directories ignored. """
        # empty directory
        os.chdir(self._dest)
        for path in os.listdir('.'):
            if path != '.git':
                if os.path.isdir(path):
                    shutil.rmtree(path)
                else:
                    os.remove(path)
        # copy from source to target
        os.chdir(self._src)
        for path in os.listdir('.'):
            if path == '.git':
                continue
            src = self._regpath(self._src, path)
            dest = self._regpath(self._dest, path)
            if os.path.isdir(src):
                shutil.copytree(src, dest)
            else:
                shutil.copyfile(src, dest)
        return

    def get_history_raw(self) -> str:
        """ Gets raw git repo history (in string output). Must first change to
        the repo directory before using it. """
        proc = subprocess.Popen(
            args=['git', 'log'],
            stdout=subprocess.PIPE)
        x = proc.stdout.read()
        proc.wait()
        return x.decode('utf-8', 'ignore')

    def _resolve_commit(self, history: List[str], ptr: int
                        ) -> Tuple[int, Union[FullCommit, None]]:
        """ Resolves at most 1 commit from git history and forwards line read
        pointer for next resolution. """
        # find commit line
        while ptr < len(history) and not history[ptr].startswith('commit '):
            ptr += 1
        if ptr >= len(history):
            return ptr, None
        chash = re.sub(r'^commit +', r'', history[ptr]).strip().lower()
        ptr += 1
        # get author line
        name, email = re.findall(r'^Author: +(.*) +<(.*)>$', history[ptr])[0]
        ptr += 1
        # get date
        date = re.findall(r'^Date: +(.*?)$', history[ptr])[0]
        *objs, tz = date.split(' ')
        tm = time.strptime(' '.join(objs))
        tm = time.mktime(tm)
        ptr += 2
        # get commit message
        msgs: List[str] = []
        while ptr < len(history) and history[ptr] != '':
            msgs.append(history[ptr][4:])
            ptr += 1
        # assemble message
        return ptr, (chash, tm, (name, email, '\n'.join(msgs)))

    def get_history(self) -> List[FullCommit]:
        """ Gets full commit information from git repo. Must first change
        working directory to the git repo before using this. """
        history = self.get_history_raw().splitlines()
        commits: List[FullCommit] = []
        ptr: int = 0
        while ptr < len(history):
            ptr, commit = self._resolve_commit(history, ptr)
            if commit is not None:
                commits.append(commit)
        return commits[::-1]

    def commit(self, args: Commit) -> None:
        """ Perform commit in CWD repository. """
        _1, _2, msg = args
        proc = subprocess.Popen(args=['git', 'commit', '-am', msg])
        proc.wait()
        return

    def rebase(self) -> None:
        """ Perform rebase. """
        os.chdir(self._src)
        fcommits = self.get_history()
        last_user, last_email = None, None
        for _cntr, (chash, ctime, commit) in enumerate(fcommits):
            # print message
            msgfirstline = commit[2].split("\n")[0]
            print(f'\n\n\n\n', end='')
            print(f'-' * 72)
            print(f'|')
            print(f'|   ({_cntr + 1}/{len(fcommits)}) {msgfirstline}')
            print(f'|')
            print(f'-' * 72)
            # fast-forward to old commit
            os.chdir(self._src)
            os.system(f'git checkout {chash}')
            # copy files to target
            self.fullcopy()
            # generated amended commit
            amended = self._modifier(commit)
            # add files to repository
            os.chdir(self._dest)
            os.system('git add .')
            os.system('git add *')
            os.system('git add ./*')
            # set current user and email
            if amended[0] != last_user:
                last_user = amended[0]
                os.system(f'git config user.name "{last_user}"')
            if amended[1] != last_email:
                last_email = amended[1]
                os.system(f'git config user.name "{last_email}"')
            # reset system time
            self._set_time(ctime)
            # commit files
            self.commit(amended)
        return
    pass


if __name__ == '__main__':
    # specify arguments
    parser = argparse.ArgumentParser(description='Git repo rebaser.')
    parser.add_argument('--src', dest='srcrepo', type=str, required=True,
                        help='absolute path to source repository.')
    parser.add_argument('--from-name', dest='srcname', type=str, required=True,
                        help='only change commits matching this author.')
    parser.add_argument('--from-email', dest='srcmail', type=str, required=True,
                        help='only change commits matching this email.')
    parser.add_argument('--dest', dest='dstrepo', type=str, required=True,
                        help='absolute path to destination repository.')
    parser.add_argument('--to-name', dest='dstname', type=str, required=True,
                        help='change commits to this author.')
    parser.add_argument('--to-email', dest='dstmail', type=str, required=True,
                        help='change commits to this email.')
    args = parser.parse_args()
    # execute rebase operation
    modifier = (lambda commit: commit
                if commit[0] != args.srcname or commit[1] != args.srcmail
                else (args.dstname, args.dstmail, commit[2]))
    Amender(
        src = args.srcrepo,
        dest = args.dstrepo,
        modifier = modifier,
    ).rebase()
