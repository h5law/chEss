/* dsa.h
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

#pragma once

#ifndef DSA_H
#define DSA_H

#define ERR_OKAY     0
#define ERR_IARGS    1
#define ERR_ALLOC    2
#define ERR_OFLOW    3
#define ERR_UFLOW    4
#define ERR_IIDX     5
#define ERR_NO_SPACE 6
#define ERR_NO_CTX   7
#define ERR_MMAP     8

#include <stdint.h>
#include <sys/types.h>

struct vector_t {
    // size: number of elements currently in the array
    // end_ptr: the index of the next empty with the greatest index
    // capacity: the size of the array in total, counting empty slots
    size_t size, end_ptr, capacity;
    // Percentage of the used capacity before a grow occurs
    // Example:
    //  load_factor = 0.8 -> grow at 80% used
    double load_factor;
    void  *array;
};
typedef struct vector_t vector_t;

void    *vector_gpos(vector_t *vec, size_t index, size_t elem_size);
int      vector_spos(vector_t *vec, size_t index, void *elem, size_t elem_size);
int      vector_zpos(vector_t *vec, size_t index, size_t elem_size);
int      vector_push(vector_t *vec, void *elem, size_t elem_size);
int      vector_compact(vector_t *vec, size_t elem_size);
int      vector_resize(vector_t *vec, size_t new_capacity, size_t elem_size);
int      vector_empty(vector_t *vec, size_t elem_size);
vector_t vector_init(size_t min_capacity, double load_factor, size_t elem_size);
int      vector_free(vector_t *vec);
vector_t vector_clone(vector_t *vec, size_t elem_size);
vector_t vector_concat(vector_t *vec1, vector_t *vec2, size_t elem_size);

#ifndef NORM_MAP_MAX_KEY_LEN
#define NORM_MAP_MAX_KEY_LEN 32
#endif

struct map_entry_t {
    size_t key_length;
    size_t elem_size;
    char   kv[];
};
typedef struct map_entry_t map_entry_t;

typedef size_t (*map_hasher_fn)(const char *key, size_t key_length);
typedef size_t (*map_probe_fn)(map_hasher_fn hasher, void *table,
                               const char *key, size_t key_length,
                               size_t elem_size, size_t table_length);

struct map_t {
    vector_t      table;
    map_hasher_fn hasher_fn;
    map_probe_fn  probe_fn;
};
typedef struct map_t map_t;

map_t map_init(size_t min_capacity, double load_factor, size_t elem_size,
               map_hasher_fn hasher_fn, map_probe_fn probe_fn);

int   map_free(map_t *map);
int   map_rehash(map_t *map, size_t elem_size);
void *map_get(map_t *map, const char *key, size_t key_length, size_t elem_size);
int   map_set(map_t *map, const char *key, void *elem, size_t key_length,
              size_t elem_size);
int   map_delete(map_t *map, const char *key, size_t key_length,
                 size_t elem_size);
int   map_clear(map_t *map, size_t elem_size);

size_t map_linear_probe(map_hasher_fn hasher, void *table, const char *key,
                        size_t key_length, size_t elem_size,
                        size_t table_length);

#ifndef MAX_CAPACITY
#define MAX_CAPACITY 655360
#endif

enum {
    ZEROED_FLAG  = 0x00000001,
    ALLOCED_FLAG = 0x00000002,
    FREE_FLAG    = 0x00000004,
    // MARKED_FLAG = 0x00000008,
};

enum {
    ZERO_REGION_OP = 0x00000001,
};

typedef struct mem_header_t mem_header_t;
struct mem_header_t {
    unsigned char status;
    size_t        data_size;
    size_t        padding_size;
    void         *next_free; // NULL unless status & FREE_FLAG > 0
    void         *memory;
};

typedef struct mem_ctx_t mem_ctx_t;
struct mem_ctx_t {
    size_t capacity;
    size_t used;
    void  *free;
    void  *memory;
};

int   mem_init(mem_ctx_t *ctx, size_t min_size);
void *mem_alloc(mem_ctx_t *ctx, size_t size_bytes, uint32_t op_flags);
int   mem_free(mem_ctx_t *ctx, void *memory, uint32_t op_flags);
int   mem_deinit(mem_ctx_t *ctx);

#endif /*  DSA_H */

// vim: ft=c ts=4 sts=4 sw=4 cin et nospell
