#ifndef __GX_MATERIAL_DATABASE_H__
#define __GX_MATERIAL_DATABASE_H__

#include <cstdint>

namespace gx
{
	uint16_t	create_material_id(void* material_address);

	void initialize_material_database();
	void shutdown_material_database();
}

#endif