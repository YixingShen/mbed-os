/* mbed Microcontroller Library
 * Copyright (c) 2006-2012 ARM Limited
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef MEMORYPOOL_H
#define MEMORYPOOL_H

#include <stdint.h>
#include <string.h>

#include "cmsis_os2.h"
#include "mbed_rtos1_types.h"
#include "mbed_rtos_storage.h"

namespace rtos {
/** \addtogroup rtos */
/** @{*/

/** Define and manage fixed-size memory pools of objects of a given type.
  @tparam  T         data type of a single object (element).
  @tparam  queue_sz  maximum number of objects (elements) in the memory pool.

 @note
 Memory considerations: The memory pool data store and control structures will be created on current thread's stack,
 both for the mbed OS and underlying RTOS objects (static or dynamic RTOS memory pools are not being used).
*/
template<typename T, uint32_t pool_sz>
class MemoryPool {
public:
    /** Create and Initialize a memory pool. */
    MemoryPool() {
        memset(_pool_mem, 0, sizeof(_pool_mem));
        memset(&_obj_mem, 0, sizeof(_obj_mem));
        memset(&_attr, 0, sizeof(_attr));
        _attr.mp_mem = _pool_mem;
        _attr.mp_size = sizeof(_pool_mem);
        _attr.cb_mem = &_obj_mem;
        _attr.cb_size = sizeof(_obj_mem);
        _id = osMemoryPoolNew(pool_sz, sizeof(T), &_attr);
        MBED_ASSERT(_id);
    }

    /** Allocate a memory block of type T from a memory pool.
      @return  address of the allocated memory block or NULL in case of no memory available.
    */
    T* alloc(void) {
        return (T*)osMemoryPoolAlloc(_id, 0);
    }

    /** Allocate a memory block of type T from a memory pool and set memory block to zero.
      @return  address of the allocated memory block or NULL in case of no memory available.
    */
    T* calloc(void) {
        T *item = (T*)osMemoryPoolAlloc(_id, 0);
        if (item != NULL) {
            memset(item, 0, sizeof(T));
        }
        return item;
    }

    /** Free a memory block.
      @param   address of the allocated memory block to be freed.
      @return  status code that indicates the execution status of the function.
    */
    osStatus free(T *block) {
        return osMemoryPoolFree(_id, (void*)block);
    }

private:
    osMemoryPoolId_t             _id;
    osMemoryPoolAttr_t           _attr;
    char                         _pool_mem[sizeof(T) * pool_sz];
    mbed_rtos_storage_mem_pool_t _obj_mem;
};

}
#endif

/** @}*/
