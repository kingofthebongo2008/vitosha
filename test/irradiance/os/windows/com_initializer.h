#ifndef __OS_WINDOWS_COM_INITIALIZER_H__
#define __OS_WINDOWS_COM_INITIALIZER_H__

#include <exception>

#include <ole2.h>
#include <boost/noncopyable.hpp>

namespace os
{
    namespace windows
    {
        class com_exception : public std::exception
        {
            public:
            com_exception ( const HRESULT hr ) : exception("com exception")
                , m_hr (hr)  
            {

            }

            private:
            const HRESULT m_hr;
            com_exception& operator=(const com_exception&);
        };


        class com_initializer : private boost::noncopyable
        {
            public:
            com_initializer()
            {
                HRESULT hr  = ::CoInitializeEx(nullptr, COINIT_MULTITHREADED);

                bool success = (hr == S_OK || hr == S_FALSE);

                if ( !success )
                {
                    throw com_exception(hr);
                }
            }


            ~com_initializer()
            {
                ::CoUninitialize();
            }

        };
    }
}



#endif
