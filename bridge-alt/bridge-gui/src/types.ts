
type integer = number;


/// Virtual function decorator.
function virtual() {
  return function (target: any, propertyKey: string, descriptor: PropertyDescriptor) {
    // throw new Error('warning: attempting to call virtual function');
  };
}


/// Interface under influence of Dart iterators.
class BIterable<_E> {

  /// Interface to get current iterator. Override this when implementing. When
  /// the object called on is an iterator itself, .iter() will always return
  /// the iterator itself.
  @virtual()
  iter(): BIterator<_E> {
    return new BIterator<_E>();
  }

  /// Checks whether any element of this iterable satisfies [test].
  any(test: (elem: _E) => boolean): boolean {
    for (let it = this.iter(), cur = it.cur(); cur !== undefined; cur = it.next())
      if (test(cur))
        return true;
    return false;
  }

  /// Converts all items to another type. Requires _E to be castable to _T.
  cast<_T>(): BIterator<_T> {
    return this.map((elem: _E) => (elem as any) as _T);
  }

  /// Whether the collection contains an element equal to [object].
  contains(object: _E): boolean {
    return this.any((elem) => elem === object);
  }

  /// Checks whether all elements of this iterable satisfies [test].
  every(test: (elem: _E) => boolean): boolean {
    for (let it = this.iter(), cur = it.cur(); cur !== undefined; cur = it.next())
      if (!test(cur))
        return false;
    return true;
  }

  /// Expands every element into an Iterable and concatenate all resulting
  /// iterables into one long lazy iterator.
  expand<_T>(fn: (elem: _E) => BIterable<_T>): BIterator<_T> {
    class ExpandIterator<_E, _T> extends BIterator<_T> {
      _iter_master: BIterator<_E>;
      _iter?: BIterator<_T>;  // second-level iterator
      _fn: (elem: _E) => BIterable<_T>;  // iterable generator
      constructor(iter: BIterator<_E>, fn: (elem: _E) => BIterable<_T>) {
        super();
        this._iter_master = iter;
        this._fn = fn;
        this._iter = undefined;
        // get current sl iterator
        let slc = this._iter_master.cur();
        if (slc !== undefined)
          this._iter = this._fn(slc).iter();
        // ensure that we're currently at a non-empty sl-iterator
        this.cur();
      }
      _next_iter(): BIterator<_T> | undefined {
        // gets next second-level iterator, undefined if no more.
        let c = this._iter_master.next();
        if (c === undefined)
          return undefined;
        return this._fn(c).iter();
      }
      next(): _T | undefined {
        if (this._iter === undefined)
          return undefined;
        this._iter.next();  // as cur is guaranteed to be non-null
        return this.cur();
      }
      cur(): _T | undefined {
        while (this._iter !== undefined && this._iter!.cur() === undefined)
          this._iter = this._next_iter();
        // no more sl-iterators
        if (this._iter === undefined)
          return undefined;
        // can get one more
        return this._iter!.cur();
      }
    }
    return new ExpandIterator<_E, _T>(this.iter(), fn);
  }

  /// Finds the first element satisfying given predicate [test]. If nothing
  /// were found, returns [elsewise] as a placeholder.
  first(test: (elem: _E) => boolean, elsewise?: _E): _E | undefined {
    for (let it = this.iter(), cur = it.cur(); cur !== undefined; cur = it.next())
      if (test(cur))
        return cur;
    return elsewise;
  }

  /// Reduces items to a single value by combining all items with a single
  /// existing [value] with [combine].
  fold<_T>(value: _T, combine: (value: _T, elem: _E) => _T): _T {
    for (let it = this.iter(), cur = it.cur(); cur !== undefined; cur = it.next())
      value = combine(value, cur);
    return value;
  }

