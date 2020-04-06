#include "klsudoku.h"
#include "../nodejs/src/common.h"

static PyObject *
pySolve(PyObject *self, PyObject *args)
{
    const char *pszstr;

    if (!PyArg_ParseTuple(args, "s", &pszstr))
        return NULL;

    DLXSolver dlx;

    int cnt=0;
    string res = dlx.solution(pszstr, &cnt);

    // printf("try to solve:%s\ncnt:%d solution:%s\n", pszstr, cnt, res.c_str());
    
    return Py_BuildValue("(i,s)", cnt, res.c_str());
}

static PyObject *
pySolutionCount(PyObject *self, PyObject *args)
{
    const char *pszstr;

    if (!PyArg_ParseTuple(args, "s", &pszstr))
        return NULL;

    DLXSolver dlx;

    // int cnt=0;
    // string res = dlx.solution(pszstr, &cnt);
    int cnt = dlx.solution_count(pszstr);

    // printf("try to solve:%s\ncnt:%d solution:%s\n", pszstr, cnt, res.c_str());
    
    return Py_BuildValue("i", cnt);
}
static PyObject *
pyGenerate(PyObject *self, PyObject *args)
{
    DLXSolver dlx;
    string strq = dlx.generate();
    int cnt=0;
    string stra = dlx.solution(strq.c_str(), &cnt);
    return Py_BuildValue("(s,s)", strq.c_str(), stra.c_str());
}
static PyObject *
pyGetHints(PyObject *self, PyObject *args)
{
    const char * puzzle;
    const char * curpsz;
    const char * allcands;
    int curnum;

    if (!PyArg_ParseTuple(args, "ssis", &puzzle, &curpsz, &curnum, &allcands))
        return NULL;
    printf("arguments:\n%s\n%s\n%d\n%s\n", puzzle, curpsz, curnum, allcands);
    Solver solver;
    Puzzle pz;
    printf("prepare to solve!");
    pz.loads(puzzle, curpsz, allcands);
    printf("Puzzle::loads() done!");
    if(curnum<1) {
        curnum = 1;
    }
    solver.setCurrentNumber(curnum);
    vector<SAction> acts = solver.doSolve(&pz);
    PyObject *ret = PyList_New(acts.size());
    for(int i=0; i<acts.size();i++){
        SAction act = acts.at(i);
        string actstr = act.dumps();
        PyList_SetItem(ret, i, PyUnicode_FromString(actstr.c_str()));
    }
    return ret;
}

static PyObject *
pySoverName(PyObject *self, PyObject *args)
{
    int nSolver=256;
    if (!PyArg_ParseTuple(args, "i", &nSolver))
        return NULL;
    return Py_BuildValue("s", toSolverName((EnumSolver)nSolver));
}

static PyMethodDef test_methods[] = {
    {"solve", pySolve, METH_VARARGS, "Try to solve the sudoku."},
    {"solutioncount", pySolutionCount, METH_VARARGS, "Try to find the solution count of the sudoku."},
    {"generate", pyGenerate, METH_VARARGS, "To generate sudoku random."},
    {"gethints", pyGetHints, METH_VARARGS, "Try to find the next hints."},
    {"solvername", pySoverName, METH_VARARGS, "Get the solve hint name by hint id."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

static struct PyModuleDef spammodule = {
    PyModuleDef_HEAD_INIT,
    "klsudoku",   /* name of module */
    NULL, /* module documentation, may be NULL */
    -1,       /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
    test_methods
};

// PyMODINIT_FUNC initklsudoku() 
// { 
//  Py_InitModule("klsudoku", test_methods); 
// }

PyMODINIT_FUNC PyInit_klsudoku(void)
{
    PyObject *m;

    m = PyModule_Create(&spammodule);
    if (m == NULL)
        return NULL;
        
    return m;
}