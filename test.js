var dlx = require('./build/Release/dlx');
// console.log(dlx);
let puzzles = [
  '000000000530000000000008702000900000000000050008007010907000100000040000000350060',
  '000012034000056078000000000000000000410000085290000067000000000120039000980047000',
];
for(let i=0;i<puzzles.length;i++){
  console.log(dlx.solve(puzzles[i]));
}

for(let i=0;i<10;i++){
  const {puzzle, solution} = dlx.generate();
  console.log(`${puzzle} => ${solution}`);
}