  /// Returns the lazy concatenation with another iterable.
  followedBy(other: BIterable<_E>): BIterator<_E> {
    class AppendIterator<_E> extends BIterator<_E> {
      _iter1: BIterator<_E>;
      _iter2: BIterator<_E>;
      _state: integer;
      constructor(first: BIterator<_E>, second: BIterator<_E>) {
        super();
        this._iter1 = first;
        this._iter2 = second;
        this._state = 1;  // 1: first, 2: second, 3: none
      }
      next(): _E | undefined {
        switch (this._state) {
          case 1: this._iter1.next(); break;
          case 2: this._iter2.next(); break;
          default: break;
        }
        return this.cur();
      }
      cur(): _E | undefined {
        if (this._state === 1) {
          let c = this._iter1.cur();
          if (c !== undefined)
            return c;
          this._state = 2;
        }
        if (this._state === 2) {
          let c = this._iter2.cur();
          if (c !== undefined)
            return c;
          this._state = 3;
        }
        return undefined;
      }
    }
    return new AppendIterator<_E>(this.iter(), other.iter());
  }

  /// Finds the last element satisfying given predicate [test]. If none were
  /// found, returns [elsewise]. Note that this could damage performance.
  last(test: (elem: _E) => boolean, elsewise?: _E): _E | undefined {
    let last = elsewise;
    for (let it = this.iter(), cur = it.cur(); cur !== undefined; cur = it.next())
      if (test(cur))
        last = cur;
    return last;
  }

  /// Returns another lazy iterable by calling [fn] on items in order.
  map<_T>(fn: (elem: _E) => _T): BIterator<_T> {
    class MapIterator<_E, _T> extends BIterator<_T> {
      _iter: BIterator<_E>;
      _fn: (elem: _E) => _T;
      constructor(iter: BIterator<_E>, fn: (elem: _E) => _T) {
        super();
        this._iter = iter;
        this._fn = fn;
      }
      next(): _T | undefined {
        this._iter.next();
        return this.cur();
      }
      cur(): _T | undefined {
        let c = this._iter.cur();
        if (c === undefined)
          return undefined;
        return this._fn(c);
      }
    }
    return new MapIterator<_E, _T>(this.iter(), fn);
  }

  /// Reduces all items to one single item by iterating all in consecutive
  /// order. Note that this function requires the iterable to be non-empty.
  /// Behaviour undefined when [fn] is not a closed arithmetic on [_E].
  reduce(fn: (a: _E, b: _E) => _E): _E | undefined {
    let it = this.iter();
    let value = it.cur();
    if (value === undefined)
      return undefined;
    for (let cur = it.cur(); cur !== undefined; cur = it.next())
      value = fn(value, cur);
    return value;
  }

  /// Returns an iterator skipping first [count] items.
  skipFirst(count: integer): BIterator<_E> {
    class SkipFirstIterator<_E> extends BIterator<_E> {
      _iter: BIterator<_E>;
      constructor(iter: BIterator<_E>, count: integer) {
        super();
        this._iter = iter;
        for (let i: integer = 0; i < count; i++)
          this._iter.next();
      }
      next(): _E | undefined {
        return this._iter.next();
      }
      cur(): _E | undefined {
        return this._iter.cur();
      }
    }
    return new SkipFirstIterator<_E>(this.iter(), count);
  }

  /// Returns an iterator skipping first items satisfying predicate [test].
  skipWhile(test: (elem: _E) => boolean): BIterator<_E> {
    class SkipWhileIterator<_E> extends BIterator<_E> {
      _iter: BIterator<_E>;
      constructor(iter: BIterator<_E>, test: (elem: _E) => boolean) {
        super();
        this._iter = iter;
        for (let cur = this._iter.cur(); cur !== undefined && test(cur); )
          cur = this._iter.next();
      }
      next(): _E | undefined {
        return this._iter.next();
      }
      cur(): _E | undefined {
        return this._iter.cur();
      }
    }
    return new SkipWhileIterator<_E>(this.iter(), test);
  }

