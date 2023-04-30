# shift_chunk_y.py: Align Minecraft chunks by their sea levels, then fill the
#                   air that were voided out with stone
# Usage: put this file at plugins/operations/*.py, exposed by the Amulet map
#        editor (https://github.com/Amulet-Team/Amulet-Map-Editor)

from typing import Generator

from amulet.api.block import Block
from amulet.api.selection import SelectionBox, SelectionGroup
from amulet.api.level import BaseLevel
from amulet.api.data_types import ChunkCoordinates, Dimension
import numpy


def shift_chunk_y(
    world: BaseLevel, dimension: Dimension, selection: SelectionGroup, options: dict
) -> Generator[float, None, None]:
    """Align chunk sea levels and fill the lowest layers with given stones."""

    min_y: int = options["Minimum y-level"]
    max_y: int = options["Maximum y-level"]
    sea_level_y: int = options["[Source] Sea level"]
    bedrock_layer_y: int = options["[Source] Bedrock layer y-level"]
    bedrock_layer_height: int = options["[Source] Bedrock layer height"]
    bedrock_layer_block = Block.from_snbt_blockstate(
        options["[Source] Replaces bedrock"]
    )
    target_sea_level_y: int = options["[Target] Sea level"]
    target_fill_block = Block.from_snbt_blockstate(options["[Target] Fills with stone"])
    target_bedrock_layer_height: int = options["[Target] Bedrock layer height"]
    target_bedrock_layer_block = Block.from_snbt_blockstate(
        options["[Target] Fills with bedrock"]
    )
    target_air_block = Block.from_snbt_blockstate(options["[Target] Fills with air"])
    transform_world: bool = options["Batch: Transform entire dimension"]
    continuation_x_z: str = options["Batch: Continuation (x, z)"].strip()
    perform_autosave: bool = options["Batch: Save while processing"]

    # pick chunks
    if not transform_world:
        chunks = get_chunk_selections(selection)
    else:
        chunks = get_world_chunks(world, dimension)
    chunk_selections = chunk_coords_to_selections(
        world, dimension, chunks, min_y, max_y
    )
    print(f"Found {len(chunk_selections)} for processing.")

    # use continuation token for failure recovery
    if continuation_x_z:
        cont_x, cont_z = eval(continuation_x_z)
        if cont_x % 16 != 0 or cont_z % 16 != 0:
            raise ValueError(
                "continuation token must be northwest (-X, -Z) corner of a chunk"
            )
        tmp: list[SelectionBox] = []
        for chunk in chunk_selections:
            if chunk.min_x < cont_x:
                continue
            if chunk.min_x == cont_x and chunk.min_z < cont_z:
                continue
            tmp.append(chunk)
        print(
            f"Applied continuation token ({cont_x}, *, {cont_z}), resuming at {len(tmp)} / {len(chunk_selections)}."
        )
        chunk_selections = tmp

    save_interval = 256
    for chunk_num, chunk in enumerate(chunk_selections):
        for _ in fiddle_chunk(
            world,
            dimension,
            chunk,
            sea_level_y,
            bedrock_layer_y,
            bedrock_layer_height,
            bedrock_layer_block,
            target_sea_level_y,
            target_fill_block,
            target_bedrock_layer_height,
            target_bedrock_layer_block,
            target_air_block,
        ):
            yield (chunk_num + _) / len(chunk_selections)
        # debug print
        chunk_x, chunk_z = chunk.min_x, chunk.min_z
        print(
            f"Processed chunk ({chunk_x}, *, {chunk_z}), {chunk_num} of {len(chunk_selections)}"
        )
        if perform_autosave and chunk_num % save_interval == save_interval - 1:
            print("Saving chunks...")
            world.save()
            world.purge()
    if perform_autosave:
        print("Saving chunks...")
        world.save()
        world.purge()

    yield 1.0


def get_chunk_selections(selection: SelectionGroup) -> list[ChunkCoordinates]:
    """Convert selection group to a minimal list of bounding boxes that as a
    whole completely contains the current selection, and also lies within the
    provided min/max y-range."""

    # 1. merge all selections so that they do not overlap
    selection = selection.merge_boxes()
    # 2. get all the chunk locations that is a superset of this selection
    chunks = selection.chunk_locations()
    # 3. ok
    return list(chunks)


