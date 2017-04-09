#include <PyCure.h>
// build setting
// Release and x64
using namespace PyCure;

static PyObject* Native_Multiple(PyObject* self, PyObject* args)
{
    int a, b;
    if (!PyArg_ParseTuple(args, "ii", &a, &b))
    {
        return NULL;
    }
    return PyType(a * b);
}

static PyObject* Native_Add(PyObject* self, PyObject* args)
{
    int a, b;
    if (!PyArg_ParseTuple(args, "ii", &a, &b))
    {
        return NULL;
    }
    return PyType(a + b);
}

PyCure_CLIB_BEGIN(myLib)
PyCure_CFUNC("Multiple", Native_Multiple, NULL)
PyCure_CFUNC("Add", Native_Add, NULL)
PyCure_CLIB_END(myLib, NULL)