
import ctypes
import os
import pydantic
from pydantic import BaseModel
import re
import shutil
import subprocess
import time
import tqdm
import typer
from typing import Generator
import yaml


class SessionConfig(BaseModel):
    # Path to the knowledge base YAML file.
    knowledge_base_path: str
    # Relative path(s) to where source files may be found.
    source_files_dirs: list[str]
    # Relative path to where output files are placed.
    output_files_dir: str
    # Whether to wrap up missing files in the error output or not.
    wrap_missing_files: bool
    # Tell conversion rules to not make changes to disk. Default to False.
    preview_only: bool
    # If set to True, all errors will be ignored. Default to False.
    force_ignore_errors: bool
    pass


session_config = SessionConfig(
    knowledge_base_path='',
    source_files_dirs=[],
    output_files_dir='',
    wrap_missing_files=False,
    preview_only=False,
    force_ignore_errors=False,
)


class ProgressBar():
    _bar: tqdm.tqdm | None = None

    @staticmethod
    def bar(replace_with: tqdm.tqdm) -> tqdm.tqdm:
        ProgressBar._bar = replace_with
        return ProgressBar._bar

    @staticmethod
    def print(msg: str) -> None:
        if ProgressBar._bar:
            ProgressBar._bar.write(msg)
        else:
            print(msg)
        return
    pass


###############################################################################
#   Knowledge base


class BangumiItem(BaseModel):
    # Sequence of the episode, can be string sometimes.
    seq: int | str
    # Title of the episode.
    title: str


class Bangumi(BaseModel):
    # The title that will be used when exporting (the real title).
    title: str
    # An optional list of additional episodes that may be used while matching.
    alt_titles: list[str] | None
    # List of episodes in the bangumi.
    episodes: list[BangumiItem]


class UnparsedBangumi(BaseModel):
    alt_titles: list[str] | None
    episodes: dict[int | str, str]  # episode seq -> episode title


KnowledgeBase = list[Bangumi]

UnparsedKnowledgeBase = dict[str, UnparsedBangumi]  # bangumi title -> metadata


def load_knowledge_base(filename: str) -> KnowledgeBase:
    # load raw file
    with open(filename, 'r', encoding='utf8') as f:
        document: UnparsedKnowledgeBase = yaml.safe_load(f)
    # parse document
    bangumis: list[Bangumi] = []
    for title, item_raw in document.items():
        item = UnparsedBangumi(**item_raw)
        episodes: list[BangumiItem] = []
        for seq, ep_title in item.episodes.items():
            episodes.append(BangumiItem(
                seq=seq,
                title=ep_title,
            ))
        bangumis.append(Bangumi(
            title=title,
            alt_titles=item.alt_titles,
            episodes=episodes,
        ))
    # finish
    return bangumis


###############################################################################
#   Conversion infrastructure


cli_app = typer.Typer()


class ConvertDescriptor(BaseModel):
    # Which bangumi does this conversion group belongs to.
    bangumi: Bangumi
    # Which episode does this conversion group belongs to.
    episode: BangumiItem
    # A list of filenames that should be parsed.
    files: list[str]


def get_convert_target_fn(descriptor: ConvertDescriptor) -> str:
    """Turn a conversion descriptor into a target filename (path incl.)."""
    global session_config
    filename = f'{descriptor.episode.seq}. {descriptor.episode.title}.mp4'
    path = Shell.join_path(
        session_config.output_files_dir,
        descriptor.bangumi.title,
        filename
    )
    illegal_chars = ['\\', '/', ':', '*', '!', '?', '"', '<', '>', '|',
                     '·', '...']
    for ch in illegal_chars:
        if ch in filename:
            raise ValueError(f'illegal char "{ch}" in file: "{path}"')
    return path