  /// Returns an iterator keeping first [count] items.
  takeFirst(count: integer): BIterator<_E> {
    class TakeFirstIterator<_E> extends BIterator<_E> {
      _iter: BIterator<_E>;
      _count: integer;
      constructor(iter: BIterator<_E>, count: integer) {
        super();
        this._iter = iter;
        this._count = count;
      }
      next(): _E | undefined {
        this._count--;
        this._iter.next();
        return this.cur();
      }
      cur(): _E | undefined {
        if (this._count < 1)
          return undefined;
        return this._iter.cur();
      }
    }
    return new TakeFirstIterator<_E>(this.iter(), count);
  }

  /// Returns an iterator keeping first items satisfying predicate [test].
  takeWhile(test: (elem: _E) => boolean): BIterator<_E> {
    class SkipWhileIterator<_E> extends BIterator<_E> {
      _iter: BIterator<_E>;
      _test: (elem: _E) => boolean;
      _keep: boolean;
      constructor(iter: BIterator<_E>, test: (elem: _E) => boolean) {
        super();
        this._iter = iter;
        this._test = test;
        this._keep = true;
      }
      next(): _E | undefined {
        if (!this._keep)
          return undefined;
        this._iter.next();
        return this.cur();
      }
      cur(): _E | undefined {
        if (!this._keep)
          return undefined;
        let c = this.cur();
        if (c === undefined || !this._test(c)) {
          this._keep = false;
          return undefined;
        }
        return c;
      }
    }
    return new SkipWhileIterator<_E>(this.iter(), test);
  }

  /// Convert iterated items to List type.
  toList(): List<_E> {
    return new List<_E>(this.toArray());
  }

  /// Convert iterated items to JavaScript Array type.
  toArray(): _E[] {
    let arr = Array<_E>();
    let iter = this.iter();
    for (let c = iter.cur(); c !== undefined; c = iter.next())
      arr.push(c);
    return arr;
  }

  /// Returns an iterator with all elements satisfying predicate [test].
  where(test: (elem: _E) => boolean): BIterator<_E> {
    class WhereIterator<_E> extends BIterator<_E> {
      _iter: BIterator<_E>;
      _test: (elem: _E) => boolean;
      constructor(iter: BIterator<_E>, test: (elem: _E) => boolean) {
        super();
        this._iter = iter;
        this._test = test;
      }
      _find_next_valid(): _E | undefined {
        // shift pointer to next valid element (won't shift if current element
        // is valid. Returns undefined if no such exists.
        let c = this._iter.cur();
        for (; c !== undefined && !this._test(c); c = this._iter.next());
        return c;
      }
      next(): _E | undefined {
        this._iter.next();
        return this._find_next_valid();
      }
      cur(): _E | undefined {
        return this._find_next_valid();
      }
    }
    return new WhereIterator<_E>(this.iter(), test);
  }

  /// Returns an iterator with all elements not satisfying predicate [test].
  whereNot(test: (elem: _E) => boolean): BIterator<_E> {
    return this.where((elem: _E) => !test(elem));
  }
}

class BIterator<_E> extends BIterable<_E> {
  /// Moves iterator to the next element. Does not return reference to it. Note
  /// that shifting over undefined should be ignored and NOT throw errors.
  @virtual()
  next(): _E | undefined {
    return undefined;
  }
  /// Returns current value to which the iterator points. Returns undefined if
  /// the iteration had already ended.
  @virtual()
  cur(): _E | undefined {
    return undefined;
  }
  /// An iterator's iterator is always itself.
  iter(): BIterator<_E> {
    return this;
  }
}

class List<_E> extends BIterable<_E>{
  _data: _E[];

  constructor(other?: List<_E> | _E[]) {
    super();
    if (other === undefined) {
      this._data = [];
    } else if ('_data' in other) {
      this._data = [];
      for (let i = 0; i < other._data.length; i++)
        this._data.push(other._data[i]);
    } else {
      this._data = [];
      for (let i = 0; i < other.length; i++)
        this._data.push(other[i]);
    }
  }

  /// Appends [elem] to the end of this list.
  append(elem: _E) {
    this._data.push(elem);
  }

