#include "precompiled.h"

#include <dx11/dx11_helpers.h>

#include <dx11/dx11_error.h>
#include <dx11/dx11_pointers.h>

namespace dx11
{
	namespace
	{
		static dx11::id3d11buffer_ptr create_vertex_buffer(ID3D11Device* device, const void* initial_data, uint32_t size, D3D11_USAGE dx11_bind_flags, uint32_t  cpu_access_flags )
		{
			D3D11_BUFFER_DESC desc = {};
			dx11::id3d11buffer_ptr result;
			desc.ByteWidth = size;
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc.CPUAccessFlags = cpu_access_flags;
			desc.Usage = dx11_bind_flags;
			D3D11_SUBRESOURCE_DATA initial_data_dx = { initial_data, 0, 0};
			dx11::throw_if_failed<dx11::create_buffer_exception> (device->CreateBuffer(&desc, &initial_data_dx, dx11::get_pointer(result)));
			return result;
		}

		static dx11::id3d11buffer_ptr create_index_buffer(ID3D11Device* device, const void* initial_data, uint32_t size, D3D11_USAGE dx11_bind_flags, uint32_t cpu_access_flags  )
		{
			D3D11_BUFFER_DESC desc = {};
			dx11::id3d11buffer_ptr result;
			desc.ByteWidth = size;
			desc.BindFlags = D3D11_BIND_INDEX_BUFFER ;
			desc.CPUAccessFlags = cpu_access_flags;
			desc.Usage = dx11_bind_flags;
			D3D11_SUBRESOURCE_DATA initial_data_dx = { initial_data, 0, 0};
			dx11::throw_if_failed<dx11::create_buffer_exception> (device->CreateBuffer(&desc, &initial_data_dx, dx11::get_pointer(result)));
			return result;
		}
	}

	dx11::id3d11buffer_ptr create_constant_buffer(ID3D11Device* device, uint32_t size)
	{
		D3D11_BUFFER_DESC desc = {};
		dx11::id3d11buffer_ptr result;

		desc.ByteWidth = size;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		dx11::throw_if_failed<dx11::create_buffer_exception> (device->CreateBuffer(&desc, nullptr, dx11::get_pointer(result) ));
		return result;
	}

	dx11::id3d11buffer_ptr create_default_vertex_buffer(ID3D11Device* device, const void* initial_data, uint32_t size )
	{
		return create_vertex_buffer(device, initial_data, size, D3D11_USAGE_DEFAULT, 0);
	}

	dx11::id3d11buffer_ptr create_default_index_buffer(ID3D11Device* device, const void* initial_data, uint32_t size )
	{
		return create_index_buffer(device, initial_data, size, D3D11_USAGE_DEFAULT, 0);
	}

	dx11::id3d11buffer_ptr create_dynamic_vertex_buffer(ID3D11Device* device, const void* initial_data, uint32_t size )
	{
		return create_vertex_buffer(device, initial_data, size, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE );
	}

	dx11::id3d11buffer_ptr create_dynamic_index_buffer(ID3D11Device* device, const void* initial_data, uint32_t size )
	{
		return create_index_buffer(device, initial_data, size, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE );
	}

	dx11::id3d11buffer_ptr create_immutable_vertex_buffer(ID3D11Device* device, const void* initial_data, uint32_t size )
	{
		return create_vertex_buffer(device, initial_data, size, D3D11_USAGE_IMMUTABLE, 0 );
	}

	dx11::id3d11buffer_ptr create_immutable_index_buffer(ID3D11Device* device, const void* initial_data, uint32_t size )
	{
		return create_index_buffer(device, initial_data, size, D3D11_USAGE_IMMUTABLE, 0);
	}
}
