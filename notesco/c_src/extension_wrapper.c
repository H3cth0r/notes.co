
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "markdown_to_html.h"
#include "markdown_to_html.c"


static PyObject* print_buffer_size_wrapper(PyObject* self, PyObject* args) {
    size_t buffer_size;
    
    if (!PyArg_ParseTuple(args, "n", 
                         &buffer_size)) {
        return NULL;
    }
    
        print_buffer_size((size_t)buffer_size);
    Py_RETURN_NONE;
    
    return Py_RETURN_NONE;
}
        

static PyObject* is_code_block_delimeter_wrapper(PyObject* self, PyObject* args) {
    const char* line;
    
    if (!PyArg_ParseTuple(args, "s", 
                         &line)) {
        return NULL;
    }
    
        int result = is_code_block_delimeter(line);
    
    return PyLong_FromLong(result);
}
        

static PyObject* get_language_spec_wrapper(PyObject* self, PyObject* args) {
    const char* line;
    
    if (!PyArg_ParseTuple(args, "s", 
                         &line)) {
        return NULL;
    }
    
        char* result = get_language_spec(line);
    
    return PyUnicode_FromString(result);
}
        

static PyObject* process_code_block_wrapper(PyObject* self, PyObject* args) {
    FILE* input;
    FILE* output;
    const char* language;
    
    if (!PyArg_ParseTuple(args, "OOs", 
                         &input, &output, &language)) {
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
    process_code_block(input_file, output_file, language);
    Py_RETURN_NONE;

                if (input_file) {
                    fclose(input_file);
                }

                if (output_file) {
                    fclose(output_file);
                }
    
    return Py_RETURN_NONE;
}
        

static PyObject* base64_encode_wrapper(PyObject* self, PyObject* args) {
    PyObject* data_obj;
    size_t input_length;
    PyObject* output_length_obj;
    
    if (!PyArg_ParseTuple(args, "OnO", 
                         &data, &input_length, &output_length)) {
        return NULL;
    }
    
        char* result = base64_encode(data, (size_t)input_length, output_length);
    
    return PyUnicode_FromString(result);
}
        

static PyObject* file_to_base64_wrapper(PyObject* self, PyObject* args) {
    const char* filename;
    PyObject* base64_length_obj;
    
    if (!PyArg_ParseTuple(args, "sO", 
                         &filename, &base64_length)) {
        return NULL;
    }
    
        char* result = file_to_base64(filename, base64_length);
    
    return PyUnicode_FromString(result);
}
        

static PyObject* process_image_block_wrapper(PyObject* self, PyObject* args) {
    const char* source;
    
    if (!PyArg_ParseTuple(args, "s", 
                         &source)) {
        return NULL;
    }
    
        char* result = process_image_block(source);
    
    return PyUnicode_FromString(result);
}
        

static PyObject* process_inline_formatting_wrapper(PyObject* self, PyObject* args) {
    const char* source;
    
    if (!PyArg_ParseTuple(args, "s", 
                         &source)) {
        return NULL;
    }
    
        char* result = process_inline_formatting(source);
    
    return PyUnicode_FromString(result);
}
        

static PyObject* parse_markdown_line_wrapper(PyObject* self, PyObject* args) {
    FILE* input;
    const char* line;
    FILE* output;
    
    if (!PyArg_ParseTuple(args, "OsO", 
                         &input, &line, &output)) {
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
    Py_RETURN_NONE;

                if (input_file) {
                    fclose(input_file);
                }

                if (output_file) {
                    fclose(output_file);
                }
    
    return Py_RETURN_NONE;
}
        

static PyMethodDef ModuleMethods[] = {
    {"print_buffer_size", print_buffer_size_wrapper, METH_VARARGS, 
                    "Python wrapper for print_buffer_size"},
    {"is_code_block_delimeter", is_code_block_delimeter_wrapper, METH_VARARGS, 
                    "Python wrapper for is_code_block_delimeter"},
    {"get_language_spec", get_language_spec_wrapper, METH_VARARGS, 
                    "Python wrapper for get_language_spec"},
    {"process_code_block", process_code_block_wrapper, METH_VARARGS, 
                    "Python wrapper for process_code_block"},
    {"base64_encode", base64_encode_wrapper, METH_VARARGS, 
                    "Python wrapper for base64_encode"},
    {"file_to_base64", file_to_base64_wrapper, METH_VARARGS, 
                    "Python wrapper for file_to_base64"},
    {"process_image_block", process_image_block_wrapper, METH_VARARGS, 
                    "Python wrapper for process_image_block"},
    {"process_inline_formatting", process_inline_formatting_wrapper, METH_VARARGS, 
                    "Python wrapper for process_inline_formatting"},
    {"parse_markdown_line", parse_markdown_line_wrapper, METH_VARARGS, 
                    "Python wrapper for parse_markdown_line"},
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
        