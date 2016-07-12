#pragma once

#include <Python.h>
#include <list>

#define PyCureArgs PyTuple_Pack

namespace PyCure
{
	namespace internal
	{
		static std::list<PyObject*> PyCureModules;

		class _PyTuple
		{
		private:
			PyObject*	_core;
			int			_count;
			int			_counter;

		public:
			_PyTuple(PyObject* object, int count)
				: _core(object), _count(count), _counter(0)
			{	}

			~_PyTuple()
			{
				if (_core != NULL)
				{
					Py_DECREF(_core);
				}
			}

			PyObject* GetData() { return _core; }
			void Pull()
			{
				++_counter;
				if (_count <= _counter)
				{
					delete this;
				}
			}
		};
	}

	using PyTuple = internal::_PyTuple*;
	using PyModule = PyObject*;

	template<typename T>
	inline T PyType(PyObject* value) noexcept
	{	}

#define PyCure_PyType_PyToCpp(_TYPE_, _ARG_) \
	template<> \
	inline _TYPE_ PyType<_TYPE_>(PyObject* value) noexcept \
	{ \
		_TYPE_ result; \
		PyArg_Parse(value, _ARG_, &result); \
		Py_DECREF(value); \
		return result; \
	}

	template<typename T>
	inline PyObject* PyType(T value) noexcept
	{	}

#define PyCure_PyType_CppToPy(_TYPE_, _ARG_) \
	template<> \
	inline PyObject* PyType(_TYPE_ value) noexcept \
	{ \
		return Py_BuildValue(_ARG_, value); \
	}

	template<typename T>
	inline T GetTupleItem(PyTuple tuple, unsigned int index) noexcept
	{	}

#define PyCure_GetTupleItem(_TYPE_, _ARG_) \
	template<> \
	inline _TYPE_ GetTupleItem<_TYPE_>(PyTuple tuple, unsigned int index) noexcept \
	{ \
		_TYPE_ result; \
		PyArg_Parse(PyTuple_GetItem(tuple->GetData(), index), _ARG_, &result); \
		tuple->Pull(); \
		return result; \
	}

	PyCure_PyType_PyToCpp(char*, "s");
	PyCure_PyType_PyToCpp(const char*, "s");
	PyCure_PyType_PyToCpp(int, "i");
	PyCure_PyType_PyToCpp(char, "c");
	PyCure_PyType_PyToCpp(double, "d");
	PyCure_PyType_PyToCpp(float, "f");

	PyCure_PyType_CppToPy(char*, "s");
	PyCure_PyType_CppToPy(const char*, "s");
	PyCure_PyType_CppToPy(int, "i");
	PyCure_PyType_CppToPy(char, "c");
	PyCure_PyType_CppToPy(double, "d");
	PyCure_PyType_CppToPy(float, "f");

	PyCure_GetTupleItem(char*, "s");
	PyCure_GetTupleItem(const char*, "s");
	PyCure_GetTupleItem(int, "i");
	PyCure_GetTupleItem(char, "c");
	PyCure_GetTupleItem(double, "d");
	PyCure_GetTupleItem(float, "f");

	template<typename T>
	inline T GetVar(PyModule module, const char* name) noexcept
	{
		PyObject* var = PyObject_GetAttrString(module, name);
		return PyType<T>(var);
	}

	template<typename T>
	inline T CallFunc(PyModule module, const char* name, PyObject* args = nullptr) noexcept
	{
		T result;
		PyObject* function = PyObject_GetAttrString(module, name);
		PyObject* pyResult = PyObject_CallObject(function, args);
		if (pyResult != Py_None)
		{
			result = PyType<T>(pyResult);
		}
		if (args != nullptr)
		{
			Py_DECREF(args);
		}
		Py_DECREF(pyResult);
		Py_DECREF(function);
		return result;
	}

	template<>
	inline void CallFunc<void>(PyModule module, const char* name, PyObject* args) noexcept
	{
		PyObject* function = PyObject_GetAttrString(module, name);
		PyObject* pyResult = PyObject_CallObject(function, args);
		if (args != nullptr)
		{
			Py_DECREF(args);
		}
		Py_DECREF(pyResult);
		Py_DECREF(function);
	}

	inline PyTuple TupleCallFunc(PyModule module, const char* name, unsigned int dataNum, PyObject* args = nullptr) noexcept
	{
		PyObject* function = PyObject_GetAttrString(module, name);
		PyObject* pyResult = PyObject_CallObject(function, args);
		if (args != nullptr)
		{
			Py_DECREF(args);
		}
		Py_DECREF(function);
		return new internal::_PyTuple(pyResult, dataNum);
	}

	inline void PyCure_Initialize()
	{
		Py_Initialize();
	}

	inline void PyCure_Finalize()
	{
		for (auto iter = internal::PyCureModules.begin(); iter != internal::PyCureModules.end(); ++iter)
		{
			if ((*iter) != NULL)
			{
				Py_DECREF((*iter));
			}
		}
		internal::PyCureModules.clear();
		Py_Finalize();
	}

	inline PyModule PyCure_ImportModule(const char* name)
	{
		PyModule result = PyImport_ImportModule(name);
		internal::PyCureModules.push_back(result);
		return result;
	}
}