  /// Appends all [elems]s to the end of this list consecutively.
  appendAll(...elems: _E[]) {
    this._data.push(...elems);
  }

  /// Returns the index-th item. If it exceeds the array size, will return
  /// undefined instead.
  at(index: integer): _E | undefined {
    if (index >= this._data.length)
      return undefined;
    return this._data[index];
  }

  /// Empties the list.
  clear(): void {
    this._data = [];
  }

  /// Fills the elements in range [start, end) with [value].
  fill(start: integer, end: integer, value: _E): void {
    for (let i = Math.max(start, 0); i < end && i < this._data.length; i++)
      this._data[i] = value;
  }

  /// Fills all elements with [value].
  fillAll(value: _E): void {
    this.fill(0, this._data.length, value);
  }

  /// Returns the index of the first element that satisfies predicate [test].
  /// Will return undefined if element not found.
  firstIndex(test: (elem: _E) => boolean): integer | undefined {
    for (let i = 0; i < this._data.length; i++)
      if (test(this._data[i]))
        return i;
    return undefined;
  }

  /// Returns if the list is empty.
  get isEmpty(): boolean {
    return this._data.length === 0;
  }

  /// Returns if the list is not empty.
  get isNotEmpty(): boolean {
    return this._data.length > 0;
  }

  /// Gets iterator for current list. During the lifetime of this iterator the
  /// list contents may not be changed.
  iter(): BIterator<_E> {
    return this.range(0, this._data.length);
  }

  /// Returns the index of the first element that satisfies predicate [test].
  /// Will return undefined if element not found.
  lastIndex(test: (elem: _E) => boolean): integer | undefined {
    for (let i = this._data.length - 1; i >= 0; i--)
      if (test(this._data[i]))
        return i;
    return undefined;
  }

  /// Retrieves the list length.
  get length(): integer {
    return this._data.length;
  }

  /// Get a lazy iterator that iterates over a range of elements. Note that
  /// the lifetime of this iterator must be handled.
  range(start: integer, end: integer): BIterator<_E> {
    class ListRangeIterator<_E> extends BIterator<_E> {
      _delegate: _E[];
      _cur: integer;
      _end: integer;
      constructor(delegate: _E[], start: integer, end: integer) {
        super();
        this._delegate = delegate;
        this._cur = start;
        this._end = end;
      }
      next(): _E | undefined {
        if (this._cur >= this._end)
          return undefined;
        this._cur++;
        return this.cur();
      }
      cur(): _E | undefined {
        if (this._cur >= this._end || this._cur >= this._delegate.length)
          return undefined;
        return this._delegate[this._cur];
      }
    }
    return new ListRangeIterator<_E>(this._data, start, end);
  }

  /// Removes the first occurence of [elem] in this list.
  remove(elem: _E): void {
    let i = this.firstIndex((x) => x === elem);
    if (i === undefined)
      return ;
    this._data.splice(i!, 1);
  }

  /// Removes the item at position [index].
  removeAt(index: integer): void {
    if (index < 0 || index >= this._data.length)
      return ;
    this._data.splice(index, 1);
  }

  /// Removes a sequence of items in range [start, end).
  removeRange(start: integer, end: integer): void {
    start = Math.max(0, start);
    end = Math.min(this._data.length, end);
    this._data.splice(start, end - start);
  }

  /// Re-order all elements in list randomly.
  shuffle(): void {
    for (let i = this._data.length - 1; i >= 0; i--) {
      let r = Math.random() * (i + 1);
      r = Math.round(r) % (i + 1);
      if (r !== i) {
        let tmp = this._data[i];
        this._data[i] = this._data[r]
        this._data[r] = tmp;
      }
    }
  }

  /// Sort all elements in list according to [compare] function. [compare] must
  /// return 0 for equal, < 0 for a < b and > 0 for a > b.
  sort(compare: (a: _E, b: _E) => integer): void {
    this._data.sort(compare);
  }
}


export type { integer, BIterable, BIterator, List };
