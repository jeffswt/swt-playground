# Lazy Python Caller

Some people are too lazy to fill in positional and keyword arguments into the methods that are to be called. So we came up with an idea to lazily fill in arguments **for** them.

Before using `@automatch` decorator, we'll always have to fill in the arguments `a` and `b`:

```python
def f(a: int, b: str):
    return b * a

def g(x: str, y: int, z: float) -> None:
    f(y, x)
```

However, if we use `@automatch` on the to-be-called method `f()`, it'll automatically match the local variables to their signatures (`x` matches `b` for being both `str`, and `y` matches `a` for being both `int`):

```python
@automatch
def f(a: int, b: str):
    return b * a

def g(x: str, y: int, z: float) -> None:
    f()  # implicitly, f(y: int, x: str)
```

A few things to be noted:

* This might not work on a later release after Python 3.9.5. It might not work on an earlier version either.
* All parameters should have distinct types on both `f` and `g`, i.e. caller and callee methods.
* It is not guaranteed to always work if you mess up with the call stack.

