# Bangumi Organizer

Keeps your video files organized in folders.

## Usage

```sh
python3 -m pip install ./requirements.txt
python3 ./mediamgr.py --help
```

The organizer script will always reject conversion unless an exact match (each entry corresponds to a file group and every file corresponds to an entry in the knowledge base).

## Example

A so-called *knowledge base* is used to provide data on existing bangumi series and episodes. It should look resemble the following example:

```yaml
魔女の旅々:
  alt_titles:
    - 'Wandering Witch: The Journey of Elaina'
    - 魔女之旅
  episodes:
    1: 魔女見習いイレイナ
    2: 魔法使いの国
    3: 花のように可憐な彼女
    4: 民なき国の王女
    5: 王立セレステリア
    6: 正直者の国
    7: 旅人が刻む壁 ぶどう踏みの少女
    8: 切り裂き魔
    9: 遡る嘆き
    10: 二人の師匠
    11: 二人の弟子
    12: ありとあらゆるありふれた灰の魔女の物語
    OP1: リテラチュア
    ED1: 灰色のサーガ
可愛いだけじゃない式守さん:
  alt_titles:
    - "Shikimori's Not Just a Cutie"
    - 式守同学不只可爱而已
  episodes:
    1: 僕の彼女はとてもかわいい
    2: 風雲、球技大会！
    3: 不幸のち、晴れ
    4: 立夏、それぞれの想い
    5: ウキウキ川あそび！
    6: 夏ぞ隔たる花火かな
    7: 文化祭Ⅰ
    8: 文化祭Ⅱ
    9: 無邪気さと不器用さ
    10: 勝ちたい気持ち
    11: 可愛いだけじゃない
    12: 夢よりも
```
