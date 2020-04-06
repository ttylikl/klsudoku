import sys
sys.path.append('./build/lib.macosx-10.9-x86_64-3.7')
sys.path.append('./build/lib.win-amd64-3.7')
import klsudoku
dir(klsudoku)
(puzzle, solution) = klsudoku.generate()
print("generate: puzzle=%s solution=%s" % (puzzle, solution))
#puzzle = '..2943.1.94...1..7.....8..659.67..3................6.94.............6......812.7.'
#solution = '672943815948561327135728496591674238864239751723185649489357162217496583356812974'
(cnt, result) = klsudoku.solve(puzzle)
print('solve result: [%s] / %s' % (cnt, result))
cnt = klsudoku.solutioncount(puzzle)
print('solutioncount is %s' % cnt)

strallcands = ';2,7,8;3,7,8,9;1;1,2,3,7;1,2,3,7;;;1,2,7,8,9;3,4,5,7;1,2,4,5,7;1,2,3,4,5,7;;1,2,3,6,7;;1,2,3,6,7;2,3,6,7;1,2,6,7;3,4,5,7,9;1,2,4,5,7,8;1,2,3,4,5,7,8,9;1,3,4,5,6;1,2,3,6,7;1,2,3,4,5,6,7;1,2,3,6,7,8;2,3,6,7,8,9;1,2,6,7,8,9;;4,6;3,4,6,9;;;2,3,4,6,8;2,6,8;2,4,6,8;2,4,6,8;;4,5,6,7;4,5,6,7,9;1,4,6,9;1,2,6,9;1,2,4,6;1,2,5,6,7;2,4,6,7;;;4,5,6,7;3,4,5,6,7;1,3,4,6;1,3,6,8;1,3,4,6,8;;4,6,7,8;1,4,5,6,7,8;4,5,7;4,5,6,7,8;4,5,6,7,8;;3,6,7,8,9;3,5,6,7,8;3,5,6,7,8;;4,5,6,7,8,9;5,7;;1,2,5,6,7,8;1,3,5,6;;1,3,5,6,7,8;2,3,5,6,7,8;2,3,6,7,8;2,5,6,7,8;4,5,7;;1,2,4,5,6,7,8;1,5,6,9;1,6,7,8,9;1,5,6,7,8;2,5,6,7,8;2,4,6,7,8,9;2,4,5,6,7,8,9'
strpz = '600000450000809000000000000100750000800000003200000900000200010090040000030000000'
strcurpsz = strpz

acts = klsudoku.gethints(strpz, strcurpsz, 1, strallcands)
print(acts)
#['{"solver":3,"solvername":"HiddenSingleNumber","ops":[{"act":0,"st":0,"oldst":0,"color":1,"nums":[9],"pts":["3,1"],"rcands":[]}],"nums":[],"pts":[],"cellsets":[{"name":"<COL1>", "type":0, "id":0}],"chains":[]}']

solver = 3
sname = klsudoku.solvername(solver)
print('solver:%s is %s' % (solver, sname))
