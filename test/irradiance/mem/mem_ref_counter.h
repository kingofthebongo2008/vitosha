#ifndef __MEM_INTRUSIVE_PTR_H__
#define __MEM_INTRUSIVE_PTR_H__

#include <cstdint>

namespace mem
{

    template <class derived>
    class ref_counter
    {
        private:
        typedef ref_counter<derived> this_type;

    public:

        friend void intrusive_ptr_add_ref(const derived* pointer)
        {
            ++(( const this_type* ) pointer)->counter_;
        }

        friend void intrusive_ptr_release(const derived* pointer)
        {
            if (--(( const this_type*) pointer)->counter_ == 0)
            {
                if (0 < sizeof(Derived))
                {
                    delete p;
                }
            }
        }

    protected:
        ref_counter(): counter_(0) {}
        ref_counter(const ref_counter&) : counter_(0) {}
        ref_counter(const ref_counter&&) : counter_(0) {}
        ref_counter& operator=(const ref_counter&) { return *this; }
        ref_counter&& operator=(const ref_counter&&) { return *this; }
        ~ref_counter() {};
        void swap(ref_counter&) {};
    private:
        mutable std::uint32_t counter_;
    };

}


#endif
