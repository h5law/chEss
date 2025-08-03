/* bb.h
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

#ifndef BITBOARD_H
#define BITBOARD_H

#include "types.h"

static inline int get_time_ms(void);

static inline void print_tiles(void);
static inline void print_bitboard(u64 bitboard);
static inline void print_attacked(int side);
static inline void print_board(int unicode);
static inline void print_move(unsigned int move, int unicode);
static inline void print_move_list(struct move_list_t *moves, int unicode);

static inline u64 mask_pawn_attacks(int square, int side);
static inline u64 mask_knight_attacks(int square);
static inline u64 mask_king_attacks(int square);
static inline u64 mask_bishop_attacks(int square);
static inline u64 mask_rook_attacks(int square);
static inline u64 generate_bishop_attacks(int square, u64 block);
static inline u64 generate_rook_attacks(int square, u64 block);
static inline u64 get_bishop_attacks(int square, u64 position);
static inline u64 get_rook_attacks(int square, u64 position);
static inline u64 get_queen_attacks(int square, u64 position);

inline int  get_attacked(struct state_t *state, int square, int side);
int         make_move(struct state_t *state, unsigned int move, int move_flag);
inline void generate_moves(struct move_list_t *list);

void               init_all(void);
static inline void init_slider_attacks(int piece);
void               init_board(void);

static inline int count_bits(u64 bitboard);
static inline int get_lsb_index(u64 bitboard);
static inline u64 set_positions(int idx, int mask_bit_count, u64 attack_mask);

static inline u64 xorshift64(void);
static inline u64 rand_u64(void);
static inline u64 find_magic(int square, int m, int piece);

#endif /* BITBOARD_H */

/* vim: ft=c ts=4 sts=4 sw=4 ai et cin */
