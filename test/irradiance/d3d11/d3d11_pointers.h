#ifndef __d3d11_POINTERS_H__
#define __d3d11_POINTERS_H__

#include <boost/intrusive_ptr.hpp>

#include <D3D11.h>

#include <dx/dx_pointers.h>

namespace d3d11
{
    typedef boost::intrusive_ptr<ID3D11Device>				idevice_ptr;
    typedef boost::intrusive_ptr<ID3D11DeviceContext>		idevicecontext_ptr;

    typedef boost::intrusive_ptr<ID3D11CommandList>			icommandlist_ptr;

    typedef boost::intrusive_ptr<ID3D11Buffer>				ibuffer_ptr;

    typedef boost::intrusive_ptr<ID3D11Resource>			iresource_ptr;
    typedef boost::intrusive_ptr<ID3D11Texture2D>			itexture2d_ptr;
    typedef boost::intrusive_ptr<ID3D11RenderTargetView>	id3d11rendertargetview_ptr;


    typedef boost::intrusive_ptr<ID3D11ShaderResourceView>	ishaderresourceview_ptr;
    typedef boost::intrusive_ptr<ID3D11SamplerState>		isamplerstate_ptr;

    typedef boost::intrusive_ptr<ID3D11DepthStencilState>	idepthstencilstate_ptr;
    typedef boost::intrusive_ptr<ID3D11BlendState>			iblendstate_ptr;
	typedef boost::intrusive_ptr<ID3D11RasterizerState>		irasterizerstate_ptr;

    typedef boost::intrusive_ptr<ID3D11DepthStencilView>	idepthstencilview_ptr;

	typedef boost::intrusive_ptr<ID3D11VertexShader>		ivertexshader_ptr;
	typedef boost::intrusive_ptr<ID3D11PixelShader>			ipixelshader_ptr;
    typedef boost::intrusive_ptr<ID3D11GeometryShader>		igeometryshader_ptr;

	typedef boost::intrusive_ptr<ID3D11InputLayout>			iinputlayout_ptr;
}

#endif