class ConversionRule():
    def __init__(self):
        return

    def matches(self, filename: str, knowledge_base: KnowledgeBase
                ) -> ConvertDescriptor | None:
        """Validates if a given [filename] provided should be processed with
        the current rule according to the [knowledge_base]. If it is, a list of
        all the files (e.g. audio track & video track) needed to process this
        conversion should be returned. Otherwise return nothing as this rule
        does not match the given file.

        Note that to ensure consistency, it's up to the implementer's duty to
        ensure that for every filename in every [ConvertDescriptor.files]
        result of [matches], running [matches] on that filename should always
        yield the same results (including string order)."""
        raise NotImplementedError()

    def convert(self, descriptor: ConvertDescriptor) -> None:
        """Convert a list of files (filenames are given in [descriptor.files])
        into an output (filename should be derived from [descriptor] using
        [get_convert_target_fn]). It is guaranteed that the [descriptor] comes
        from at least one of the [matches] as a result."""
        raise NotImplementedError()
    pass


class ConversionHelper():
    def __init__(self, knowledge_base: KnowledgeBase, all_files: list[str]):
        global conversion_rules
        self.rules = conversion_rules
        self.knowledge_base = knowledge_base
        # catalog of all available filenames (updated on load)
        self.all_files: list[str] = all_files
        # captures a list of results
        ConversionRecord = tuple[ConversionRule, ConvertDescriptor]
        self.results: dict[str, list[ConversionRecord]] = {}
        # these files / episodes are not matched by any rules
        self.rogue_files: list[str] = []
        self.rogue_episodes: list[tuple[Bangumi, BangumiItem]] = []

    def resolve_all(self) -> list[str]:
        """Run matches between rules and files. A list of errors (if any, or an
        empty list otherwise) will be returned."""
        # retrieve match data
        for fn in self.all_files:
            self.resolve_file(fn)
        # ensure the whole match is sound
        errors: list[str] = []
        for fn in self.all_files:
            err = self.validate_file(fn)
            if err is not None:
                errors.append(err)
        for err in self.validate_knowledge_base():
            errors.append(err)
        # done
        return errors

    def convert_all(self) -> Generator[tuple[int, ConvertDescriptor | None], None, None]:
        """Generator that yields [total_count, None | current_descriptor] while
        running against all of the targets."""
        # pick unique descriptors
        flagged_files: set[str] = set()
        all_records: list[tuple[ConversionRule, ConvertDescriptor]] = []
        for fn, records in self.results.items():
            if fn in flagged_files:
                continue
            flagged_files.add(fn)
            rule, descriptor = records[0]
            all_records.append((rule, descriptor))
        # ignore built targets & descriptor groups
        descriptor_idx: set[ConvertDescriptor] = set()
        filtering_records = all_records
        all_records = []
        for rule, descriptor in filtering_records:
            if self.target_exists(descriptor):
                continue
            if repr(descriptor) in descriptor_idx:
                continue
            all_records.append((rule, descriptor))
            descriptor_idx.add(repr(descriptor))
        # notify listener: found total count
        yield (len(all_records), None)
        # go through every rule
        for rule, descriptor in all_records:
            yield (len(all_records), descriptor)
            self.convert_target(rule, descriptor)
        return

    def resolve_file(self, filename: str) -> None:
        """Match a file against all known rules (no conversion done here)."""
        matched = False
        # perform an O(n*m) from all files against all filenames
        for rule in self.rules:
            result = rule.matches(filename, self.knowledge_base)
            if result is None:
                continue
            for fn in result.files:
                # (self.results[fn] ?? []).push(result)
                descriptors = self.results.get(fn, [])
                descriptors.append((rule, result))
                self.results[fn] = descriptors
            matched = True
        # mark as 'not matched'
        if not matched:
            self.rogue_files.append(filename)
        return

    def validate_file(self, filename: str) -> str | None:
        """Checks if there's something wrong (the resolution results) with a
        given [filename]. Errors will be returned as a string if any."""
        # no matches on this file
        if filename in self.rogue_files:
            return f'no known rule matches file "{filename}"'
        # must have at least one match
        descriptors = self.results[filename]
        if len(descriptors) < 1:
            raise KeyError(f'no descriptors on file {filename}')
        # descriptors must contain this very file
        if filename not in descriptors[0][1].files:
            rule, match = descriptors[0]
            return (
                f'match from [{rule.__name__}] on "{filename}" '
                f'does not contain itself: {match.dict()}'
            )
        # ensure all matched descriptors are mutually equivalent
        for i in range(1, len(descriptors)):
            (lhv_rule, lhv), (rhv_rule, rhv) = descriptors[0], descriptors[i]
            if lhv != rhv:
                lhv_dict, rhv_dict = lhv.dict(), rhv.dict()
                lhv_name, rhv_name = lhv_rule.__name__, rhv_rule.__name__
                return (
                    f'match from [{lhv_name}] differs from [{rhv_name}] '
                    f'on file "{filename}": '
                    f'{lhv_dict} != {rhv_dict}'
                )
            pass
        # search for missing files
        for fn in descriptors[0][1].files:
            if fn not in self.all_files:
                return f'target "{filename}" depends on "{fn}" but is missing'
        # no errors found
        return

    def validate_knowledge_base(self) -> list[str]:
        """Checks if anything is wrong with the knowledge base. All errors will
        be returned in a list (if any, or an empty list otherwise)."""
        # build scan index & error results
        resolved_eps: dict[tuple[str, int | str], ConvertDescriptor] = {}
        targets: dict[str, ConvertDescriptor] = {}
        errors: list[str] = []
        # scan all file -- kbase matches
        for _fn, records in self.results.items():
            for _rule, descriptor in records:
                title = descriptor.bangumi.title
                seq = descriptor.episode.seq
                resolved_eps[(title, seq)] = descriptor
                # check if multiple descriptors point to the same target
                target = get_convert_target_fn(descriptor)
                if target not in targets or targets[target] == descriptor:
                    targets[target] = descriptor
                else:
                    errors.append(
                        f'conflict targets: '
                        f'[{title}]::[{seq}]::[{descriptor.episode.title}]'
                    )
                    related_files: set[str] = set()
                    for f in descriptor.files:
                        related_files.add(f)
                    for f in targets[target].files:
                        related_files.add(f)
                    for f in related_files:
                        errors.append(f'  - {f}')
                pass
        # O(n*m) scan for missing ones
        for bangumi in self.knowledge_base:
            for episode in bangumi.episodes:
                # check if item exists
                title, seq = bangumi.title, episode.seq
                if (title, seq) in resolved_eps:
                    continue
                # check if output file exists
                sentinel = ConvertDescriptor(
                    bangumi=bangumi,
                    episode=episode,
                    files=[],
                )
                if self.target_exists(sentinel):
                    continue
                # error: rogue target
                errors.append(
                    f'missing files for target: '
                    f'[{title}]::[{seq}]::[{episode.title}]'
                )
        # finish
        return errors

    def target_exists(self, descriptor: ConvertDescriptor) -> bool:
        """Checks if a build target is existent."""
        output_fn = get_convert_target_fn(descriptor)
        return os.path.exists(output_fn)

    def convert_target(self, rule: ConversionRule,
                       descriptor: ConvertDescriptor) -> None:
        """Run conversion on rule."""
        rule.convert(descriptor)
    pass


