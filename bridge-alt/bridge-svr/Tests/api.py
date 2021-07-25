
import json
import os
import random
import re
import requests
import shutil
import sys
from typing import Any, Dict, List, Tuple, Union
import unittest
import urllib3


class Folders:
    Tree = Dict

    @staticmethod
    def _folder_stub() -> Tree:
        """ A sample of the 'folder' definition. """
        return {
            'parent1': {
                'subparent': {
                    'file.txt': 2000,  # 2 KB
                    'empty.file': 0,  # empty file
                },
                'subparent_other': {
                    'word.docx': 10,
                    'slides.pptx': 20,
                },
            },
            'parent2': {},
        }

    @staticmethod
    def make(folders: Tree, cwd: str = '.') -> None:
        """ Apply folder definition to local filesystem, root at [cwd]. """
        for name, sub in folders.items():
            path = cwd + '/' + name
            if isinstance(sub, int):
                # write to file with size [sub]
                with open(path, 'wb') as f:
                    f.write(b'x' * sub)
            else:
                # iterate next level directory
                os.makedirs(path)
                Folders.make(sub, cwd=path)
        return

    @staticmethod
    def check(folders: Tree, cwd: str = '.') -> bool:
        """ Check if local filesystem structure under root [cwd] is identical
        to that specified in [folders]. """
        for name, sub in folders.items():
            path = cwd + '/' + name
            if isinstance(sub, int):
                # check file size
                if not os.path.isfile(path):
                    return False
                if os.path.getsize(path) != sub:
                    return False
            else:
                # must be directory
                if not os.path.isdir(path):
                    return False
                # subdirectory must contain exactly the same items
                if len(os.listdir(path)) != len(sub):
                    return False
                # recursive check
                if not Folders.check(sub, path):
                    return False
        return True

    def purge(folders: Tree, cwd: str = '.') -> None:
        """ Purge folders in the [folders] tree, rooting at [cwd]. If there
        are other files in those folders, they will be purged as well. """
        for name, sub in folders.items():
            path = cwd + '/' + name
            shutil.rmtree(name, ignore_errors=True)

    class keep:
        """ Creates some folders, checks validity of expected folders and
        removes them upon leave. """
        def __init__(self, make: Dict, check: Dict, cwd: str = '.'):
            self._make = make
            self._check = check
            self._cwd = cwd

        def __enter__(self):
            Folders.purge(self._make, cwd=self._cwd)
            Folders.purge(self._check, cwd=self._cwd)
            Folders.make(self._make, cwd=self._cwd)

        def __exit__(self, _1, _2, _3):
            flag = Folders.check(self._check, cwd=self._cwd)
            tree = Folders.get(cwd=self._cwd)
            Folders.purge(self._make, cwd=self._cwd)
            Folders.purge(self._check, cwd=self._cwd)
            if not flag:
                raise RuntimeError('Failed filesystem structure check: %s' %
                                   json.dumps(tree, indent=4))
        pass

    def get(cwd: str = '.') -> Tree:
        """ Get hierarchy from local filesystem. """
        tr = {}
        for fn in os.listdir(cwd):
            path = cwd + '/' + fn
            if os.path.isfile(path):
                tr[fn] = os.path.getsize(path)
            else:
                tr[fn] = Folders.get(path)
        return tr

    def to_bridge(folders: Tree) -> List[Dict]:
        """ Convert filesystem hierarchy to List<DirectoryDescription>. """
        return list(map(lambda kv: {
            'isFile': isinstance(kv[1], int),
            'name': kv[0],
            'size': kv[1] if isinstance(kv[1], int) else None,
        }, folders.items()))
    pass


class Bridge:
    @staticmethod
    def _api_location() -> str:
        return 'https://127.0.0.1:5001'

    @staticmethod
    def post(endpoint: str, payload: Dict) -> Dict:
        """ Use post on Bridge API. """
        urllib3.disable_warnings()
        req = requests.post(Bridge._api_location() + endpoint, json=payload,
                            verify=False)
        if not 200 <= req.status_code < 300:
            raise RuntimeError('Bad status code %d' % req.status_code)
        return json.loads(req.text)

    @staticmethod
    def path(relpath: str) -> str:
        """ Get absolute path on Bridge with [relpath] relative to CWD. """
        path = os.getcwd() + '/' + relpath
        path = re.sub(r'[/\\]+', r'/', path)
        if 'win' in sys.platform:
            path = '/' + path[0].lower() + path[2:]
        return path
    pass


class TestApiRobustness(unittest.TestCase):
    def make_sample_folder(self):
        return {
            'root_folder_1': {
                'alice': {
                    'synthesis': {
                        'thirty.txt': 30,
                    },
                    'kishi.rar': 2333,
                },
                'asuna.a': 666,
                'kirito.a': 6666,
            },
            'root_folder_2': {},
            'test.jar': 1234,
        }

    def test_listdir(self):
        sample = self.make_sample_folder()
        make = {'__test_ls': sample}
        with Folders.keep(make, make) as _:
            f = Bridge.path('/__test_ls')
            j = Bridge.post('/api/files/list', {'loc': f})
            self.assertEqual(j['path'], f)
            self.assertEqual(j['files'], Folders.to_bridge(sample))
    
    def test_copy(self):
        sample = self.make_sample_folder()
        make = {'__test_cp_src': sample}
        check = {'__test_cp_src': sample, '__test_cp_dst': sample}
        with Folders.keep(make, check) as _:
            f1 = Bridge.path('/__test_cp_src')
            f2 = Bridge.path('/__test_cp_dst')
            j = Bridge.post('/api/files/copy', {'src': f1, 'dest': f2})

    def test_move(self):
        sample = self.make_sample_folder()
        make = {'__test': {'mv_src': sample}}
        check = {'__test': {'mv_dst': sample}}
        with Folders.keep(make, check) as _:
            f1 = Bridge.path('/__test/mv_src')
            f2 = Bridge.path('/__test/mv_dst')
            j = Bridge.post('/api/files/move', {'src': f1, 'dest': f2})

    def test_delete(self):
        make = {'__test': {'to_del': self.make_sample_folder()}}
        check = {'__test': {}}
        with Folders.keep(make, check) as _:
            f = Bridge.path('/__test/to_del')
            j = Bridge.post('/api/files/delete', {'loc': f})
    pass


if __name__ == '__main__':
    unittest.main()
