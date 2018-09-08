#include <PyCure.hpp>

using namespace PyCure;

int main()
{
	Py_Initialize();

	PyModule module = pyLoadModule("Embedding");
	int result = module->getVar<int>("sample");
	printf("get sample : %d\n", result);

	module->setVar("sample", 10);
	result = module->getVar<int>("sample");
	printf("set sample : %d\n", result);

	module->callFunc<void>("function1");
	printf("\n");

	int result1 = module->callFunc<int>("function2");
	printf("function2 result : %d\n\n", result1);

	PyTuple result2 = module->callFunc("function3", 2);
	int result2_0 = result2.get<int>(0);
	const char* result2_1 = result2.get<const char*>(1);
	printf("function3 result : %d, %s\n\n", result2_0, result2_1);

	PyClass sampleClass = module->createPyClass("SampleClass");
	int result3 = sampleClass->callMethod<int>("getA");
	printf("class SampleClass getA result : %d\n\n", result3);

	int result4 = sampleClass->getField<int>("_a");
	printf("class SampleClass field _a result : %d\n\n", result4);

	sampleClass->callMethod<void>("setA", PyCureArgs(1, pyType(100)));
	int result5 = sampleClass->getField<int>("_a");
	printf("class SampleClass field _a change : %d\n\n", result5);

	PyTuple result6 = sampleClass->callMethod("getMultiValue", 2);
	int result6_0 = result6.get<int>(0);
	int result6_1 = result6.get<int>(1);
	printf("class SampleClass getMultiValue result : %d, %d\n", result6_0, result6_1);

	Py_Finalize();

	delete sampleClass;
	delete module;

	return 0;
}