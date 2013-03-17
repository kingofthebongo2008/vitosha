#include "precompiled.h"

#include <mesh_utils.h>


namespace mesh
{
    template <typename t> struct triangle;

    template <> struct triangle<std::uint32_t>
    {
        std::uint32_t m_index_0;
        std::uint32_t m_index_1;
        std::uint32_t m_index_2;
    };

    template <> struct triangle<std::uint16_t>
    {
        std::uint16_t m_index_0;
        std::uint16_t m_index_1;
        std::uint16_t m_index_2;
    };

    template <typename t> struct index_buffer;
    
    template <>
    struct index_buffer<std::uint32_t>
    {
        index_buffer(	const std::uint8_t* indices_ptr,
                        const std::uint32_t index_stride,
                        const std::uint32_t index_count ) : m_indices_ptr(indices_ptr), m_index_stride(index_stride), m_index_count(index_count)
                        {

                        }

        std::uint32_t operator[](const std::uint32_t i) const
        {
            return * ( reinterpret_cast<std::uint32_t*> ( m_indices_ptr[ i * m_index_stride ] ) );
        }

        const std::uint32_t size() const
        {
            return m_index_count;
        }

        bool empty() const
        {
            return m_index_count == 0;
        }

        const std::uint8_t* m_indices_ptr;
        const std::uint32_t m_index_stride;
        const std::uint32_t m_index_count;

        private:

        index_buffer();
        index_buffer(const index_buffer&);
        const index_buffer& operator=(const index_buffer&);
    };

    template <> struct index_buffer<std::uint16_t>
    {
        index_buffer(	const std::uint8_t* indices_ptr,
                        const std::uint32_t index_stride,
                        const std::uint32_t index_count ) : m_indices_ptr(indices_ptr), m_index_stride(index_stride), m_index_count(index_count)
                        {

                        }

        std::uint16_t operator[](const std::uint32_t i) const
        {
            return * ( reinterpret_cast<std::uint16_t*> ( m_indices_ptr[ i * m_index_stride ] ) );
        }

        const std::uint32_t size() const
        {
            return m_index_count;
        }

        bool empty() const
        {
            return m_index_count == 0;
        }

        const std::uint8_t* m_indices_ptr;
        const std::uint32_t m_index_stride;
        const std::uint32_t m_index_count;

        private:

        index_buffer();
        index_buffer(const index_buffer&);
        const index_buffer& operator=(const index_buffer&);
    };


    validation_result validate( const vertex* , const std::uint32_t , const std::uint32_t , void*, const std::uint32_t , const std::uint32_t )
    {
        /*
        const std::uint8_t* indices_ptr  = reinterpret_cast<const std::uint8_t*>(indices);

        index_buffer<std::uint32_t> indices_arr(indices_ptr, index_stride, index_count);

        std::uint32_t i  = indices_arr[0];

        //step 1.
        if (index_count == 0 || index_count % 3 != 0)
        {
            return validation_result::invalid_number_of_triangles;
        }
         */
        return validation_result::success;
       
    }
}