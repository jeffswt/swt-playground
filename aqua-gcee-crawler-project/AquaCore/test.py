
import asyncio
import bs4
from dataclasses import dataclass
import json
import lxml
import os
from playwright.async_api import async_playwright
import shutil
import time
import tqdm
from typing import List, Set, Union


def parse_int(s: str) -> Union[int, None]:
    try:
        return int(s)
    except Exception:
        return None
    pass


async def sleep_until(timestamp: float) -> None:
    tm = time.time()
    if tm >= timestamp:
        return
    await asyncio.sleep(timestamp - tm)


@dataclass
class ScoreEntry:
    university: str  # 高校名称
    major: str  # 专业名称
    mean: Union[int, None]  # 平均分
    top: Union[int, None]  # 最高分
    region: str  # 生源地
    category: str  # 科别
    year: Union[int, None]  # 报考年份
    batch: Union[str, None]  # 批次

    def to_json(self):
        return {
            'university': self.university,
            'major': self.major,
            'mean': self.mean,
            'top': self.top,
            'region': self.region,
            'category': self.category,
            'year': self.year,
            'batch': self.batch,
        }
    pass


@dataclass
class ScoreEntryIndex:
    university: str  # 高校名称
    major: str  # 专业名称
    region: str  # 生源地
    category: str  # 科别
    year: Union[int, None]  # 报考年份
    batch: Union[str, None]  # 批次

    def __hash__(self):
        return hash(repr({
            '__type__': 'ScoreEntryIndex',
            'university': self.university,
            'major': self.major,
            'region': self.region,
            'category': self.category,
            'year': self.year,
            'batch': self.batch,
        }))
    pass


class Crawler:
    async def crawl_page(self, page, pageid: int) -> List[ScoreEntry]:
        # initialize playwright
        await page.goto(f'http://college.gaokao.com/spepoint/a1/p{pageid}/')
        # extract element
        table = await page.inner_html('table')
        dom = bs4.BeautifulSoup(table, 'lxml')
        # parse rows
        entries: List[ScoreEntry] = []
        for row in dom.find_all('tr')[1:]:
            values = [td.text.strip() for td in row.find_all('td')]
            entries.append(ScoreEntry(
                university=values[1],
                major=values[0],
                mean=parse_int(values[2]),
                top=parse_int(values[3]),
                region=values[4],
                category=values[5],
                year=parse_int(values[6]),
                batch=None if values[7].startswith('---') else values[7],
            ))
        return entries

    def _add_entries(self, entries: List[ScoreEntry],
                     index: Set[ScoreEntryIndex],
                     increment: List[ScoreEntry]) -> None:
        for x in increment:
            i = ScoreEntryIndex(
                university=x.university,
                major=x.major,
                region=x.region,
                category=x.category,
                year=x.year,
                batch=x.batch,
            )
            if i not in index:
                index.add(i)
                entries.append(x)
        return

    def _dbname(self, temp: bool = False) -> str:
        if temp:
            return 'score-entry-db.json.new'
        return 'score-entry-db.json'

    async def _load_backup(self) -> List[ScoreEntry]:
        if not os.path.exists(self._dbname()):
            return []
        entries: List[ScoreEntry] = []
        with open(self._dbname(), 'r', encoding='utf-8') as f:
            for l in f.readlines():
                l = l.strip()
                if not l:
                    continue
                entries.append(ScoreEntry(**json.loads(l)))
        return entries

    async def _save_backup(self, entries: List[ScoreEntry]) -> None:
        with open(self._dbname(temp=True), 'w', encoding='utf-8') as f:
            for entry in entries:
                f.write(json.dumps(entry.to_json(), ensure_ascii=False) + '\n')
        if os.path.exists(self._dbname()):
            os.remove(self._dbname())
        os.rename(self._dbname(temp=True), self._dbname())
        return

    async def _run(self, startpage: int):
        entries: List[ScoreEntry] = []
        index: Set[ScoreEntryIndex] = set()
        self._add_entries(entries, index, await self._load_backup())
        tot_pages = 1818
        async with async_playwright() as p:
            # load browser context
            browser = await p.chromium.launch(
                # proxy={
                #     'server': 'socks5://127.0.0.1:2333',
                # },
            )
            page = await browser.new_page()
            bar = tqdm.tqdm(total=tot_pages, initial=startpage - 1)
            # iterate pages
            for pg in range(startpage, tot_pages + 1):
                tm_begin = time.time()
                self._add_entries(entries, index,
                                  await self.crawl_page(page, pg))
                if pg % 7 == 0:
                    await self._save_backup(entries)
                bar.update(1)
                await sleep_until(tm_begin + 1.5)
            # finalize
            bar.close()
            await page.close()
            await browser.close()
        await self._save_backup(entries)
        return

    def run(self, startpage: int):
        asyncio.run(self._run(startpage))
    pass


if __name__ == '__main__':
    Crawler().run(1810)
