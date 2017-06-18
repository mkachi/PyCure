#include <PyCure.h>

using namespace PyCure;

#pragma region Function Sample
/*
static int Multiple(int a, int b)
{
	return a * b;
}

static PyObject* Native_Multiple(PyObject* self, PyObject* args)
{
	int a = GetArg<int>(args, 0);
	int b = GetArg<int>(args, 1);
	return PyType(a * b);
}

static int Add(int a, int b)
{
	int result = a + b;
	return result;
}

static PyObject* Native_Add(PyObject* self, PyObject* args)
{
	int a = GetArg<int>(args, 0);
	int b = GetArg<int>(args, 1);
	int result = a + b;
	return PyType(result);
}
*/
#pragma endregion

static PyObject* Native_Multiple(PyObject* self, PyObject* args)
{
    int a, b;
	a = GetArg<int>(args, 0);
	b = GetArg<int>(args, 1);
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

PyCure_CLIB_BEGIN(Extending)
PyCure_CFUNC("Multiple", Native_Multiple, NULL)
PyCure_CFUNC("Add", Native_Add, NULL)
PyCure_CLIB_END(Extending, NULL)