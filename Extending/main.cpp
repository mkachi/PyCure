#include <PyCure.hpp>

using namespace PyCure;

PY_FUNCTION(native_add)
{
	PY_PARAMETER(2);
	int a = args.get<int>(0);
	int b = args.get<int>(1);
	PY_RETURN(a + b);
}

PY_MODULE(sample)
{
	PY_MODULE_FUNC("add", native_add),
	PY_MODULE_FUNC_END
};

PY_EXPORT_MODULE(sample);