def get_world_chunks(world: BaseLevel, dimension: Dimension) -> list[ChunkCoordinates]:
    """Fetch chunks from the entire dimension."""

    return list(world.all_chunk_coords(dimension))


def chunk_coords_to_selections(
    world: BaseLevel,
    dimension: Dimension,
    coords: list[ChunkCoordinates],
    min_y: int,
    max_y: int,
) -> list[SelectionBox]:
    # 1. de-duplicate coordinates
    coords = list(set(coords))
    # 2. check if coords are in world
    coords = [(cx, cz) for cx, cz in coords if world.has_chunk(cx, cz, dimension)]

    # 3. turn chunk location into chunk boxes
    boxes: list[SelectionBox] = []
    for cx, cz in coords:
        tmp = SelectionBox.create_chunk_box(cx, cz)
        min_x, min_z = tmp.min_x, tmp.min_z
        max_x, max_z = tmp.max_x, tmp.max_z
        # 4. limit chunk selection to provided y-range
        tmp = SelectionBox((min_x, min_y, min_z), (max_x, max_y, max_z))
        boxes.append(tmp)
    # 5. sort and done
    boxes.sort(key=lambda i: (i.min_x, i.min_z))
    return boxes


def fiddle_chunk(
    world: BaseLevel,
    dimension: Dimension,
    chunk: SelectionBox,
    sea_level_y: int,
    bedrock_layer_y: int,
    bedrock_layer_height: int,
    bedrock_layer_block: Block,
    target_sea_level_y: int,
    target_fill_block: Block,
    target_bedrock_layer_height: int,
    target_bedrock_layer_block: Block,
    target_air_block: Block,
) -> Generator[float, None, None]:
    """Process 1 single chunk with provided parameters. Air below the original
    bedrock layer (incl.) will be replaced with the target fill block. The
    bedrock layer would also be restored, generated randomly."""

    min_x, max_x = chunk.min_x, chunk.max_x
    min_y, max_y = chunk.min_y, chunk.max_y
    min_z, max_z = chunk.min_z, chunk.max_z

    # 0. never transform an empty chunk
    if is_empty_selection(world, dimension, chunk):
        yield 1.0
        return

    # 1. replace bedrock layers to target block
    selection = SelectionBox(
        (min_x, bedrock_layer_y, min_z),
        (max_x, bedrock_layer_y + bedrock_layer_height, max_z),
    )
    replace_selection(
        world, dimension, selection, bedrock_layer_block, target_fill_block
    )
    yield 0.25

    # 2. shift original chunk to match sea level
    delta_y = target_sea_level_y - sea_level_y
    for _ in shift_selection(world, dimension, chunk, delta_y):
        yield 0.25 + _ * 0.25
    yield 0.5

    if min_y < bedrock_layer_y + delta_y:
        # CASE: fill underground air with target block
        selection = SelectionBox(
            (min_x, min_y, min_z), (max_x, bedrock_layer_y + delta_y, max_z)
        )
        fill_selection(world, dimension, selection, target_fill_block)
    if delta_y < 0:
        # CASE: dropping sea level, need to trim the top
        selection = SelectionBox(
            (min_x, max_y + delta_y, min_z), (max_x, max_y, max_z)
        )
        air_block = world.block_palette.get_add_block(target_air_block)
        fill_selection(world, dimension, selection, target_air_block)
    yield 0.7

    # 3.5. fill bottom-est bedrock layer
    selection = SelectionBox((min_x, min_y, min_z), (max_x, min_y + 1, max_z))
    fill_selection(world, dimension, selection, target_bedrock_layer_block)
    yield 0.75
    # 4. randomly fill the rest of the bedrock layer(s)
    for y_offset in range(1, target_bedrock_layer_height):
        y = min_y + y_offset
        ratio = y_offset / target_bedrock_layer_height
        selection = SelectionBox((min_x, y, min_z), (max_x, y + 1, max_z))
        randomly_fill(
            world,
            dimension,
            selection,
            target_bedrock_layer_block,
            1 - ratio,
            target_fill_block,
        )
        yield 0.75 + ratio * 0.25

    yield 1.0


def is_empty_selection(
    world: BaseLevel,
    dimension: Dimension,
    selection: SelectionBox,
) -> bool:
    """Check if the selection is completely empty."""

    for chunk, slices, _ in world.get_chunk_slice_box(dimension, selection):
        sub_chunk = numpy.array(chunk.blocks[slices])
        if numpy.any(sub_chunk != 0):
            return False
    return True