def run_conversion() -> None:
    """Reads configuration and runs the conversion."""
    global session_config
    # load knowledge base
    knowledge_base = load_knowledge_base(session_config.knowledge_base_path)
    # load files
    all_files: list[str] = []
    for source_dir in session_config.source_files_dirs:
        for path, _, files in os.walk(source_dir):
            for fn in files:
                all_files.append(Shell.join_path(path, fn))
    # spin up helper
    helper = ConversionHelper(knowledge_base, all_files)

    # start parsing files
    errors = helper.resolve_all()
    if errors:
        print(f'fatal error: conversion cannot continue because:')
        print(f'--------------------------------------------------------')
        has_missing_files = False
        for err in errors:
            if err.startswith('missing files for target:'):
                if session_config.wrap_missing_files:
                    has_missing_files = True
                    continue
            print(f'  - {err}')
        if has_missing_files:
            print('  - some targets are missing required dependencies.')
        # check if user had set it to 'force run' mode
        if not session_config.force_ignore_errors:
            return
        else:
            print('WARNING: ignoring errors and performing force conversion ( )', end='')
            for i in range(5):
                print(f'\b\b\b({5 - i})', end='')
                time.sleep(1.0)
            print('')
        pass
    # run conversion
    progress_bar: tqdm.tqdm | None = None
    for total, descriptor in helper.convert_all():
        descriptor: ConvertDescriptor
        if progress_bar is None:
            progress_bar = ProgressBar.bar(tqdm.tqdm(total=total, unit='files'))
            continue
        # debug print conversion
        lines = [f'Converted file "{get_convert_target_fn(descriptor)}" from:']
        for fn in descriptor.files:
            lines.append(f'  - {fn}')
        if progress_bar is not None:
            for line in lines:
                progress_bar.write(line)
            progress_bar.update(1)
        pass
    # finish up
    if progress_bar is not None:
        progress_bar.close()
    return


