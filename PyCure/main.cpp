#include <PyCure.h>

using namespace PyCure;

//PyObject* TestFunc(PyObject* self)
//{
//	PyObject* result;
//	printf("Call Cpp TestFunc!\n");
//	Py_INCREF(Py_None);
//	result = Py_None;	// return 없음
//	return result;
//}
//
//PyObject* Multiple(PyObject* self, PyObject* args)
//{
//	PyObject* result;
//	int a, b;
//	Py_INCREF(args);
//	PyArg_ParseTuple(args, "II", &a, &b);
//	result = PyType(a * b);
//	return result;
//}
//
//// 함수를 등록
//// METH_KEYWORDS	: 함수가 PyCFunctionWithKeywords 타입 인자는 PyArg_ParseTupleAndKeywords함수를 사용
//// METH_NORARGS		: 인자가 없음
//// METH_O			: 인자가 하나인 경우
//// METH_CLASS		: 클래스의 메소드일 경우
//// METH_STATIC		: 클래스의 메소드일 경우
//static PyMethodDef ExtMethods[] =
//{ 
//	{ "TestFunc", (PyCFunction)TestFunc, METH_NOARGS, "Excute a TestFunc" },{ NULL, NULL, 0, NULL },
//	{ "Multiple", (PyCFunction)Multiple, METH_VARARGS, "Excute a Multiple" },{ NULL, NULL, 0, NULL }
//};
//
//// 모듈에 등록
//static PyModuleDef PyExtModule =
//{ PyModuleDef_HEAD_INIT, "PyCureDemo", NULL, -1, ExtMethods };
//
//PyMODINIT_FUNC PyInit_Ext1()
//{
//	// 모듈을 생성
//	return PyModule_Create(&PyExtModule);
//}

int main()
{
//	PyImport_AppendInittab("PyCureDemo", PyInit_Ext1);
	PyCure_Initialize();

	PyModule module = PyCure_ImportModule("Sample01");

	int result;

	// Get global variable 'o'
	result = GetVar<int>(module, "o");
	printf("Get o : %d\n", result);

	// Set global variable 'o'
	printf("\n");
	SetVar(module, "o", 9999);
	result = GetVar<int>(module, "o");
	printf("Set o : %d\n", result);

	// Call function 'Case1()'
	printf("\n");
	CallFunc<void>(module, "Case1");

	// Call function 'Case2()'
	printf("\n");
	result = CallFunc<int>(module, "Case2");
	printf("Case2 return : %d\n", result);

	// Call function 'Case3(a)'
	printf("\n");
	PyTuple tuple = TupleCallFunc(module, "Case3", 2);
	result = GetTupleItem<int>(tuple, 0);
	const char* str = GetTupleItem<const char*>(tuple, 1);
	printf("Case3 return : %d, %s\n", result, str);

	// Call function 'Add(a, b)'
	printf("\n");
	result = CallFunc<int>(module, "Add", PyCureArgs(2, PyType(1), PyType(1)));
	printf("Add return : %d\n", result);

	// Import Python class
	PyClass Ang = CreatePyClass(module, "Ang");

	// Get class Ang local variable '_a'
	printf("\n");
	result = Ang->GetLocalVar<int>("_a");
	printf("Get class Ang _a : %d\n", result);

	// Set class Ang local variable '_a'
	printf("\n");
	Ang->SetLocalVar("_a", 99);
	result = Ang->GetLocalVar<int>("_a");
	printf("Set class Ang _a : %d\n", result);

	// Call class Ang Method 'GetA()'
	printf("\n");
	result = Ang->CallMethod<int>("GetA");
	printf("class Ang GetA return : %d\n", result);

	// Call class Ang Method 'SetA(a)'
	printf("\n");
	Ang->CallMethod<void>("SetA", PyCureArgs(1, PyType(100)));
	result = Ang->CallMethod<int>("GetA");
	printf("class Ang SetA : %d\n", result);

	PyCure_Finalize();
	return 0;
}