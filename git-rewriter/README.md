# Git History Rewriter

Takes the entire Git history from one repo, rewrite the entire history by modifying the committer name & email, author name & email, date, commit message and folder structure, and replicates *this very commit* in another repo. The entire process is automated.

The rewriter supports mutating (multiple) partial chains on the source tree and attaching the result to a certain node on the destination tree. This is useful when you want to rewrite the history of a certain branch and attach it to the tip of another branch.

## Usage

```bash
mkdir /path/to/dest
cd /path/to/dest
git init .

cd /path/to/git-history-rewriter
python3 ./grewrite.py plan.json
```

## Notes

* It supports rewriting multiple branches at once. The intersection of chains will be merged automatically and that these commits need no explicit declaration in the plan.
* To perform more advanced operations, you may `import git_rewrite from grewrite` and use the API directly. The mutation plan schema supports raw Python code as well. Sadly they cannot be defined in a JSON file.
* The script supports all platforms that runs Python and Git.
* Before running the script, both repo directories must be initialized and the working tree must be clean.
* [TODO] Merge commits are not supported yet.

## License

```
MIT License

Copyright (c) 2023 Geoffrey Tang

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
