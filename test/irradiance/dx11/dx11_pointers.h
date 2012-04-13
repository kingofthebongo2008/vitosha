#ifndef __dx11_POINTERS_H__
#define __dx11_POINTERS_H__

#include <boost/intrusive_ptr.hpp>

#include <D3D11.h>

namespace dx11
{
    typedef boost::intrusive_ptr<IDXGIFactory>				idxgifactory_ptr;
    typedef boost::intrusive_ptr<IDXGIFactory1>				idxgifactory1_ptr;

    typedef boost::intrusive_ptr<IDXGIAdapter>				idxgiadapter_ptr;
    typedef boost::intrusive_ptr<IDXGISwapChain>			idxgiswapchain_ptr;

    typedef boost::intrusive_ptr<ID3D11Device>				id3d11device_ptr;
    typedef boost::intrusive_ptr<ID3D11DeviceContext>		id3d11devicecontext_ptr;

    typedef boost::intrusive_ptr<ID3D11CommandList>			id3d11commandlist_ptr;

    typedef boost::intrusive_ptr<ID3D11Buffer>				id3d11buffer_ptr;

    typedef boost::intrusive_ptr<ID3D11Texture2D>			id3d11texture2d_ptr;
    typedef boost::intrusive_ptr<ID3D11RenderTargetView>	id3d11rendertargetview_ptr;


    typedef boost::intrusive_ptr<ID3D11ShaderResourceView>	id3d11shaderresourceview_ptr;
    typedef boost::intrusive_ptr<ID3D11SamplerState>		id3d11samplerstate_ptr;

    typedef boost::intrusive_ptr<ID3D11DepthStencilState>	id3d11depthstencilstate_ptr;
    typedef boost::intrusive_ptr<ID3D11BlendState>			id3d11blendstate_ptr;
	typedef boost::intrusive_ptr<ID3D11RasterizerState>		id3d11rasterizerstate_ptr;

    typedef boost::intrusive_ptr<ID3D11DepthStencilView>	id3d11depthstencilview_ptr;

	typedef boost::intrusive_ptr<ID3D11VertexShader>		id3d11vertexshader_ptr;
	typedef boost::intrusive_ptr<ID3D11PixelShader>			id3d11pixelshader_ptr;

	typedef boost::intrusive_ptr<ID3D11InputLayout>			id3d11inputlayout_ptr;

    template<typename T> 
    inline T** get_pointer( boost::intrusive_ptr<T>& ptr )
    {
        return reinterpret_cast<T**>(&(ptr));
    }
}

inline void intrusive_ptr_release( IUnknown* px )
{
    px->Release();
}

inline void intrusive_ptr_add_ref( IUnknown* px )
{
    px->AddRef();
}

#endif

