var dlx = require('./build/Release/dlx');
if (false) {
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

  // multi test:
  const pszmulti = '000000000000000000000000000000900000000000050008007010907000100000040000000350060';
  console.log(dlx.solve(pszmulti));
  console.log(dlx.solutioncount(pszmulti));

  const pszvalid = '000000000530000000000008702000900000000000050008007010907000100000040000000350060';
  console.log(dlx.solve(pszvalid));
  console.log(dlx.solutioncount(pszvalid));

  const psznotvalid = '110000000530000000000008702000900000000000050008007010907000100000040000000350060';
  console.log(dlx.solve(psznotvalid));
  console.log(dlx.solutioncount(psznotvalid));
}

if(true) { // hints test
  // get hints
  const pszvalid = '000000000530000000000008702000900000000000050008007010907000100000040000000350060';
  let ret = dlx.gethints(pszvalid, pszvalid, 1);
  console.log(ret);
  try {
    const acts = JSON.parse(ret.acts[0]);
    console.log(acts);
  } catch(e) {
    console.log(e);
  }

  const pzhints2 = '512374060600250407470000205067425000200607054054000672825100740340702580790548020';
  ret = dlx.gethints(pzhints2, pzhints2, 1);
  console.log(ret);
  try {
    const acts = JSON.parse(ret.acts[0]);
    console.log(acts);
  } catch(e) {
    console.log(e);
  }
}