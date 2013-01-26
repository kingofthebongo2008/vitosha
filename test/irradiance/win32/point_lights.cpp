#include "precompiled.h"

#include "point_lights.h"

#include <iterator>

#include <d3d11/d3d11_error.h>
#include <d3d11/d3d11_helpers.h>

#include <math/math_graphics.h>

#include <gx/gx_draw_call_collector.h>
#include <gx/gx_draw_call_collector_context.h>
#include <gx/gx_draw_call_context.h>
#include <gx/gx_profile.h>

#include <gxu/gxu_entity_factory.h>

point_lights_entity::point_lights_entity
		( 
			gx::indexed_draw_call<3, gx::bit_16>	draw_call,
			d3d11::ibuffer_ptr	transform_color,
			d3d11::iinputlayout_ptr	input_layout,
			point_lights_entity::shader_info info
		) : 
	m_draw_call(draw_call)
	, m_transform_color(transform_color)
	, m_input_layout(input_layout)
	, m_debug_vertex_shader(info.m_debug_vertex_shader)
	, m_debug_pixel_shader(info.m_debug_pixel_shader)
	, m_cbuffer(info.m_cbuffer)
{
	m_lights.reserve(max_light_count);
}

void point_lights_entity::add_lights( const point_light* begin, const point_light* end)
{
	m_lights.reserve ( end - begin );
	std::copy( begin, end, std::back_inserter(m_lights) );
}

void point_lights_entity::update_light_positions( float dt)
{
	dt;
}

void point_lights_entity::on_create_draw_calls( gx::draw_call_collector_context* context, gx::draw_call_collector* collector)
{
	uint16_t material_id = 2;

	gx::draw_call_key key = gx::create_debug_draw_call(material_id, gx::get_perspective_transform3_depth(context), std::bind(&point_lights_entity::on_execute_draw_calls, this, std::placeholders::_1) );
	collector->add_draw_call(key, context->m_entity_index ) ;

	//gx::draw_call_key key2  = gx::create_light_draw_call( std::bind(&point_lights_entity::on_execute_draw_calls, this, std::placeholders::_1)  );
	//collector->add_draw_call(key2, context->m_entity_index ) ;
}

void point_lights_entity::update_instance_stream(ID3D11DeviceContext* device_context, math::float4x4 world_matrix)
{
	d3d11::d3d11_buffer_scope_lock buffer ( device_context, m_transform_color.get() );
	auto	buffer_data = reinterpret_cast< math::float4* > (buffer.m_mapped_resource.pData );

	//transform lights into world space and upload matrices and color
	for ( auto& light : m_lights)
	{
		auto translation = math::translation (  light.get_position() ) ;
		auto world = math::mul ( translation, world_matrix );

		math::store44(reinterpret_cast<float*> (buffer_data), world );
		buffer_data += 4;
		math::store4( reinterpret_cast<float*> (buffer_data), light.get_power() );
		buffer_data +=1;
	}
}

void point_lights_entity::on_execute_draw_calls(gx::draw_call_context* context)
{
	return;
    gx::profile p(L"point_lights_entity");
	ID3D11DeviceContext* device_context = context->m_device_context;

	//execute debug draw call for all lights
	update_instance_stream(device_context, *context->m_world_matrix );

	//update view projection
	auto vp = math::mul( *context->m_view_matrix, *context->m_projection_matrix );
	m_cbuffer.set_vp(vp);
	m_cbuffer.flush(device_context);
	m_cbuffer.bind_as_vertex_constant_buffer(device_context);

	device_context->IASetInputLayout(m_input_layout.get());

    d3d11::vs_set_shader(device_context, m_debug_vertex_shader );
    d3d11::ps_set_shader(device_context, m_debug_pixel_shader );
    
	m_draw_call.draw_instanced( device_context, static_cast<uint32_t> ( m_lights.size() ) );
}

std::shared_ptr<point_lights_entity> create_point_lights_entity(ID3D11Device* device)
{
	//create positions and index buffer
	std::tuple< d3d11::ibuffer_ptr,  d3d11::ibuffer_ptr, d3d11::ibuffer_ptr, uint32_t >	sphere = gxu::create_lat_lon_sphere_2(device, 0.2f, 20 );

	const uint32_t transform_buffer_size  = ( sizeof(math::float4x4) + sizeof(math::float4) )  * point_lights_entity::max_light_count;
	std::vector<uint8_t> initial_data;
	initial_data.resize(transform_buffer_size);
	
	//create instance stream
	auto	transform_color = d3d11::create_dynamic_vertex_buffer(device, &initial_data[0], transform_buffer_size  );

	//create shaders
	point_light_sphere_debug_vertex_shader	debug_vertex_shader(device);
	point_light_sphere_debug_pixel_shader	debug_pixel_shader(device);

	//create constant buffer for vertex shader
	point_light_sphere_debug_vertex_shader_constant_buffer cbuffer(device);

	point_lights_entity::shader_info info(debug_vertex_shader, debug_pixel_shader, cbuffer);

	//create input layout
	d3d11::iinputlayout_ptr	input_layout;
	D3D11_INPUT_ELEMENT_DESC desc[] = 
	{
		{ "position",			0,	DXGI_FORMAT_R16G16B16A16_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "normal",				0,	DXGI_FORMAT_R16G16B16A16_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "texcoord",			0,  DXGI_FORMAT_R16G16_FLOAT,		1, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "sphere_transform",	0,  DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "sphere_transform",	1,  DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "sphere_transform",	2,  DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "sphere_transform",	3,  DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "sphere_color",		0,  DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};

	dx::throw_if_failed<d3d11::create_input_layout> (device->CreateInputLayout(&desc[0], sizeof(desc) / sizeof(desc[0]) ,debug_vertex_shader.m_code, debug_vertex_shader.m_code_size, dx::get_pointer(input_layout)));

	return std::make_shared<point_lights_entity>( 
		gx::create_indexed_draw_call< 8, 12,  sizeof(math::float4x4) + sizeof(math::float4), gx::bit_16 > (std::get<3>(sphere), std::get<0>(sphere), std::get<1>(sphere), transform_color, std::get<2>(sphere) )
		, transform_color
		, input_layout
		, info
		);
}



