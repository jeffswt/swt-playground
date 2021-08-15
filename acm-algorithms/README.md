# Common ACM-ICPC Algorithms

* You may produce a PDF output with `docgen/gen.py`.
  * Pandoc is used for converting Markdown to PDF.
* For starting a new problem project, `template.cpp` may be used for an omnipotent template.
* A deprecated template that has not been organized exists at `algorithms/assets/deprecated.cpp`.

---

## Template format

* Each algorithm has its unique string ID `${id}`.
* For each algorithm there will be two files, an algorithm description `algorithms/${id}.md` and a classy source code `algorithms/${id}.cpp`.
* The source code may only contain the program logic itself. For common constants like `maxn` or `maxm`, they could be defined outside the class.
* All Markdown descriptions must contain the title, related problems (e.g. hdu1001, referring to a solution under `problems/`) and all dependencies as a YAML front matter.
* All titles within the algorithm description will be elevated by exactly 1 level after combination into the final PDF. That is, all titles would become (2-level) subtitles in the final PDF. In addition to this rule, there are a few titles that you must implement in all descriptions.
  * `# Target`, the purpose of the algorithm.
  * `# Complexity`, an unordered list of unordered lists specifying the (Worst case, Amortized and Best case) (Space or Time) complexity of this algorithm.
  * `# Solution`, a description of how the algorithm works.
  * `# Invocation`, on how to invoke the `.cpp` class to solve problems. Under cases where this class may be slightly modified for other uses, specify them here as well.
* Titles may be subject to changes according to rules defined by `docgen/gen.py`.

