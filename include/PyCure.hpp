#pragma once

#include <Python.h>
#include <cassert>
#include <list>

namespace PyCure
{
	typedef PyObject*	PyVar;
	typedef PyObject*	PyArgs;

	template <typename T>
	inline T pyType(PyVar value) noexcept { }
	template <typename T>
	inline PyVar pyType(T value) noexcept {	}

#define PyCureArgs PyTuple_Pack

#define PYCURE_PYTYPE_PY2CPP(_TYPE_, _SYMBOL_) \
	template<> \
	inline _TYPE_ pyType<_TYPE_>(PyVar value) noexcept \
	{ \
		_TYPE_ result; \
		assert(value != NULL && "Unassigned variable!"); \
		PyArg_Parse(value, _SYMBOL_, &result); \
		Py_DECREF(value); \
		return result; \
	}

#define PYCURE_PYTYPE_CPP2PY(_TYPE_, _SYMBOL_) \
	template<> \
	inline PyVar pyType(_TYPE_ value) noexcept \
	{ \
		PyVar result = Py_BuildValue(_SYMBOL_, value); \
		assert(result!=NULL && "Value Build Failed!"); \
		return result; \
	}

#define PYCURE_GET_PYOBJECT_ITEM(_TYPE_, _SYMBOL_) \
	template<> \
	inline _TYPE_ internal::_getPyObjectItem<_TYPE_>(PyObject* tuple, unsigned int index) noexcept \
	{ \
		_TYPE_ result; \
		PyArg_Parse(PyTuple_GetItem(tuple, index), _SYMBOL_, &result); \
		return result; \
	}

#define PY_FUNCTION(_NAME_) PyObject* _NAME_(PyObject* self, PyObject* _param)
#define PY_PARAMETER(_ARGS_COUNT_) PyTuple args(_param, _ARGS_COUNT_)
#define PY_RETURN(_VALUE_) return pyType(_VALUE_)
#define PY_MODULE(_NAME_) static PyMethodDef _NAME_##_methods_[] =
#define PY_MODULE_FUNC(_PY_FUNC_NAME_, _FUNCTION_) { _PY_FUNC_NAME_, (PyCFunction)_FUNCTION_, METH_VARARGS, NULL }
#define PY_MODULE_FUNC_END { NULL, NULL, 0, NULL }
#define PY_EXPORT_MODULE(_NAME_) \
static PyModuleDef _NAME_##_module_ = { PyModuleDef_HEAD_INIT, #_NAME_, NULL, -1, _NAME_##_methods_ }; \
PyMODINIT_FUNC PyInit_##_NAME_() { return PyModule_Create(&_NAME_##_module_); }

	namespace internal
	{
		template <typename T>
		inline T _getPyObjectItem(PyObject* _PyTuple, unsigned int index) noexcept { }

		class _PyClass;
		class _PyTuple;
		class _PyModule
		{
		protected:
			PyObject*	_module;
			const char* _moduleName;

			_PyModule() : _module(nullptr), _moduleName("") { }

		public:
			_PyModule(const char* name)
				: _module(PyImport_ImportModule(name))
				, _moduleName(name)
			{
				assert(!(_module == NULL) && "Module import failed!");
			}
			virtual ~_PyModule()
			{
				if (_module != nullptr)
				{
					Py_DECREF(_module);
				}
			}

			PyObject* getModule() { return _module; }

			template <typename T>
			void setVar(const char* name, T value) noexcept
			{
				PyVar var = pyType(value);
				PyObject_SetAttrString(_module, name, var);
			}
			template <typename T>
			inline T getVar(const char* name) noexcept
			{
				PyVar value = PyObject_GetAttrString(_module, name);
				assert(value != NULL && "Variable import failed");
				return pyType<T>(value);
			}

			template <typename T>
			T callFunc(const char* name, PyArgs args = nullptr)
			{
				T result;
				PyObject* function = PyObject_GetAttrString(_module, name);
				assert(function != NULL && "Function import failed");

				PyVar value = PyObject_CallObject(function, args);

				assert((value != Py_None || value != NULL) && "This function has no return value.");
				result = pyType<T>(value);

				if (args != nullptr)
				{
					Py_DECREF(args);
				}
				Py_DECREF(value);
				Py_DECREF(function);

				return result;
			}
			template <>
			void callFunc<void>(const char* name, PyArgs args)
			{
				PyObject* function = PyObject_GetAttrString(_module, name);
				assert(function != NULL && "Function import failed");
				PyVar result = PyObject_CallObject(function, args);
				if (args != nullptr)
				{
					Py_DECREF(args);
				}
				if (result != NULL)
				{
					Py_DECREF(result);
				}
				Py_DECREF(function);
			}

