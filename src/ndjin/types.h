/* types.h
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

#ifndef TYPES_H
#define TYPES_H

typedef unsigned long long u64;

#define set_bit(bitboard, square) ((bitboard) |= (1ULL << (square)))
#define get_bit(bitboard, square) ((bitboard) & (1ULL << (square)))
#define pop_bit(bitboard, square) ((bitboard) &= ~(1ULL << (square)))

/* clang-format off */
enum {
    a1, b1, c1, d1, e1, f1, g1, h1,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a8, b8, c8, d8, e8, f8, g8, h8, no_sq
};
/* clang-format on */

enum { P, N, B, R, Q, K, p, n, b, r, q, k };
enum { white, black, both };
enum { no_check, white_check, black_check };
enum { bishop, rook };

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

struct square_moves_t {
    unsigned int moves[32];
    int          count;
};
struct move_list_t {
    struct square_moves_t squares[64];
    int                   count;
};

/* Move binary schema
 * 0000 0000 0000 0000 0011 1111 source
 * 0000 0000 0000 1111 1100 0000 target
 * 0000 0000 1111 0000 0000 0000 piece
 * 0000 1111 0000 0000 0000 0000 promoted piece
 * 0001 0000 0000 0000 0000 0000 capture flag
 * 0010 0000 0000 0000 0000 0000 double push flag
 * 0100 0000 0000 0000 0000 0000 enpassant flag
 * 1000 0000 0000 0000 0000 0000 castling flag
 */
#define MOVE_SOURCE             0x3F
#define MOVE_TARGET             0xFC0
#define MOVE_PIECE              0xF000
#define MOVE_PROMO              0xF0000
#define MOVE_CAPTURE            0x100000
#define MOVE_DOUBLE             0x200000
#define MOVE_PASSANT            0x400000
#define MOVE_CASTLE             0x800000

#define MOVE_CAPTURE_FLAG(move) (move & MOVE_CAPTURE)
#define MOVE_DOUBLE_FLAG(move)  (move & MOVE_DOUBLE)
#define MOVE_PASSANT_FLAG(move) (move & MOVE_PASSANT)
#define MOVE_CASTLE_FLAG(move)  (move & MOVE_CASTLE)

/* clang-format off */
#define ENCODE_MOVE(source, target, piece, promo, capture, dpush, passant,     \
                    castle)                                                    \
    (0U | (                                                                    \
        (source & 63) |                                                        \
        ((target & 63) << 6) |                                                 \
        ((piece & 15) << 12) |                                                 \
        ((promo & 15) << 16) |                                                 \
        (capture << 20) |                                                      \
        (dpush << 21) |                                                        \
        (passant << 22) |                                                      \
        (castle << 23)))

#define DECODE_MOVE(move, source, target, piece, promo)                        \
    do {                                                                       \
        *source   = ((move & MOVE_SOURCE) & 63);                               \
        *target   = (((move & MOVE_TARGET) >> 6) & 63);                        \
        *piece    = (((move & MOVE_PIECE) >> 12) & 15);                        \
        *promo    = (((move & MOVE_PROMO) >> 16) & 15);                        \
    } while(0)
/* clang-format on */

struct state_t {
    int          side;
    int          check;
    int          enpassant;
    int          castle;
    int          ply;
    int          fifty;
    int          fullmoves;
    u64          bitboards[12];
    u64          positions[3];
    unsigned int current_best_move;
};

enum { all_moves, only_captures };

#define BOARD_BACKUP(state1, state2)                                           \
    do {                                                                       \
        memcpy((( struct state_t * )state2)->bitboards,                        \
               (( struct state_t * )state1)->bitboards, 96);                   \
        memcpy((( struct state_t * )state2)->positions,                        \
               (( struct state_t * )state1)->positions, 24);                   \
        (( struct state_t * )state2)->side =                                   \
                (( struct state_t * )state1)->side;                            \
        (( struct state_t * )state2)->enpassant =                              \
                (( struct state_t * )state1)->enpassant;                       \
        (( struct state_t * )state2)->castle =                                 \
                (( struct state_t * )state1)->castle;                          \
        (( struct state_t * )state2)->ply = (( struct state_t * )state1)->ply; \
        (( struct state_t * )state2)->fifty =                                  \
                (( struct state_t * )state1)->fifty;                           \
        (( struct state_t * )state2)->fullmoves =                              \
                (( struct state_t * )state1)->fullmoves;                       \
    } while (0)

#define BOARD_RESTORE(state2, state1)                                          \
    do {                                                                       \
        memcpy((( struct state_t * )state1)->bitboards,                        \
               (( struct state_t * )state2)->bitboards, 96);                   \
        memcpy((( struct state_t * )state1)->positions,                        \
               (( struct state_t * )state2)->positions, 24);                   \
        (( struct state_t * )state1)->side =                                   \
                (( struct state_t * )state2)->side;                            \
        (( struct state_t * )state1)->enpassant =                              \
                (( struct state_t * )state2)->enpassant;                       \
        (( struct state_t * )state1)->castle =                                 \
                (( struct state_t * )state2)->castle;                          \
        (( struct state_t * )state1)->ply = (( struct state_t * )state2)->ply; \
        (( struct state_t * )state1)->fifty =                                  \
                (( struct state_t * )state2)->fifty;                           \
        (( struct state_t * )state1)->fullmoves =                              \
                (( struct state_t * )state2)->fullmoves;                       \
    } while (0)

#endif /* TYPES_H */

/* vim: ft=c ts=4 sts=4 sw=4 ai et cin */
