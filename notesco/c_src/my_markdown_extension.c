
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "markdown_to_html.h"


static PyObject* py_process_markdown_file(PyObject* self, PyObject* args) {
    const char* input_filename;const char* output_filename;
    if (!PyArg_ParseTuple(args, "ss", &input_filename, &output_filename)) {
        return NULL;
    }
    int result = process_markdown_file(input_filename, output_filename);
    return PyLong_FromLong(result);
}
            

static PyMethodDef module_methods[] = {
    {"process_markdown_file", py_process_markdown_file, METH_VARARGS, "Wrapper for process_markdown_file"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "my_markdown_extension",
    "Python C Extension",
    -1,
    module_methods
};

PyMODINIT_FUNC PyInit_my_markdown_extension(void) {
    return PyModule_Create(&moduledef);
}
        