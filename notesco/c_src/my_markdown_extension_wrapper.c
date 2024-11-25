#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "markdown_to_html.h"
#include "markdown_to_html.c"


static PyObject* parse_markdown_line_wrapper(PyObject* self, PyObject* args) {
    PyObject* input;    const char* line;    PyObject* output;
    
    if (!PyArg_ParseTuple(args, "s", &line)) {
        return NULL;
    }

    FILE* input_file = NULL;
    if (PyUnicode_Check(input)) {
        const char* filename = PyUnicode_AsUTF8(input);
        input_file = fopen(filename, "r");
        if (!input_file) {
            PyErr_SetString(PyExc_IOError, "Could not open file");
            return NULL;
        }
    } else {
        PyErr_SetString(PyExc_TypeError, "Expected string for filename");
        return NULL;
    }
    FILE* output_file = NULL;
    if (PyUnicode_Check(output)) {
        const char* filename = PyUnicode_AsUTF8(output);
        output_file = fopen(filename, "r");
        if (!output_file) {
            PyErr_SetString(PyExc_IOError, "Could not open file");
            return NULL;
        }
    } else {
        PyErr_SetString(PyExc_TypeError, "Expected string for filename");
        return NULL;
    }
    parse_markdown_line(input_file, line, output_file);

    if (input_file) { fclose(input_file); }
    if (output_file) { fclose(output_file); }
    Py_RETURN_NONE;
}

static PyMethodDef ModuleMethods[] = {
    {"parse_markdown_line", parse_markdown_line_wrapper, METH_VARARGS, "Python wrapper for parse_markdown_line"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "markdown_extension",
    NULL,
    -1,
    ModuleMethods
};

PyMODINIT_FUNC PyInit_markdown_extension(void) {
    return PyModule_Create(&moduledef);
}