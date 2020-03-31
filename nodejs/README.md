# klsudoku

Sudoku generator and solver based on Knuth's DLX method, implement with C++， N-API.

Tested on Centos and MacOS.

Note:

- N-API is available in Node.js 8.0 as an experimental feature. N-API: https://nodejs.org/docs/latest-v8.x/api/n-api.html
- C++ compiler needed
- python 2.7.x recommended

Install:
```
npm install klsudoku
```
Usage:
```
const klsudoku = require('klsudoku');
let result = klsudoku.generate();
let {puzzle, solution} = result;
result = klsudoku.solve(puzzle);
console.log(`puzzle:${puzzle}\nsolution:${solution}\n`);
console.log('solve() return:'+result.solution);
```
