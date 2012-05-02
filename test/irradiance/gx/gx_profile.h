#ifndef __GX_PROFILE_H__
#define __GX_PROFILE_H__

#include <d3d9.h>

#include <gx/gx_color.h>

namespace gx
{
    class profile
    {
        public:

        explicit profile(const wchar_t* description)
        {
            D3DPERF_BeginEvent(0, description );
        }

        ~profile()
        {
            D3DPERF_EndEvent();
        }

    };
}



#endif

