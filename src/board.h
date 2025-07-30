/* board.h
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

#ifndef BOARD_H
#define BOARD_H

#include <raylib.h>
#include <stdint.h>

#include "dsa.h"

#define PAWN_ID     6
#define ROOK_ID     5
#define KNIGHT_ID   4
#define BISHOP_ID   3
#define QUEEN_ID    2
#define KING_ID     1

#define WHITE_MASK  0x0020
#define WHITE_ID    0x02
#define BLACK_MASK  0x0010
#define BLACK_ID    0x01
#define COLOUR_MASK 0x0030
#define ROW_MASK    0xF000
#define COL_MASK    0x0F00
#define PIECE_MASK  0x00CF

#define PIECE_COLOUR(u16)                                                      \
    (((u16 & COLOUR_MASK) == WHITE_MASK)   ? 0x2                               \
     : ((u16 & COLOUR_MASK) == BLACK_MASK) ? 0x1                               \
                                           : 0x0)
#define PIECE_ROW(u16) (( uint16_t )u16 & ROW_MASK) >> 12
#define PIECE_COL(u16) (( uint16_t )u16 & COL_MASK) >> 8

#define PIECE_ROLE(u16)                                                        \
    (((u16 & PIECE_MASK) == PAWN_ID)     ? PAWN_ID                             \
     : ((u16 & PIECE_MASK) == KNIGHT_ID) ? KNIGHT_ID                           \
     : ((u16 & PIECE_MASK) == BISHOP_ID) ? BISHOP_ID                           \
     : ((u16 & PIECE_MASK) == ROOK_ID)   ? ROOK_ID                             \
     : ((u16 & PIECE_MASK) == QUEEN_ID)  ? QUEEN_ID                            \
     : ((u16 & PIECE_MASK) == KING_ID)   ? KING_ID                             \
                                         : 7)

#define PIECE_STRING(u16)                                                      \
    (((u16 & PIECE_MASK) == PAWN_ID)     ? "pawn"                              \
     : ((u16 & PIECE_MASK) == KNIGHT_ID) ? "knight"                            \
     : ((u16 & PIECE_MASK) == BISHOP_ID) ? "bishop"                            \
     : ((u16 & PIECE_MASK) == ROOK_ID)   ? "rook"                              \
     : ((u16 & PIECE_MASK) == QUEEN_ID)  ? "queen"                             \
     : ((u16 & PIECE_MASK) == KING_ID)   ? "king"                              \
                                         : "")

typedef uint16_t piece_t;

struct piece_s {
    piece_t role;
    enum {
        UNMOVED,
        MOVED,
        PINNED,
        TAKEN,
        CHECK,
        PROMOTED,
    } state;
};

struct square_t {
    struct piece_s piece;
    int            row;
    int            col;
    Color          colour;
};

uint64_t         checksum_board(struct square_t *board);
struct square_t *get_square(struct square_t *board, int x, int y);

int move_piece(struct square_t *orig, struct square_t *nsq);

int destroy_piece(mem_ctx_t *ctx, struct piece_s *p);
int destroy_board(mem_ctx_t *ctx, struct square_t *board);

#endif /* BOARD_H */
