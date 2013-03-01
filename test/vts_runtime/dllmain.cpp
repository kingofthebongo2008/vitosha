// dllmain.cpp : Defines the entry point for the DLL application.
#include "precompiled.h"

#include <mem/mem_streamflow.h>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    using namespace mem::streamflow;

	switch (ul_reason_for_call)
	{
	    case DLL_PROCESS_ATTACH:
        {
            if ( initialize() == initialization_code::success)
            {
                return (thread_initialize() == initialization_code::success) ? TRUE : FALSE;
            }
            else
            {
                return FALSE;
            }
        }

	    case DLL_THREAD_ATTACH:
        {
            return ( thread_initialize() == initialization_code::success) ? TRUE : FALSE;
        }

	    case DLL_THREAD_DETACH:
        {
            if ( lpReserved == nullptr)
            {
                thread_finalize();
            }
            break;
        }
	    case DLL_PROCESS_DETACH:
        {
            if ( lpReserved == nullptr)
            {
                mem::streamflow::finalize();
            }
            break;
        }
		break;
	}
	return TRUE;
}

