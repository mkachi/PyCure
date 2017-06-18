#include <PyCure.h>

using namespace PyCure;

int main()
{
    PyCure_Initialize();

    PyModule module = PyCure_ImportModule("EmbeddingTest");

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

    // Clear class Module
    ClearModule(Ang);

    PyCure_Finalize();
    return 0;
}