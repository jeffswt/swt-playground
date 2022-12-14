
import os
import pydantic
from pydantic import BaseModel
import shutil
import tqdm
import typer
import yaml


class SessionConfig(BaseModel):
    # Path to the knowledge base YAML file.
    knowledge_base_path: str
    # Relative path(s) to where source files may be found.
    source_files_dirs: list[str]
    # Relative path to where output files are placed.
    output_files_dir: str
    pass


session_config = SessionConfig(
    knowledge_base_path='',
    source_files_dirs=[],
    output_files_dir='',
)


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
    return Shell.join_path(
        session_config.output_files_dir,
        descriptor.bangumi.title,
        f'{descriptor.episode.title}.mp4',
    )


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

    def convert_all(self) -> tuple[int, ConvertDescriptor | None]:
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
        # ignore built targets
        all_records = [(rule, descriptor) for rule, descriptor in all_records
                       if self.target_exists(descriptor)]
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
        errors: list[str] = []
        # scan all file -- kbase matches
        for _fn, records in self.results.items():
            for _rule, descriptor in records:
                title = descriptor.bangumi.title
                seq = descriptor.episode.seq
                resolved_eps[(title, seq)] = descriptor
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
        for err in errors:
            print(f'  - {err}')
        return
    # run conversion
    progress_bar: tqdm.tqdm | None = None
    for total, descriptor in helper.convert_all():
        descriptor: ConvertDescriptor
        if progress_bar is None:
            progress_bar = tqdm.tqdm(total=total, unit='files')
            continue
        # debug print conversion
        lines = [f'Converted file "{get_convert_target_fn(descriptor)}" from:']
        for fn in descriptor.files:
            lines.append(f'  - {fn}')
        if progress_bar is not None:
            for line in lines:
                progress_bar.write(line)
        pass
    # finish up
    if progress_bar is not None:
        progress_bar.close()
    return


@cli_app.command()
def convert(knowledge_base_yaml: str, from_files: list[str], out_dir: str) -> None:
    global session_config
    session_config.knowledge_base_path = knowledge_base_yaml
    session_config.source_files_dirs = from_files
    session_config.output_files_dir = out_dir
    run_conversion()


###############################################################################
#   Shell operations & misc


class Shell():
    @staticmethod
    def join_path(*components: list[str]) -> str:
        return os.path.join(*components).replace('\\', '/')

    @staticmethod
    def move(source: str, target: str) -> None:
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
        shutil.move(temp_target, target)
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
                seq_2d = (str(episode.seq).rjust(2, '0')
                          if not isinstance(episode.seq, str)
                          else episode.seq)
                patterns: list[str] = []
                for alt_title in bangumi.alt_titles:
                    patterns.append(f'{alt_title} - {seq_2d}')
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
