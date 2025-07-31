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

#include "../types.h"

/* Castling bits binary protocol
 * 0001 - White King can Castle Kingside
 * 0010 - White King can Castle Queenside
 * 0100 - Black King can Castle Kingside
 * 1000 - Black King can Castle Queenside
 * ----
 * 1111 - Both sides can castle both directions
 * 1001 - Black can castle queenside and white kingside
 */
enum { WKCK = 0x1, WKCQ = 0x2, BKCK = 0x4, BKCQ = 0x8 };

#define set_bit(bitboard, square) ((bitboard) |= (1ULL << (square)))
#define get_bit(bitboard, square) ((bitboard) & (1ULL << (square)))
#define pop_bit(bitboard, square) ((bitboard) &= ~(1ULL << (square)))

int get_time_ms(void);

void print_tiles(void);
void print_bitboard(u64 bitboard);
void print_board(int unicode);

u64 mask_pawn_attacks(int square, int side);
u64 mask_knight_attacks(int square);
u64 mask_king_attacks(int square);
u64 mask_bishop_attacks(int square);
u64 mask_rook_attacks(int square);
u64 generate_bishop_attacks(int square, u64 block);
u64 generate_rook_attacks(int square, u64 block);
u64 get_bishop_attacks(int square, u64 position);
u64 get_rook_attacks(int square, u64 position);

void init_all(void);
void init_slider_attacks(int piece);
void init_board(void);

int count_bits(u64 bitboard);
int get_lsb_index(u64 bitboard);
u64 set_positions(int idx, int mask_bit_count, u64 attack_mask);

u64 xorshift64(void);
u64 rand_u64(void);
u64 find_magic(int square, int m, int piece);

#endif /* BITBOARD_H */
