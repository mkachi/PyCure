#pragma once

#include <Python.h>
#include <list>

#define PyCureArgs PyTuple_Pack

namespace PyCure
{
	using PyModule = PyObject*;
	using PyFunction = PyObject*;
	using PyVar = PyObject*;
	using PyArgs = PyObject*;

	namespace internal
	{
		static std::list<PyModule> __PyCureModules;
		class PyCureModule
		{
		protected:
			PyModule	_module;

		public:
			PyModule GetModule() { return _module; }

		};
		class _PyTuple
			: public PyCureModule
		{
		private:
			int			_count;
			int			_counter;

		public:
			_PyTuple() = delete;

			_PyTuple(PyModule module, int count)
				: _count(count), _counter(0)
			{
				assert(module != NULL && "Unassigned module!");
				_module = module;
			}

			_PyTuple(const _PyTuple& other)
				: _count(other._count), _counter(other._counter)
			{
				_module = other._module;
			}

			~_PyTuple()
			{
				if (_module != NULL)
				{
					Py_DECREF(_module);
				}
			}

			void Pull()
			{
				++_counter;
				if (_count <= _counter)
				{
					delete this;
				}
			}
		};
		class _PyClass
			: public PyCureModule
		{
		private:
			const char* _className;

		public:
			_PyClass() = delete;

			_PyClass(PyModule module, const char* className)
				: _className(className)
			{
				_module = PyObject_CallMethod(module, _className, NULL);
				assert(_module != NULL && "Class import Failed!");
				internal::__PyCureModules.push_back(_module);
			}

			_PyClass(const _PyClass& other)
				: _className(other._className)
			{
				_module = other._module;
			}

			~_PyClass()
			{	}

			template<typename T>
			inline void SetLocalVar(const char* name, T value)
			{
				SetVar<T>(_module, name, value);
			}

			template<typename T>
			inline T GetLocalVar(const char* name)
			{
				return GetVar<T>(_module, name);
			}

			template<typename T>
			inline T CallMethod(const char* name, PyArgs args = nullptr) noexcept
			{
				return CallFunc<T>(_module, name, args);
			}

			const char* const GetClassName() { return _className; }

		};
	}
	using PyTuple = internal::_PyTuple*;
	using PyClass = internal::_PyClass*;

	template<typename T>
	inline T PyType(PyVar value) noexcept
	{	}
#define PyCure_PyType_PyToCpp(_TYPE_, _ARG_) \
	template<> \
	inline _TYPE_ PyType<_TYPE_>(PyVar value) noexcept \
	{ \
		_TYPE_ result; \
		assert(value != NULL && "Unassigned variable!"); \
		PyArg_Parse(value, _ARG_, &result); \
		Py_DECREF(value); \
		return result; \
	}

	template<typename T>
	inline PyVar PyType(T value) noexcept
	{	}
#define PyCure_PyType_CppToPy(_TYPE_, _ARG_) \
	template<> \
	inline PyVar PyType(_TYPE_ value) noexcept \
	{ \
		PyVar result = Py_BuildValue(_ARG_, value); \
		assert(result!=NULL && "Value Build Failed!"); \
		return result; \
	}

	template<typename T>
	inline T GetTupleItem(PyTuple tuple, unsigned int index) noexcept
	{	}
#define PyCure_GetTupleItem(_TYPE_, _ARG_) \
	template<> \
	inline _TYPE_ GetTupleItem<_TYPE_>(PyTuple tuple, unsigned int index) noexcept \
	{ \
		_TYPE_ result; \
		PyArg_Parse(PyTuple_GetItem(tuple->GetModule(), index), _ARG_, &result); \
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
	inline void SetVar(PyModule module, const char* name, T value) noexcept
	{
		PyVar var = PyType(value);
		PyObject_SetAttrString(module, name, var);
	}

	template<typename T>
	inline T GetVar(PyModule module, const char* name) noexcept
	{
		PyVar var = PyObject_GetAttrString(module, name);
		assert(var != NULL && "Variable import failed");
		return PyType<T>(var);
	}

	template<typename T>
	inline T CallFunc(PyModule module, const char* name, PyArgs args = nullptr) noexcept
	{
		T result;
		PyFunction function = PyObject_GetAttrString(module, name);
		assert(function != NULL && "Function import failed");
		PyVar pyResult = PyObject_CallObject(function, args);
		if (pyResult == Py_None || pyResult == NULL)
		{
			assert(!"This function has no return value.");
		}
		else
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
	inline void CallFunc<void>(PyModule module, const char* name, PyVar args) noexcept
	{
		PyFunction function = PyObject_GetAttrString(module, name);
		assert(function != NULL && "Function import failed");
		PyVar pyResult = PyObject_CallObject(function, args);
		if (args != nullptr)
		{
			Py_DECREF(args);
		}
		if (pyResult != NULL)
		{
			Py_DECREF(pyResult);
		}
		Py_DECREF(function);
	}

	inline PyTuple TupleCallFunc(PyModule module, const char* name, unsigned int tupleCount, PyArgs args = nullptr) noexcept
	{
		assert(tupleCount >= 0 && "tupleCount is to small");

		PyFunction function = PyObject_GetAttrString(module, name);
		assert(function != NULL && "Function import failed");
		PyVar pyResult = PyObject_CallObject(function, args);
		if (args != nullptr)
		{
			Py_DECREF(args);
		}
		Py_DECREF(function);
		return new internal::_PyTuple(pyResult, tupleCount);
	}

	inline void PyCure_Initialize()
	{
		Py_Initialize();
	}

	inline void PyCure_Finalize()
	{
		for (auto iter = internal::__PyCureModules.begin(); iter != internal::__PyCureModules.end(); ++iter)
		{
			if ((*iter) != NULL)
			{
				Py_DECREF((*iter));
			}
		}
		internal::__PyCureModules.clear();
		Py_Finalize();
	}

	inline PyModule PyCure_ImportModule(const char* name)
	{
		PyModule result = PyImport_ImportModule(name);
		if (result == NULL)
		{
			assert(!"Module import Failed!");
		}
		internal::__PyCureModules.push_back(result);
		return result;
	}

	inline void ClearModule(PyModule module)
	{
		if (module != NULL)
		{
			internal::__PyCureModules.erase(
				std::find(internal::__PyCureModules.cbegin(), internal::__PyCureModules.cend(), module));
			Py_DECREF(module);
		}
		else
		{
			assert(!"Unassigned module!");
		}
	}

	inline void ClearModule(PyClass pyClass)
	{
		PyModule module = pyClass->GetModule();
		if (module != NULL)
		{
			internal::__PyCureModules.erase(
				std::find(internal::__PyCureModules.cbegin(), internal::__PyCureModules.cend(), module));
			Py_DECREF(module);
		}
		else
		{
			assert(!"Unassigned module!");
		}
	}

	inline PyClass CreatePyClass(PyModule module, const char* className)
	{
		PyClass result = new internal::_PyClass(module, className);
		return result;
	}
}