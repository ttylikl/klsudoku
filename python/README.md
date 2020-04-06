# klsudoku

Sudoku generator and solver based on Knuth's DLX method, implement with C++ to extend Python.

Tested on Centos and MacOS.

Note:

- C++ compiler needed
- python 3.7.x recommended

Distribute:
python setup2.py sdist bdist_wheel
twine upload --repository-url https://upload.pypi.org/legacy/ dist/*

Install:
```
pip install klsudoku
```
Usage: [test.py]
```
import sys
import klsudoku
(puzzle, solution) = klsudoku.generate()
print("generate: puzzle=%s solution=%s" % (puzzle, solution))
(cnt, result) = klsudoku.solve(puzzle)
print('solve result: [%s] / %s' % (cnt, result))
cnt = klsudoku.solutioncount(puzzle)
print('solutioncount is %s' % cnt)

strallcands = ';2,7,8;3,7,8,9;1;1,2,3,7;1,2,3,7;;;1,2,7,8,9;3,4,5,7;1,2,4,5,7;1,2,3,4,5,7;;1,2,3,6,7;;1,2,3,6,7;2,3,6,7;1,2,6,7;3,4,5,7,9;1,2,4,5,7,8;1,2,3,4,5,7,8,9;1,3,4,5,6;1,2,3,6,7;1,2,3,4,5,6,7;1,2,3,6,7,8;2,3,6,7,8,9;1,2,6,7,8,9;;4,6;3,4,6,9;;;2,3,4,6,8;2,6,8;2,4,6,8;2,4,6,8;;4,5,6,7;4,5,6,7,9;1,4,6,9;1,2,6,9;1,2,4,6;1,2,5,6,7;2,4,6,7;;;4,5,6,7;3,4,5,6,7;1,3,4,6;1,3,6,8;1,3,4,6,8;;4,6,7,8;1,4,5,6,7,8;4,5,7;4,5,6,7,8;4,5,6,7,8;;3,6,7,8,9;3,5,6,7,8;3,5,6,7,8;;4,5,6,7,8,9;5,7;;1,2,5,6,7,8;1,3,5,6;;1,3,5,6,7,8;2,3,5,6,7,8;2,3,6,7,8;2,5,6,7,8;4,5,7;;1,2,4,5,6,7,8;1,5,6,9;1,6,7,8,9;1,5,6,7,8;2,5,6,7,8;2,4,6,7,8,9;2,4,5,6,7,8,9'
strpz = '600000450000809000000000000100750000800000003200000900000200010090040000030000000'
strcurpsz = strpz

acts = klsudoku.gethints(strpz, strcurpsz, 1, strallcands)
print(acts)

solver = 3
sname = klsudoku.solvername(solver)
print('solver:%s is %s' % (solver, sname))

```
