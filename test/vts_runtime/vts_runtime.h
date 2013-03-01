// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the VTS_RUNTIME_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// VTS_RUNTIME_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef VTS_RUNTIME_EXPORTS
#define VTS_RUNTIME_API __declspec(dllexport)
#else
#define VTS_RUNTIME_API __declspec(dllimport)
#endif

// This class is exported from the vts_runtime.dll
class VTS_RUNTIME_API Cvts_runtime {
public:
	Cvts_runtime(void);
	// TODO: add your methods here.
};

extern VTS_RUNTIME_API int nvts_runtime;

VTS_RUNTIME_API int fnvts_runtime(void);
