
import json
from typing import Any, Callable, Dict, Iterable, List, Set, Union


class SerializableDataclass:
    pass


def serialize(obj) -> Union[str, Dict]:
    """ iteratively serialize a SerializableDataclass object """
    result = obj
    if isinstance(obj, SerializableDataclass):
        result = {k: serialize(v) for k, v in obj.__dict__.items()}
    elif type(obj) == dict:
        result = {k: serialize(v) for k, v in obj.items()}
    elif type(obj) == list:
        result = [serialize(v) for v in obj]
    return result


class LinqObj:
    def __init__(self, values: Iterable[Any]):
        self._values = list(values)

    def __repr__(self) -> str:
        return f'linq{repr(self._values)}'

    def __str__(self) -> str:
        return str(self._values)

    def any(self, predicate: Callable[[Any], bool]) -> bool:
        """ .any(lambda val: ...) """
        for val in self._values:
            if predicate(val):
                return True
        return False

    def any_idx(self, predicate: Callable[[int, Any], bool]) -> bool:
        """ .any_idx(lambda idx, val: ...) """
        for idx, val in self._values:
            if predicate(idx, val):
                return True
        return False

    def all(self, predicate: Callable[[Any], bool]) -> bool:
        """ .all(lambda val: ...) """
        return not self.any(lambda val: not predicate(val))

    def all_idx(self, predicate: Callable[[int, Any], bool]) -> bool:
        """ .all_idx(lambda idx, val: ...) """
        return not self.any_idx(lambda idx, val: not predicate(idx, val))

    def join(self, separator: str) -> str:
        return str(separator).join(str(val) for val in self._values)

    def map(self, func: Callable[[Any], Any]):
        return LinqObj(map(func, self._values))

    def map_many(self, func: Callable[[Any], Iterable[Any]]):
        res = []
        for val in self._values:
            for add in func(val):
                res.append(add)
        return LinqObj(res)

    def reduce(self, func: Callable[[Any, Any], Any]) -> Union[Any, None]:
        if not self._values:
            return None
        tmp = self._values[0]
        for i in range(1, len(self._values)):
            tmp = func(tmp, self._values[i])
        return tmp

    def to_list(self) -> List[Any]:
        return list(self._values)

    def to_dict(self, keys: Callable[[Any], Any], values: Callable[Any, Any]
                ) -> Dict[Any, Any]:
        return dict(map(lambda pair: (keys(pair), values(pair)), self._values))

    def to_set(self) -> Set[Any]:
        return set(self._values)

    def where(self, predicate: Callable[[Any], bool]):
        """ .where(lambda val: ...) """
        return LinqObj(filter(predicate, self._values))

    def where_not(self, predicate: Callable[[Any], bool]):
        """ .where_not(lambda val: ...) """
        return self.where(lambda val: not predicate(val))
    pass


def linq(values: Iterable[Any]):
    return LinqObj(values)


class SafeValue(dict):
    def __init__(self, value):
        super().__init__({})
        self._value = value

    def __getitem__(self, key):
        return SafeValue(None)

    def __call__(self, _type=None, otherwise=None):
        if _type is None:
            return self._value
        try:
            return _type(self._value)
        except Exception:
            return otherwise
    pass


class SafeDict(dict):
    """ Dict that won't throw KeyError when iterating.
    Usage: the_dict['key-1']..['key-n'](target_type, elsewise)
           the_dict['key-1']..['key-n'](target_type)
           the_dict['key-1']..['key-n']() """

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

    def __getitem__(self, key):
        # skip non-existent keys
        if not super().__contains__(key):
            return SafeDict()
        # convert values into safedicts with key ...
        value = super().__getitem__(key)
        if type(value) == dict:
            value = SafeDict(value)
        elif type(value) != SafeDict:
            value = SafeValue(value)
        return value

    def __call__(self, _type=None, otherwise=None):
        """ *() call the leaf to resolve value """
        this = dict(super().items())
        return this if _type is None or _type == dict else otherwise
    pass


def safedict(d: Dict) -> SafeDict:
    return SafeDict(d)