			_PyTuple callFunc(const char* name, unsigned int tupleSize, PyArgs args = nullptr) noexcept;

			_PyClass* createPyClass(const char* name);

		};
		class _PyTuple
			: protected _PyModule
		{
		private:
			int _count;

		public:
			_PyTuple(PyObject* pyModule, int count)
				: _count(count)
			{
				assert(pyModule != nullptr && "Unassigned module");
				_module = pyModule;
			}
			virtual ~_PyTuple() { }

			PyObject* getModule() { return _module; }

			template <typename T>
			T get(int index)
			{
				assert(index < _count && "Out of range");
				return _getPyObjectItem<T>(_module, index);
			}

		};
		class _PyClass
			: protected _PyModule
		{
		private:
			const char* _className;

		public:
			_PyClass() = delete;
			_PyClass(PyObject* pyModule, const char* className)
				: _className(className)
			{
				_module = PyObject_CallMethod(pyModule, className, NULL);
				assert(_module != NULL && "Class import failed!");
			}
			virtual ~_PyClass() { }

			PyObject* getModule() { return _module; }
			const char* getClassName() { return _className; }

			template <typename T>
			inline void setField(const char* name, T value)
			{
				setVar<T>(name, value);
			}
			template <typename T>
			inline T getField(const char* name)
			{
				return getVar<T>(name);
			}

			template <typename T>
			inline T callMethod(const char* name, PyArgs args = nullptr) noexcept
			{
				return callFunc<T>(name, args);
			}

			_PyTuple callMethod(const char* name, unsigned int tupleSize, PyArgs args = nullptr) noexcept
			{
				return callFunc(name, tupleSize, args);
			}

		};

		_PyTuple _PyModule::callFunc(const char* name, unsigned int tupleSize, PyArgs args) noexcept
		{
			assert(tupleSize >= 0 && "tupleSize is to small");
			PyObject* function = PyObject_GetAttrString(_module, name);
			assert(function != NULL && "Function import failed");
			PyVar result = PyObject_CallObject(function, args);
			if (args != nullptr)
			{
				Py_DECREF(args);
			}
			Py_DECREF(function);
			return _PyTuple(result, tupleSize);
		}
		_PyClass* _PyModule::createPyClass(const char* name)
		{
			_PyClass* result = new _PyClass(_module, name);
			assert(result != nullptr && "Create PyClass failed");
			return result;
		}
	}

	typedef internal::_PyModule*	PyModule;
	typedef internal::_PyClass*		PyClass;
	typedef internal::_PyTuple		PyTuple;

	PYCURE_PYTYPE_PY2CPP(char*,			"s");
	PYCURE_PYTYPE_PY2CPP(const char*,	"s");
	PYCURE_PYTYPE_PY2CPP(int,			"i");
	PYCURE_PYTYPE_PY2CPP(char,			"c");
	PYCURE_PYTYPE_PY2CPP(double,		"d");
	PYCURE_PYTYPE_PY2CPP(float,			"f");

	PYCURE_PYTYPE_CPP2PY(char*,			"s");
	PYCURE_PYTYPE_CPP2PY(const char*,	"s");
	PYCURE_PYTYPE_CPP2PY(int,			"i");
	PYCURE_PYTYPE_CPP2PY(char,			"c");
	PYCURE_PYTYPE_CPP2PY(double,		"d");
	PYCURE_PYTYPE_CPP2PY(float,			"f");

	PYCURE_GET_PYOBJECT_ITEM(char*,			"s");
	PYCURE_GET_PYOBJECT_ITEM(const char*,	"s");
	PYCURE_GET_PYOBJECT_ITEM(int,			"i");
	PYCURE_GET_PYOBJECT_ITEM(char,			"c");
	PYCURE_GET_PYOBJECT_ITEM(double,		"d");
	PYCURE_GET_PYOBJECT_ITEM(float,			"f");

	inline PyModule pyLoadModule(const char* name)
	{
		return new internal::_PyModule(name);
	}
}