@cli_app.command()
def convert(knowledge_base_yaml: str,
            from_files: list[str],
            out_dir: str,
            wrap_missing_files: bool = False,
            preview_only: bool = False,
            force_ignore_errors: bool = False) -> None:
    global session_config
    session_config.knowledge_base_path = knowledge_base_yaml
    session_config.source_files_dirs = from_files
    session_config.output_files_dir = out_dir
    session_config.wrap_missing_files = wrap_missing_files
    session_config.preview_only = preview_only
    session_config.force_ignore_errors = force_ignore_errors
    run_conversion()


###############################################################################
#   Shell operations & misc


class Shell():
    @staticmethod
    def join_path(*components: list[str]) -> str:
        return os.path.join(*components).replace('\\', '/')

    @staticmethod
    def move(source: str, target: str) -> None:
        # preview mode
        if session_config.preview_only:
            ProgressBar.print(f'  $ mv "{source}" "{target}"')
            return
        # create folders on fs tree
        dir_path = os.path.dirname(target)
        os.makedirs(dir_path, exist_ok=True)
        # ensure that the 'finished' marker be hidden if the move operation
        # stopped halfway through
        temp_target = f'{target}.moving'
        if os.path.exists(temp_target):
            raise RuntimeError(f'temporary file exists: {temp_target}')
        if os.path.exists(target):
            raise RuntimeError(f'target file exists: {target}')
        shutil.move(source, temp_target)
        # finish it up quickly as it's on the same drive
        if os.path.exists(target):
            raise RuntimeError(f'target file exists: {target}')
        time.sleep(0.1)
        shutil.move(temp_target, target)
    pass


class FFmpegExecutor():
    def __init__(self, *args: tuple[str], show_progress=False):
        self._args = args
        self._show_progress = show_progress
        self._progress_bar: tqdm.tqdm | None = None
        self._progress: float = 0.0

    def run(self) -> None:
        # disable windowed mode, ignore crash
        SEM_NOGPFAULTERRORBOX = 0x0002
        CREATE_NO_WINDOW = 0x08000000
        ctypes.windll.kernel32.SetErrorMode(SEM_NOGPFAULTERRORBOX)
        platform_subprocess_flags = CREATE_NO_WINDOW
        # spawn process
        process = subprocess.Popen(
            self._args,
            creationflags=platform_subprocess_flags,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE)
        # listen for updates
        for line in self._readlines(process):
            # matching total duration
            patt_duration = r'^Duration: (\d+:\d{2}:\d{2}\.\d+)'
            match_duration = re.findall(patt_duration, line)
            if match_duration and not self._progress_bar:
                duration = self._duration_to_seconds(match_duration[0])
                self._create_progress_bar(duration)
            # matching current progress
            patt_progress = r'^frame *= *\d+.*?time *= *(\d+:\d{2}:\d{2}\.\d+).*?x'
            match_progress = re.findall(patt_progress, line)
            if match_progress:
                position = self._duration_to_seconds(match_progress[0])
                self._write_progress(position)
            pass
        # finish up results
        self._close_progress_bar()
        return

    def _readlines(self, process: subprocess.Popen
                   ) -> Generator[str, None, None]:
        """Sequentially read lines from the process's `stderr` handle."""
        line_buffer = b''
        while True:
            ch = process.stderr.read(1)
            # process terminated
            if not ch:
                break
            # append char to buffer
            if ch not in {b'\r', b'\n'}:
                line_buffer += ch
                continue
            # line created
            line = line_buffer.decode('utf-8', 'ignore').strip()
            line_buffer = b''
            if line:
                yield line
            pass
        # flush buffer
        line = line_buffer.decode('utf-8', 'ignore').strip()
        if line:
            yield line
        return

    def _duration_to_seconds(self, duration: str) -> float:
        """Convert 'HH:MM:SS.zzz' format duration into seconds as a decimal."""
        h, m, s = duration.strip().split(':')
        return int(h) * 3600 + int(m) * 60 + float(s)

    def _create_progress_bar(self, duration: float) -> None:
        if not self._show_progress:
            return
        self._close_progress_bar()
        self._progress_bar = tqdm.tqdm(total=duration, leave=False, unit='sec')
        self._progress = 0.0
        return

    def _write_progress(self, position: float) -> None:
        if not self._show_progress:
            return
        if not self._progress_bar:
            return
        increment = position - self._progress
        if increment <= 0:
            return
        self._progress_bar.update(increment)
        self._progress = increment
        return

    def _close_progress_bar(self) -> None:
        if not self._show_progress:
            return
        if self._progress_bar:
            self._progress_bar.close()
            self._progress_bar = None
        self._progress = 0.0
        return
    pass