def replace_selection(
    world: BaseLevel,
    dimension: Dimension,
    selection: SelectionBox,
    src_block: Block,
    dst_block: Block,
) -> None:
    """Replace all [src_block]s into [dst_block]s from the provided chunk
    selection. The box should always be a vertical slice of a whole chunk."""

    src_id = world.block_palette.get_add_block(src_block)
    dst_id = world.block_palette.get_add_block(dst_block)

    for chunk, slices, _ in world.get_chunk_slice_box(dimension, selection):
        old_blocks = chunk.blocks[slices]
        new_blocks = numpy.array(old_blocks)
        new_blocks[old_blocks == src_id] = dst_id
        chunk.blocks[slices] = new_blocks
        chunk.changed = True
    return


def shift_selection(
    world: BaseLevel, dimension: Dimension, selection: SelectionBox, delta_y: int
) -> Generator[float, None, None]:
    """Move the provided chunk selection up/down by [delta_y] blocks."""

    min_x, max_x = selection.min_x, selection.max_x
    min_y, max_y = selection.min_y, selection.max_y
    min_z, max_z = selection.min_z, selection.max_z
    # to ensure we don't go out-of-bounds
    if delta_y >= 0:
        max_y -= abs(delta_y)
    elif delta_y < 0:
        min_y += abs(delta_y)
    # also to make sure that the selection is still valid
    if min_y >= max_y:
        raise ValueError(f"out-of-bounds: {min_y} >= {max_y}")
    shrunk = SelectionBox((min_x, min_y, min_z), (max_x, max_y, max_z))

    center_x = (shrunk.min_x + shrunk.max_x) // 2
    center_y = (shrunk.min_y + shrunk.max_y) // 2
    center_z = (shrunk.min_z + shrunk.max_z) // 2

    yield from world.paste_iter(
        world,
        dimension,
        SelectionGroup(shrunk),
        dimension,
        (center_x, center_y + delta_y, center_z),
    )
    return


def fill_selection(
    world: BaseLevel, dimension: Dimension, selection: SelectionBox, block: Block
) -> None:
    block_id = world.block_palette.get_add_block(block)
    for chunk, slices, _ in world.get_chunk_slice_box(dimension, selection, True):
        chunk.blocks[slices] = block_id
        chunk.changed = True
    return


def randomly_fill(
    world: BaseLevel,
    dimension: Dimension,
    selection: SelectionBox,
    main_block: Block,
    main_ratio: float,
    sub_block: Block,
) -> None:
    main_id = world.block_palette.get_add_block(main_block)
    sub_id = world.block_palette.get_add_block(sub_block)

    for chunk, slices, _ in world.get_chunk_slice_box(dimension, selection):
        old_blocks = chunk.blocks[slices]
        new_blocks = numpy.array(old_blocks)
        sample = numpy.random.uniform(low=0.0, high=1.0, size=new_blocks.shape)
        new_blocks[:] = main_id
        new_blocks[sample > main_ratio] = sub_id
        chunk.blocks[slices] = new_blocks
        chunk.changed = True
    return


shift_chunk_y_options = {
    "Chunk y-level Shifter": ["label"],
    "Minimum y-level": ["int", -64],
    "Maximum y-level": ["int", 320],
    "[Source] Sea level": ["int", 62],
    "[Source] Bedrock layer y-level": ["int", -64],
    "[Source] Bedrock layer height": ["int", 5],
    "[Source] Replaces bedrock": [
        "str",
        'universal_minecraft:bedrock[infiniburn="false"]',
    ],
    "[Target] Sea level": ["int", 62],
    "[Target] Fills with stone": ["str", 'universal_minecraft:deepslate[axis="y"]'],
    "[Target] Bedrock layer height": ["int", 5],
    "[Target] Fills with bedrock": [
        "str",
        'universal_minecraft:bedrock[infiniburn="false"]',
    ],
    "[Target] Fills with air": ["str", "universal_minecraft:air"],
    "Batch: Transform entire dimension": ["bool", False],
    "Batch: Continuation (x, z)": ["str", ""],
    "Batch: Save while processing": ["bool", False],
}


export = {
    "name": "Shift Chunk Y",
    "operation": shift_chunk_y,
    "options": shift_chunk_y_options,
}
