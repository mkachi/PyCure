#include <PyCure.h>

using namespace PyCure;

int main()
{
	PyCure_Initialize();

	PyModule module = nullptr;
	module = PyCure_ImportModule("Sample01");

	// Call Case1()
	CallFunc<void>(module, "Case1");

	// Call Case2()
	int result = CallFunc<int>(module, "Case2");
	printf("Case2 return : %d\n", result);

	// Call Case3()
	PyTuple tuple = TupleCallFunc(module, "Case3", 2);
	int num = GetTupleItem<int>(tuple, 0);
	const char* str = GetTupleItem<const char*>(tuple, 1);
	printf("Case3 return %d, %s\n", num, str);

	// Call Add
	result = CallFunc<int>(module, "Add", PyCureArgs(2, PyType(1), PyType(1)));
	printf("Add return %d\n", result);

	PyCure_Finalize();
	return 0;
}