class FFmpegFinder():
    ffmpeg_path: str | None = None

    @staticmethod
    def get() -> str:
        """Looks up FFmpeg binary path with cache. Throws exception if no such
        binary was found."""
        if FFmpegFinder.ffmpeg_path is not None:
            return FFmpegFinder.ffmpeg_path
        FFmpegFinder.ffmpeg_path = FFmpegFinder.get_uncached()
        if not FFmpegFinder.ffmpeg_path:
            raise KeyError('missing ffmpeg binary')
        return FFmpegFinder.ffmpeg_path

    @staticmethod
    def get_uncached() -> str | None:
        """Tries to lookup for a FFmpeg binary path."""
        # define list of known paths
        known_paths = [
            'ffmpeg',
            './ffmpeg',
            './ffmpeg.exe',
            '/bin/ffmpeg',
            '/usr/bin/ffmpeg',
        ]
        # Generate rules to lookup the binary
        drives = [chr(ord('A') + i) for i in range(26)]  # A:/ to Z:/
        program_dirs = [
            'Program Files',
            'Program Files (x86)',
            'Programs',
            'ProgramData',
        ]
        ffmpeg_dirs = [
            'ffmpeg',
            'FFmpeg',
        ]
        bin_path = 'bin/ffmpeg.exe'
        for drive in drives:
            for program_dir in program_dirs:
                for ffmpeg_dir in ffmpeg_dirs:
                    path = f'{drive}:/{program_dir}/{ffmpeg_dir}/{bin_path}'
                    known_paths.append(path)
        # verify for files and whether they exists
        for path in known_paths:
            if os.path.exists(path):
                return path
        return None
    pass


###############################################################################
#   Conversion logics


class RenameRule(ConversionRule):
    def matches(self, filename: str, knowledge_base: KnowledgeBase
                ) -> ConvertDescriptor | None:
        found_bangumi: Bangumi | None = None
        for bangumi in knowledge_base:
            for episode in bangumi.episodes:
                # build known patterns
                seq_2d = self.pad_float(episode.seq)
                patterns: list[str] = []
                for alt_title in bangumi.alt_titles:
                    patterns.append(f'{alt_title} - {seq_2d} ')
                    patterns.append(f'{alt_title}[{seq_2d}]')
                    patterns.append(f'{alt_title} 第{seq_2d}集')
                # try to pair up a match
                match = False
                for pattern in patterns:
                    if pattern in filename:
                        match = True
                # send result on match
                if not match:
                    continue
                return ConvertDescriptor(
                    bangumi=bangumi,
                    episode=episode,
                    files=[filename],
                )
        return None

    def convert(self, descriptor: ConvertDescriptor) -> None:
        source = descriptor.files[0]
        target = get_convert_target_fn(descriptor)
        Shell.move(source, target)
        time.sleep(0.1)
        return

    def pad_float(self, num: int | str) -> str:
        num = str(num)
        if not re.findall(r'^\d+(\.\d+)?$', num):
            return num
        if num.isnumeric():
            return num.rjust(2, '0')
        i, f = num.split('.')
        return i.rjust(2, '0') + '.' + f
    pass


# give a list of conversion rules (instances) here... i'm too lazy to implement
# a metaclass that does registers upon inheritance
conversion_rules: list[ConversionRule] = [
    RenameRule(),
]


###############################################################################
#   Main executor


if __name__ == '__main__':
    cli_app()
