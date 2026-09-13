#include <cstdlib>
#include <sys/reent.h>

/**
 * @brief Custom stubs to prevent linking standard dynamic memory allocation (newlib heap) 
 *        and exit-time destructor registration, enforcing compile-time or custom allocation handlers.
 */
extern "C" {

    void __register_exitproc(void) {
    }

    void* _malloc_r(struct _reent*, size_t) {
        while(1);
        return nullptr;
    }

    void _free_r(struct _reent*, void*) {
        while(1);
    }

    void* _realloc_r(struct _reent*, void*, size_t) { 
        while(1); 
        return nullptr; 
    }

    void* _calloc_r(struct _reent*, size_t, size_t) { 
        while(1); 
        return nullptr; 
    }
}
