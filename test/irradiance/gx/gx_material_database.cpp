#include "precompiled.h"

#include <gx/gx_material_database.h>

#include <sys/sys_spin_lock.h>

namespace gx
{
	struct material_database
	{
		material_database() : m_slots(), m_slots_used(0)
		{

		}
								sys::spinlock_fas	m_lock;
								uintptr_t			m_slots[65536];
		__declspec(align(64))	uint16_t			m_slots_used;
	};

	static material_database s_material_database;

	uint16_t create_material_id(void* material_address)
	{
		sys::lock<sys::spinlock_fas> l(s_material_database.m_lock);
		s_material_database.m_slots[s_material_database.m_slots_used] = reinterpret_cast<uintptr_t>(material_address);
		++s_material_database.m_slots_used;
		return s_material_database.m_slots_used;
	}

	void initialize_material_database()
	{

	}

	void shutdown_material_database()
	{

	}
}