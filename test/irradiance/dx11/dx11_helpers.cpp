#include "precompiled.h"

#include <dx11/dx11_helpers.h>

#include <dx11/dx11_error.h>
#include <dx11/dx11_pointers.h>

namespace dx11
{
	dx11::id3d11buffer_ptr create_vertex_buffer(ID3D11Device* device, const void* initial_data, uint32_t size )
	{
		D3D11_BUFFER_DESC desc = {};
		dx11::id3d11buffer_ptr result;
		desc.ByteWidth = size;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.Usage = D3D11_USAGE_DEFAULT;
		D3D11_SUBRESOURCE_DATA initial_data_dx = { initial_data, 0, 0};
		dx11::throw_if_failed<dx11::create_buffer_exception> (device->CreateBuffer(&desc, &initial_data_dx, dx11::get_pointer(result)));
		return result;
	}

	dx11::id3d11buffer_ptr create_index_buffer(ID3D11Device* device, const void* initial_data, uint32_t size )
	{
		D3D11_BUFFER_DESC desc = {};
		dx11::id3d11buffer_ptr result;
		desc.ByteWidth = size;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.Usage = D3D11_USAGE_DEFAULT;
		D3D11_SUBRESOURCE_DATA initial_data_dx = { initial_data, 0, 0};
		dx11::throw_if_failed<dx11::create_buffer_exception> (device->CreateBuffer(&desc, &initial_data_dx, dx11::get_pointer(result)));
		return result;
	}
}