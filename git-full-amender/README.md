# Git Full Amender

Takes the entire Git history from one repo, modifies the committer name & email, commit message, and replicates *this very commit* in another (initially empty) repo. The entire process is automated.

## Usage

```bash
python3 amender.py                             \
    --src /absolute/path/to/source/repo        \
    --from-name "Take This Person"             \
    --from-email "and@his.email"               \
    --dest /absolute/path/to/destination/repo  \
    --to-name "Into This Name"                 \
    --to-email "with@this.email"               ;
```

You may also provide an alternate `modifier` to the `Amender` class to give your own commit modification logic (the command line only provided a simple version for ease of use). For example:

```python
from amender import Amender

def myModifier(commit):
    name, email, commit_msg = commit
    if name == 'Kamisato Ayaka':
        return ('Raiden Shogun', 'raiden@inazuma.teyvat', commit_msg)
   	return commit

Amender(
    src='/absolute/path/to/source/repo',
    dest='/absolute/path/to/target/repo',
    modifier=myModifier,
).rebase()
```

## Notes

* You should provide an **empty** repository as the target. If it's not, previously existent files within will be removed as a result. (We only copy and paste the source to the target, and cleans anything in the target before doing so).
  * Using an empty repo just cloned from upstream is fine.
* This script **only works for Windows** as of now as it involves the modification of system time. We are open to pull requests on system time change for other platforms.
* If you are running on Windows, **Administrator Privileges** are mandatory for system time change.
* It is a **must** to have the both repos' **working tree clean**. Undefined behavior might be inflicted otherwise.
* This tool clones Git frames verbatim so there would be *equal* commits in the target as the source has.

## License

```
MIT License

Copyright (c) 2021 Geoffrey Tang

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

