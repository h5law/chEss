/* dsa.c
 * Copyright 2025 h5law <dev@h5law.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS”
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "dsa.h"

static int memvacmp(void *memory, unsigned char val, size_t size)
{
    unsigned char *mm = ( unsigned char * )memory;
    return (*mm == val) && memcmp(mm, mm + 1, size - 1) == 0;
}

static int memvcmp(void *memory, unsigned char val, size_t size)
{
    unsigned char *mm = ( unsigned char * )memory;
    return (*mm == val) && memcmp(mm, mm + 1, size - 1) == 0;
}

////////////////////////////////////////////////////////////////////////////////
//                        Dynamic Array/Vector Methods                        //
////////////////////////////////////////////////////////////////////////////////

vector_t vector_init(size_t min_capacity, double load_factor, size_t elem_size)
{
    vector_t vec = {0};

    if (load_factor <= 0 || load_factor > 1 || min_capacity <= 0 ||
        elem_size <= 0)
        return vec;

    void *array;
    array = calloc(min_capacity, elem_size);
    if (array == NULL)
        return vec;

    vec.capacity    = min_capacity;
    vec.load_factor = load_factor;
    vec.array       = array;

    return vec;
}

void *vector_gpos(vector_t *vec, size_t index, size_t elem_size)
{
    if (!vec || !vec->array || index < 0 || index >= vec->capacity ||
        elem_size <= 0)
        return NULL;
    return ( char * )vec->array + (index * elem_size);
}

int vector_spos(vector_t *vec, size_t index, void *elem, size_t elem_size)
{
    if (!vec || !vec->array || index < 0 || index >= vec->capacity ||
        elem_size <= 0)
        return ERR_IARGS;
    int new_size = vec->size;
    if (memvcmp((( char * )(vec->array)) + (index * elem_size),
                ( unsigned char )0, elem_size)) {
        new_size = vec->size + 1;
    }
    if (( double )new_size >= ( double )vec->capacity * vec->load_factor) {
        void  *new_arr;
        size_t new_cap = vec->capacity * 2;
        new_arr        = calloc(new_cap, elem_size);
        if (new_arr == NULL)
            return ERR_ALLOC;
        memmove(new_arr, ( char * )vec->array, vec->capacity * elem_size);
        free(vec->array);
        vec->array    = new_arr;
        vec->capacity = new_cap;
    }
    memmove(( char * )vec->array + (index * elem_size), elem, elem_size);
    vec->size = new_size;
    if (index >= vec->end_ptr)
        vec->end_ptr = index + 1;
    return ERR_OKAY;
}

int vector_zpos(vector_t *vec, size_t index, size_t elem_size)
{
    if (!vec || !vec->array || vec->size == 0 || index < 0 ||
        index >= vec->end_ptr || elem_size <= 0)
        return ERR_IARGS;
    int new_size = vec->size - 1;
    if (memvcmp((( char * )(vec->array)) + (index * elem_size),
                ( unsigned char )0, elem_size)) {
        return ERR_OKAY;
    }
    // check for shrink?
    memset(( char * )vec->array + (index * elem_size), 0, elem_size);
    vec->size = new_size;
    if (vec->end_ptr == index + 1)
        vec->end_ptr = index;
    return ERR_OKAY;
}

int vector_push(vector_t *vec, void *elem, size_t elem_size)
{
    return vector_spos(vec, vec->end_ptr, elem, elem_size);
}

int vector_compact(vector_t *vec, size_t elem_size)
{
    if (!vec || !vec->array || vec->size == 0)
        return ERR_IARGS;

    int curr, left, right;
    left  = 0;
    right = 0;
    size_t lsize, rsize;
    lsize = 0;
    rsize = 0;
    while (left + (lsize / elem_size) <= vec->end_ptr) {
        curr = memvcmp((( char * )(vec->array)) + (left * elem_size) + lsize,
                       ( unsigned char )0, elem_size);
        if (curr) {
            lsize += elem_size;
            continue;
        }
        if (left == 0 && lsize == 0) {
            ++left;
            continue;
        }
        right = left + (lsize / elem_size);
        curr  = memvcmp((( char * )(vec->array)) + (right * elem_size) + rsize,
                        ( unsigned char )0, elem_size);
        if (!curr && right + (rsize / elem_size) < vec->end_ptr) {
            rsize += elem_size;
            continue;
        }
        memmove((( char * )(vec->array)) + (left * elem_size),
                (( char * )(vec->array)) + (right * elem_size), rsize);
        size_t offset = rsize > lsize ? rsize - lsize : lsize - rsize;
        memset((( char * )(vec->array)) + (left * elem_size) + rsize, 0,
               elem_size + rsize + offset);
        if (rsize < lsize)
            left = left + (rsize / elem_size);
        else
            left = right + (rsize / elem_size);
        lsize = 0;
        rsize = 0;
    }

    vec->end_ptr = vec->size;
    return ERR_OKAY;
}

vector_t vector_clone(vector_t *vec, size_t elem_size)
{
    vector_t dup = {0};
    void    *array;
    array = calloc(vec->capacity, elem_size);
    if (array == NULL)
        return dup;
    memmove(( char * )array, ( char * )vec->array, vec->capacity * elem_size);
    dup.capacity    = vec->capacity;
    dup.size        = vec->size;
    dup.end_ptr     = vec->end_ptr;
    dup.load_factor = vec->load_factor;
    dup.array       = array;
    return dup;
}

int vector_empty(vector_t *vec, size_t elem_size)
{
    if (!vec || !vec->array || vec->size == 0)
        return ERR_IARGS;
    memset(( char * )vec->array, 0, vec->capacity * elem_size);
    vec->size    = 0;
    vec->end_ptr = 0;
    return ERR_OKAY;
}

vector_t vector_concat(vector_t *vec1, vector_t *vec2, size_t elem_size)
{
    vector_t new = {0};

    if (!vec1 || !vec1->array || !vec2 || !vec2->array)
        return new;

    void *array;
    array = calloc(vec1->capacity + vec2->capacity, elem_size);
    if (array == NULL)
        return new;
    memmove(( char * )array, ( char * )vec1->array, vec1->capacity * elem_size);
    memmove(( char * )array + (vec1->capacity * elem_size),
            ( char * )vec2->array, vec2->capacity * elem_size);

    new.size        = vec1->size + vec2->size;
    new.capacity    = vec1->capacity + vec2->capacity;
    new.end_ptr     = vec1->capacity + vec2->end_ptr - 1;
    new.load_factor = vec1->load_factor > vec2->load_factor ? vec2->load_factor
                                                            : vec1->load_factor;
    new.array       = array;

    return new;
}

int vector_resize(vector_t *vec, size_t new_capacity, size_t elem_size)
{
    if (!vec || !vec->array)
        return ERR_IARGS;

    void *new_array;
    new_array = calloc(new_capacity, elem_size);
    if (new_array == NULL)
        return ERR_ALLOC;

    int x   = new_capacity;
    int y   = vec->capacity;
    int len = x > y ? y : x;
    memmove(new_array, ( char * )vec->array, len * elem_size);
    free(vec->array);

    if (new_capacity < vec->size) {
        vec->size    = new_capacity;
        vec->end_ptr = new_capacity;
    }

    vec->array    = new_array;
    vec->capacity = new_capacity;

    return ERR_OKAY;
}

int vector_free(vector_t *vec)
{
    if (!vec || !vec->array)
        return ERR_IARGS;
    free(vec->array);
    memset(vec, 0, sizeof(vector_t));
    return ERR_OKAY;
}

////////////////////////////////////////////////////////////////////////////////
//                             HashMap Methods                                //
////////////////////////////////////////////////////////////////////////////////

static inline void *_map_entry_value(map_entry_t *entry)
{
    // return entry + (2 * sizeof(size_t)) + NORM_MAP_MAX_KEY_LEN;
    return entry->kv + NORM_MAP_MAX_KEY_LEN;
}

static inline size_t _map_entry_size(size_t elem_size)
{
    return sizeof(map_entry_t) + NORM_MAP_MAX_KEY_LEN + elem_size;
}

map_t map_init(size_t min_capacity, double load_factor, size_t elem_size,
               map_hasher_fn hasher_fn, map_probe_fn probe_fn)
{
    map_t map = {0};
    if (load_factor <= 0 || load_factor > 1 || min_capacity <= 0 ||
        elem_size <= 0 || hasher_fn == NULL)
        return map;
    uint32_t cap2 = min_capacity;
    --cap2;
    cap2 |= cap2 >> 1;
    cap2 |= cap2 >> 2;
    cap2 |= cap2 >> 4;
    cap2 |= cap2 >> 8;
    cap2 |= cap2 >> 16;
    ++cap2;
    vector_t vec = {0};
    vec          = vector_init(cap2, load_factor, _map_entry_size(elem_size));
    if (vec.capacity != cap2)
        return map;
    map.table     = vec;
    map.hasher_fn = hasher_fn;
    map.probe_fn  = probe_fn;
    return map;
}

int map_free(map_t *map)
{
    if (!map)
        return ERR_IARGS;
    int res = ERR_OKAY;
    res     = vector_free(&(map->table));
    if (res != ERR_OKAY)
        return res;
    memset(map, 0, sizeof(map_t));
    return ERR_OKAY;
}

int map_set(map_t *map, const char *key, void *elem, size_t key_length,
            size_t elem_size)
{
    if (!map || key_length <= 0 || key_length > NORM_MAP_MAX_KEY_LEN ||
        elem_size <= 0)
        return ERR_IARGS;
    int    res = ERR_OKAY;
    size_t index, entry_size, start_table_cap;
    start_table_cap = map->table.capacity;
    entry_size      = _map_entry_size(elem_size);
    index = map->probe_fn(map->hasher_fn, ( void * )(map->table.array), key,
                          key_length, entry_size, map->table.capacity);
    if (index == ~(( size_t )0))
        return ERR_IIDX;
    map_entry_t *entry =
            calloc(1, sizeof(map_entry_t) + NORM_MAP_MAX_KEY_LEN + elem_size);
    if (entry == NULL)
        return ERR_ALLOC;
    entry->key_length = key_length;
    entry->elem_size  = elem_size;
    memmove(entry->kv, key, key_length);
    memmove((( char * )(entry->kv)) + NORM_MAP_MAX_KEY_LEN, elem, elem_size);
    res = vector_spos(&(map->table), index, entry, entry_size);
    free(entry);
    if (res != ERR_OKAY) {
        return res;
    }
    if (map->table.capacity != start_table_cap)
        return map_rehash(map, elem_size);
    return ERR_OKAY;
}

void *map_get(map_t *map, const char *key, size_t key_length, size_t elem_size)
{
    size_t index, entry_size;
    entry_size = _map_entry_size(elem_size);
    index = map->probe_fn(map->hasher_fn, map->table.array, key, key_length,
                          entry_size, map->table.capacity);
    if (index == ~(( size_t )0))
        return NULL;
    map_entry_t *entry = vector_gpos(&(map->table), index, entry_size);
    if (entry == NULL)
        return NULL;
    if (entry->key_length != key_length ||
        memcmp(( char * )(entry->kv), key, key_length) != 0)
        return NULL;
    return ( void * )((( char * )(entry->kv)) + NORM_MAP_MAX_KEY_LEN);
}

int map_delete(map_t *map, const char *key, size_t key_length, size_t elem_size)
{
    size_t index, entry_size;
    entry_size = _map_entry_size(elem_size);
    index = map->probe_fn(map->hasher_fn, map->table.array, key, key_length,
                          entry_size, map->table.capacity);
    if (index == ~(( size_t )0))
        return ERR_IIDX;
    return vector_zpos(&(map->table), index, entry_size);
}

int map_clear(map_t *map, size_t elem_size)
{
    return vector_empty(&(map->table), _map_entry_size(elem_size));
}

int map_rehash(map_t *map, size_t elem_size)
{
    size_t entry_size;
    entry_size    = _map_entry_size(elem_size);
    char *new_arr = calloc(map->table.capacity, entry_size);
    if (new_arr == NULL)
        return ERR_ALLOC;
    size_t index;
    size_t end_ptr = 0;
    for (size_t i = 0; i < map->table.capacity; ++i) {
        map_entry_t *entry;
        entry = vector_gpos(&(map->table), i, entry_size);
        if (entry == NULL || memvcmp(entry, ( unsigned char )0, entry_size))
            continue;
        index = map->probe_fn(map->hasher_fn, new_arr, ( char * )entry->kv,
                              entry->key_length, entry_size,
                              map->table.capacity);
        if (index == ~(( size_t )0))
            return ERR_IIDX;
        if (index > end_ptr)
            end_ptr = index;
        memmove(new_arr + (index * entry_size), entry, entry_size);
    }
    free(map->table.array);
    map->table.array   = new_arr;
    map->table.end_ptr = end_ptr + 1;
    return ERR_OKAY;
}

// TODO: Implement a hasher to be used in the default hasher_fn
//       Implement either SipHash 2-4 or xxHash from scratch (64 bit variant)

size_t map_linear_probe(map_hasher_fn hasher_fn, void *table, const char *key,
                        size_t key_length, size_t elem_size,
                        size_t table_length)
{
    size_t index;
    index             = hasher_fn(key, key_length);
    index             = index & (table_length - 1);
    map_entry_t *curr = calloc(1, elem_size);
    if (curr == NULL)
        return ~(( size_t )0);
    memmove(( char * )curr, (( char * )(table)) + (index * elem_size),
            elem_size);
    while (!memvcmp(( char * )curr, ( unsigned char )0, elem_size)) {
        if (memcmp(curr->kv, key, key_length) == 0) {
            break;
        }
        index = (index + 1) & (table_length - 1);
        memset(( char * )curr, 0, elem_size);
        memmove(( char * )curr, (( char * )(table)) + (index * elem_size),
                elem_size);
    }
    free(curr);
    return index;
}

////////////////////////////////////////////////////////////////////////////////
//                           Allocator Methods                                //
////////////////////////////////////////////////////////////////////////////////

static inline size_t _mem_align_to_pagesize(size_t unaligned)
{
    size_t pagesize = getpagesize();
    return ((pagesize - 1) & unaligned)
                   ? ((unaligned + pagesize) & ~(pagesize - 1))
                   : unaligned;
}

static inline size_t _mem_pointer_align(size_t unaligned)
{
    size_t rem  = unaligned & (sizeof(void *) - 1);
    size_t diff = rem > 0 ? sizeof(void *) - rem : 0;
    return (diff + unaligned);
}

int mem_init(mem_ctx_t *ctx, size_t size)
{
    if (!ctx)
        return ERR_NO_CTX;

    void  *memory;
    size_t width;
    width  = size == 0 ? _mem_align_to_pagesize(MAX_CAPACITY)
                       : _mem_align_to_pagesize(size);

    memory = mmap(NULL, width, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON,
                  -1, 0);
    if (memory == MAP_FAILED)
        return ERR_MMAP;

    mem_header_t free;
    free.status       = FREE_FLAG | ZEROED_FLAG;
    free.data_size    = width - sizeof(mem_header_t);
    free.padding_size = 0;
    free.next_free    = NULL;
    free.memory = ( void * )(( unsigned char * )memory + sizeof(mem_header_t));
    memmove((( char * )(memory)), &free, sizeof(mem_header_t));

    ctx->capacity = width;
    ctx->used     = sizeof(mem_header_t);
    ctx->free     = ( void * )memory;
    ctx->memory   = ( void * )memory;

    return ERR_OKAY;
}

void *mem_alloc(mem_ctx_t *ctx, size_t size, uint32_t op_flags)
{
    if (!ctx)
        return NULL;

    size_t padded_length;
    padded_length = _mem_pointer_align(size);

    mem_header_t *prev, *free;
    prev = ( mem_header_t * )( void * )ctx->free;
    free = ( mem_header_t * )( void * )ctx->free;
    printf("s = %ld\tfds = %ld\n", size, free->data_size);
    while (free && free->data_size < padded_length) {
        if (!free->next_free)
            return NULL;
        printf("(free) probe: %ld\n", free->data_size);
        prev = free;
        free = ( mem_header_t * )( void * )free->next_free;
    }
    if (!free)
        return NULL;

    printf("(width): %ld    (ds): %zu    (ptr):  0x%-lx\n", padded_length,
           free->data_size, ( uintptr_t )( void * )free);

    mem_header_t *next_free_header = NULL;
    if (free->data_size < (padded_length + sizeof(mem_header_t))) {
        next_free_header  = ( mem_header_t * )( void * )free->next_free;
        prev->next_free   = free->next_free;
        ctx->used        -= sizeof(mem_header_t);
    } else {
        next_free_header =
                ( mem_header_t * )( void * )(( unsigned char * )free->memory +
                                             padded_length);

        next_free_header->data_size =
                free->data_size - (padded_length + sizeof(mem_header_t));
        next_free_header->status = FREE_FLAG | (free->status & ZEROED_FLAG);

        next_free_header->memory =
                free->memory + padded_length + sizeof(mem_header_t);

        next_free_header->next_free = free->next_free;
        prev->next_free =
                ( void * )(( unsigned char * )(free->memory) + padded_length);
        ctx->used += sizeof(mem_header_t);
    }
    if (( void * )free == ctx->free)
        ctx->free = ( void * )next_free_header;
    printf("next -> 0x%-px\n", ( void * )next_free_header);

    ctx->used                    += padded_length;

    mem_header_t *new_mem_header  = free;
    size_t        padding_width;
    padding_width                = padded_length - size;
    new_mem_header->data_size    = size;
    new_mem_header->padding_size = padding_width;
    new_mem_header->next_free    = ( void * )NULL;
    new_mem_header->status       = (free->status & ZEROED_FLAG) | ALLOCED_FLAG;

    if (op_flags & ZERO_REGION_OP) {
        new_mem_header->status |= ZEROED_FLAG;
        memset(( unsigned char * )( void * )(new_mem_header->memory), 0,
               new_mem_header->data_size + new_mem_header->padding_size);
    }

    return ( void * )new_mem_header->memory;
}

int mem_free(mem_ctx_t *ctx, void *memory, uint32_t op_flags)
{
    mem_header_t *header =
            ( mem_header_t
                      * )( void * )( unsigned char
                                             * )(( unsigned char * )(memory) -
                                                 sizeof(mem_header_t));
    header->status       ^= FREE_FLAG | ALLOCED_FLAG;
    header->data_size    += header->padding_size;
    header->padding_size  = 0;
    ctx->used            -= header->data_size;
    printf("(free) 0x%-lx\n", ( uintptr_t )( void * )header);
    if (!ctx->free)
        ctx->free = ( void * )header;
    else if (ctx->free > ( void * )header) {
        header->next_free = ctx->free;
        ctx->free         = ( void * )header;
    } else {
        mem_header_t *prev = ( mem_header_t * )( void * )ctx->free;
        while (prev && prev->next_free < ( void * )header) {
            if (( void * )prev->next_free == NULL)
                break;
            if (prev->next_free < ( void * )header)
                prev = ( mem_header_t * )( void * )prev->next_free;
            else
                break;
        }
        header->next_free = prev->next_free;
        prev->next_free   = ( void * )header;

        if (( void * )((( unsigned char * )( void * )(prev->memory)) +
                       prev->data_size) == ( void * )header) {
            prev->data_size += header->data_size + sizeof(mem_header_t);
            printf("(free merge) new size: %-zu\n", header->data_size);
            prev->next_free  = header->next_free;
            ctx->used       -= sizeof(mem_header_t);
            header           = prev;
        }
    }

    if (( void * )((( unsigned char * )( void * )(header->memory)) +
                   header->data_size) == header->next_free) {
        mem_header_t *neighbour = ( mem_header_t * )( void * )header->next_free;
        header->data_size += neighbour->data_size + sizeof(mem_header_t);
        printf("(free merge) new size: %-zu\n", header->data_size);
        header->next_free  = neighbour->next_free;
        ctx->used         -= sizeof(mem_header_t);
    }

    if (op_flags & ZERO_REGION_OP) {
        header->status |= ZEROED_FLAG;
        memset(( unsigned char * )( void * )(header->memory), 0,
               header->data_size);
    }

    printf("(freed next) %-px\n", header->next_free);

    return ERR_OKAY;
}

int mem_deinit(mem_ctx_t *ctx)
{
    munmap(( void * )ctx->memory, ctx->capacity);
    memset(ctx, 0, sizeof(mem_ctx_t));
    return ERR_OKAY;
}

// vim: ft=c ts=4 sts=4 sw=4 cin et nospell
