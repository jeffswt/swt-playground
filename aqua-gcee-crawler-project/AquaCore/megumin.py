
import argparse
import base64
import json
import os
import requests
import sqlite3
import sys
import time
from typing import List, Union


class Megumin:
    """ A web request handler that works on sqlite as a cache. """
    __instance = None

    def __init__(self):
        self._db_path = './assets/megumin-data.db'
        exists = os.path.exists(self._db_path)
        self._db = sqlite3.connect(self._db_path)
        if not exists:
            self._init_db()
        self._enable_logging = False

    def _log(self, msg):
        if self._enable_logging:
            sys.stderr.write(f'{msg}\n')
        return

    @staticmethod
    def _instance():
        if Megumin.__instance is None:
            Megumin.__instance = Megumin()
        return Megumin.__instance

    def _init_db(self):
        sys.stderr.write('Initializing db... ')
        cur = self._db.cursor()
        cur.execute("""
            CREATE TABLE entries (
                url     TEXT    PRIMARY KEY,
                data    TEXT
            ); """)
        cur.close()
        self._db.commit()
        sys.stderr.write('ok\n')

    def _headers(self):
        return {
            'Accept': 'application/json, text/plain, */*',
            'Accept-Encoding': 'gzip, deflate, br',
            'Accept-Language': 'en,en-US;q=0.9,zh;q=0.8,zh-CN;q=0.7,zh-TW;'
                'q=0.6,ja;q=0.5',
            'Content-Type': 'application/json;charset=UTF-8',
            'Origin': 'https://gkcx.eol.cn',
            'Referer': 'https://gkcx.eol.cn/',
            'sec-ch-ua': '"Microsoft Edge";v="93", " Not;A Brand";v="99", '
                '"Chromium";v="93"',
            'sec-ch-ua-mobile': '?0',
            'sec-ch-ua-platform': '"Windows"',
            'Sec-Fetch-Dest': 'empty',
            'Sec-Fetch-Mode': 'cors',
            'Sec-Fetch-Site': 'same-site',
            'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) '
                'AppleWebKit/537.36 (KHTML, like Gecko) Chrome/93.0.4577.82 '
                'Safari/537.36 Edg/93.0.961.52',
        }

    def _proxies(self):
        return {
            'http': 'socks5://127.0.0.1:2333',
            'https': 'socks5://127.0.0.1:2333',
        }

    def _insert(self, url: str, data: bytes):
        cur = self._db.cursor()
        cur.execute('INSERT INTO entries (url, data) VALUES (?, ?);',
                    (url, base64.b64encode(data).decode('utf-8')))
        cur.close()
        self._db.commit()

    def _lookup(self, url: str) -> Union[bytes, None]:
        cur = self._db.cursor()
        cur.execute('SELECT url, data FROM entries WHERE url = ?;',
                    (url,))
        res = cur.fetchall()
        cur.close()
        return (base64.b64decode(res[0][1].encode('utf-8'))
                if len(res) > 0 else None)

    def _get(self, method, url: str) -> bytes:
        self._log(f'[proc] performing "{method}" on "{url}"')
        cache = self._lookup(url)
        if cache is not None:
            self._log(f'       cache hit, fetching from db')
            return cache
        if method == 'get':
            req = requests.get(url, headers=self._headers(),
                               proxies=self._proxies())
        elif method == 'post':
            req = requests.post(url, headers=self._headers(),
                               proxies=self._proxies())
        # time.sleep(1.0)
        req.encoding = 'utf-8'
        cache = req.content
        self._log(f'       inserting into db')
        self._insert(url, cache)
        return cache

    def _list(self) -> List[str]:
        cur = self._db.cursor()
        cur.execute('SELECT url FROM entries;')
        res = cur.fetchall()
        cur.close()
        return [i[0] for i in res]

    def _delete(self, url: str) -> None:
        cur = self._db.cursor()
        cur.execute('DELETE FROM entries WHERE url = ?', (url,))
        cur.close()
        self._db.commit()

    @staticmethod
    def enable_logging(enable: bool = True) -> None:
        Megumin._instance()._enable_logging = enable

    @staticmethod
    def has(url: str) -> bool:
        return Megumin._instance()._lookup(url) is not None

    @staticmethod
    def get(url: str) -> bytes:
        return Megumin._instance()._get('get', url)
    
    @staticmethod
    def post(url: str) -> bytes:
        return Megumin._instance()._get('post', url)
    
    @staticmethod
    def list() -> List[str]:
        return Megumin._instance()._list()

    @staticmethod
    def delete(url: str) -> List[str]:
        return Megumin._instance()._delete(url)
    pass


def main() -> None:
    # parse args
    parser = argparse.ArgumentParser(description='Megumin Web Cache')
    parser.add_argument('-u', '--url', dest='url', type=str, required=True,
                        help='cache database url key (`list` to show all)')
    parser.add_argument('-d', '--delete', dest='delete', action='store_const',
                        const=True, help='delete url from cache')
    parser.add_argument('-j', '--json', dest='json', action='store_const',
                        const=True, help='auto prettify json')
    parser.add_argument('-x', '--export', dest='export', type=str,
                        help='write to file')
    args = parser.parse_args()
    # check for list action
    if args.url == 'list':
        for x in Megumin.list():
            sys.stderr.write(f'{x}\n')
        return
    # check for delete action
    if args.delete:
        Megumin.delete(args.url)
        sys.stderr.write(f'Successfully deleted entry "{args.url}".\n')
        return
    # get url and its data
    data = Megumin.get(args.url)
    if args.json:
        data = data.decode('utf-8')
        data = json.dumps(json.loads(data), indent=4, ensure_ascii=False)
        data = data.encode('utf-8')
    # export to stdout or file
    if args.export is not None:
        with open(args.export, 'wb') as f:
            f.write(data)
    else:
        sys.stdout.write(data.decode('utf-8'))
    pass


if __name__ == '__main__':
    main()
