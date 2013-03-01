// vts_runtime.cpp : Defines the exported functions for the DLL application.
//

#include "precompiled.h"
#include "vts_runtime.h"


// This is an example of an exported variable
VTS_RUNTIME_API int nvts_runtime=0;

// This is an example of an exported function.
VTS_RUNTIME_API int fnvts_runtime(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see vts_runtime.h for the class definition
Cvts_runtime::Cvts_runtime()
{
	return;
}
