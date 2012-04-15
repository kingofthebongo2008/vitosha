#include "precompiled.h"

#include "point_lights.h"

#include <iterator>

#include <dx11/dx11_error.h>
#include <dx11/dx11_helpers.h>

#include <math/math_graphics.h>

#include <gx/gx_draw_call_collector.h>
#include <gx/gx_draw_call_collector_context.h>
#include <gx/gx_draw_call_context.h>

#include <gxu/gxu_entity_factory.h>

point_lights_entity::point_lights_entity
		( 
			gx::indexed_draw_call	draw_call,
			dx11::id3d11buffer_ptr	transform_color,
			dx11::id3d11inputlayout_ptr	input_layout,
			point_light_sphere_debug_vertex_shader	m_debug_vertex_shader,
			point_light_sphere_debug_pixel_shader	m_debug_pixel_shader
		) : 
	m_draw_call(draw_call)
	, m_transform_color(transform_color)
	, m_input_layout(input_layout)
	, m_debug_vertex_shader(m_debug_vertex_shader)
	, m_debug_pixel_shader(m_debug_pixel_shader)
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

	gx::draw_call_key key = gx::create_debug_layer_key(material_id, gx::get_perspective_transform3_depth(context) );
	collector->add_draw_call(key, context->m_entity_index, 0 ) ;
}

void point_lights_entity::update_instance_stream(ID3D11DeviceContext* device_context, math::matrix_float44 world_matrix, math::matrix_float44 view_matrix, math::matrix_float44 projection_matrix)
{
	dx11::d3d11_buffer_scope_lock buffer ( device_context, m_transform_color.get() );
	auto	buffer_data = reinterpret_cast< math::vector_float4* > (buffer.m_mapped_resource.pData );

	auto vp = math::mul( view_matrix, projection_matrix);

	//transform lights into world space and upload matrices and color
	for ( auto& light : m_lights)
	{
		auto translation = math::translation (  light.get_position() ) ;
		auto world = math::mul ( translation, world_matrix );
		auto wvp = math::mul( world , vp );

		math::store44(reinterpret_cast<float*> (buffer_data), wvp );
		buffer_data += 4;
		math::store4( reinterpret_cast<float*> (buffer_data), light.get_power() );
		buffer_data +=1;
	}
}

void point_lights_entity::on_execute_draw_calls(gx::draw_call_context* context)
{
	ID3D11DeviceContext* device_context = context->m_device_context;


	//execute debug draw call for all lights
	update_instance_stream(device_context, *context->m_world_matrix, *context->m_view_matrix, *context->m_projection_matrix );
	device_context->IASetInputLayout(m_input_layout.get());
	device_context->VSSetShader(m_debug_vertex_shader, nullptr, 0);
	device_context->PSSetShader(m_debug_pixel_shader, nullptr, 0 );

	m_draw_call.draw_instanced( device_context, static_cast<uint32_t> ( m_lights.size() ) );
}

std::shared_ptr<point_lights_entity> create_point_lights_entity(ID3D11Device* device)
{
	//create positions and index buffer
	std::tuple< dx11::id3d11buffer_ptr, dx11::id3d11buffer_ptr, uint32_t >	sphere = gxu::create_lat_lon_sphere_2(device, 0.2f, 10 );

	const uint32_t transform_buffer_size  = ( sizeof(math::matrix_float44) + sizeof(math::vector_float4) )  * point_lights_entity::max_light_count;
	std::vector<uint8_t> initial_data;
	initial_data.resize(transform_buffer_size);
	
	//create instance stream
	auto	transform_color = dx11::create_dynamic_vertex_buffer(device, &initial_data[0], transform_buffer_size  );

	//create shaders
	point_light_sphere_debug_vertex_shader	m_debug_vertex_shader(device);
	point_light_sphere_debug_pixel_shader	m_debug_pixel_shader(device);

	//create input layout
	dx11::id3d11inputlayout_ptr	input_layout;
	D3D11_INPUT_ELEMENT_DESC desc[] = 
	{
		{ "position",			0,	DXGI_FORMAT_R16G16B16A16_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "sphere_transform",	0,  DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "sphere_transform",	1,  DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "sphere_transform",	2,  DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "sphere_transform",	3,  DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "sphere_color",		0,  DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};

	dx11::throw_if_failed<dx11::create_input_layout> (device->CreateInputLayout(&desc[0], sizeof(desc) / sizeof(desc[0]) ,m_debug_vertex_shader.m_code, m_debug_vertex_shader.m_code_size, dx11::get_pointer(input_layout)));

	gx::indexed_draw_call::index_info info = {};
	info.m_index_count = std::get<2>(sphere);
	info.m_vertex_size_0 = 8;
	info.m_vertex_size_1 = sizeof(math::matrix_float44) + sizeof(math::vector_float4);

	return std::make_shared<point_lights_entity>( gx::indexed_draw_call(info, std::get<0>(sphere), transform_color, std::get<1>(sphere) )
		, transform_color
		, input_layout
		, m_debug_vertex_shader
		, m_debug_pixel_shader
		);
}



