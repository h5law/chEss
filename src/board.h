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

struct game_state_t {
    unsigned int player : 2;
    unsigned int winner : 2;
    unsigned int check  : 2;

    struct square_t *board;

    uint32_t turn_count;
    uint32_t player1_count;
    uint32_t player2_count;
    uint32_t inactive_run;

    uint32_t round_start;
    uint32_t player1_remaining;
    uint32_t player2_remaining;
    char     log_messages[256];
};

struct player_state_t {
    unsigned int active : 2;
    unsigned int pieces : 2;

    uint8_t  score;
    uint16_t move_count;
    uint16_t inactivity_count;

    uint32_t start_time;
    uint32_t remaining_time;

    char user_banner[32];
};

int pair_players(struct player_state_t *players, size_t player_count,
                 struct player_state_t *p1, struct player_state_t *p2);
int init_game(struct game_state_t *gs, struct player_state_t *p1,
              struct player_state_t *p2);
int play_game(struct game_state_t *gs, struct player_state_t *p1,
              struct player_state_t *p2);
int end_game(struct game_state_t *gs, struct player_state_t *p1,
             struct player_state_t *p2, int outcome);

uint64_t checksum_board(struct square_t *board);
uint64_t check_check(struct square_t *board, uint8_t colour_id);

struct square_t *get_square(struct square_t *board, int x, int y);
int              move_piece(struct square_t *board, struct game_state_t *gs,
                            struct square_t *orig, struct square_t *nsq);

int destroy_piece(mem_ctx_t *ctx, struct piece_s *p);
int destroy_board(mem_ctx_t *ctx, struct square_t *board);

#endif /* BOARD_H */
