
import gc
import inspect
import sys

def _get_signature(func):
    sig = list(inspect.signature(func).parameters.items())
    return [(x[1].name, x[1].annotation) for x in sig]

def automatch(func):
    def decorated_func():
        last_frame = sys._getframe(1)
        prev_globals, prev_locals = last_frame.f_globals, last_frame.f_locals
        parent_func = gc.get_referrers(sys._getframe(1).f_code)[0]
        ch_sig = _get_signature(func)
        par_sig = _get_signature(parent_func)
        typ_to_val = dict((i[1], i[0]) for i in par_sig)
        kwargs = dict((v[0], prev_locals[typ_to_val[v[1]]]) for v in ch_sig)
        return func(**kwargs)
    return decorated_func

@automatch
def f(a: int, b: str) -> float:
    return b * a

@automatch
def f2(a: str, b: int) -> float:
    return f'[{a}]' * b

def g(x: str, y: int, z: float) -> bytes:
    r1 = f()
    print(f'result : {r1}')
    y = 3
    r2 = f2()
    print(f'result : {r2}')

g('test', 5, 8.7)
