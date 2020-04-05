#include "klsudoku.h"

static PyObject *
echoTest(PyObject *self, PyObject *args)
{
    const char *command;

    if (!PyArg_ParseTuple(args, "s", &command))
        return NULL;
    char *strbuf = (char *)malloc(strlen(command)+100);
    sprintf(strbuf, "Echo返回：%s", command);
    PyObject * ret =  PyUnicode_FromString(strbuf);
    free(strbuf);
    return ret;
}

static PyMethodDef test_methods[] = {
    {"echoTest", echoTest, METH_VARARGS, "Echo args back!"}, 
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