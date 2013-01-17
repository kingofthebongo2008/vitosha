#ifndef __GXU_TEXTURE_LOADING_H__
#define __GXU_TEXTURE_LOADING_H__

#include <cstdint>

#include <d3d11/d3d11_pointers.h>

#include <directxtk/inc/ddstextureloader.h>
#include <directxtk/inc/wictextureloader.h>



namespace gxu
{
	inline d3d11::itexture2d_ptr load_texture_dds(ID3D11Device* device, const wchar_t* file_name) throw()
	{
		d3d11::itexture2d_ptr result;
		d3d11::iresource_ptr  resource;

		typedef boost::intrusive_ptr<ID3D11Resource>			iresource_ptr;
		typedef boost::intrusive_ptr<ID3D11Texture2D>			itexture2d_ptr;

		HRESULT hresult = DirectX::CreateDDSTextureFromFile(device, file_name, dx::get_pointer(resource), 0);

		if (hresult == S_OK)
		{
			hresult = resource->QueryInterface(__uuidof(ID3D11Texture2D),dx::get_pointer_void(result));

			if (hresult == S_OK)
			{
				return std::move(result);
			}
		}

		return result;
	}

	inline d3d11::itexture2d_ptr load_texture_wic(ID3D11Device* device, ID3D11DeviceContext* device_context, const wchar_t* file_name)  throw()
	{
		d3d11::itexture2d_ptr result;
		d3d11::iresource_ptr  resource;

		typedef boost::intrusive_ptr<ID3D11Resource>			iresource_ptr;
		typedef boost::intrusive_ptr<ID3D11Texture2D>			itexture2d_ptr;

		HRESULT hresult = DirectX::CreateWICTextureFromFile(device, device_context,  file_name, dx::get_pointer(resource), 0);

		if (hresult == S_OK)
		{
			hresult = resource->QueryInterface(__uuidof(ID3D11Texture2D),dx::get_pointer_void(result));

			if (hresult == S_OK)
			{
				return std::move(result);
			}
		}

		return result;
	}

}

#endif