/* moves.c
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

#include <stdint.h>
#include <stdio.h>

#include "board.h"
#include "moves.h"

uint64_t knight_move_map(uint8_t row, uint8_t col)
{
    uint64_t left_ph  = (( uint64_t )1 << (col - 2)) << (row * 8);
    uint64_t right_ph = (( uint64_t )1 << (col + 2)) << (row * 8);
    uint64_t td_ph    = (( uint64_t )5 << (col - 1)) << (row * 8);
    if (col == 0)
        td_ph = (( uint64_t )0b00000010) << (row * 8);
    if (col == 7)
        td_ph = (( uint64_t )0b01000000) << (row * 8);
    if (col < 2)
        left_ph = 0;
    if (col > 5)
        right_ph = 0;

    uint64_t td_map    = (td_ph << 16) | (td_ph >> 16);
    uint64_t right_map = (right_ph << 8) | (right_ph >> 8);
    uint64_t left_map  = (left_ph << 8) | (left_ph >> 8);

    return td_map | right_map | left_map;
}

uint64_t pawn_move_map(struct square_t *board, uint8_t row, uint8_t col)
{
    uint64_t map = 0;

    if (row > 7 || col > 7) {
        fprintf(stderr, "pawn_move_map: invalid position (%d, %d)\n", row, col);
        return 0;
    }

    uint8_t colour = (PIECE_COLOUR(board[(row * 8) + col].piece.role));

    if (colour == WHITE_ID) {
        if (row > 0 &&
            PIECE_COLOUR(board[(row - 1) * 8 + col].piece.role) == 0) {
            map |= (( uint64_t )1 << col) << ((row - 1) * 8);
        }
        if (row == 6 &&
            PIECE_COLOUR(board[(row - 1) * 8 + col].piece.role) == 0 &&
            PIECE_COLOUR(board[(row - 2) * 8 + col].piece.role) == 0) {
            map |= (( uint64_t )1 << col) << ((row - 2) * 8);
        }
        if (row > 0 && col > 0) {
            if (PIECE_COLOUR(board[(row - 1) * 8 + (col - 1)].piece.role) ==
                2 - colour + 1) {
                map |= (( uint64_t )1 << (col - 1)) << ((row - 1) * 8);
            }
        }
        if (row > 0 && col < 7) {
            if (PIECE_COLOUR(board[(row - 1) * 8 + (col + 1)].piece.role) ==
                2 - colour + 1) {
                map |= (( uint64_t )1 << (col + 1)) << ((row - 1) * 8);
            }
        }
        // TODO: En-Passant
    } else if (colour == BLACK_ID) {
        if (row < 7 &&
            PIECE_COLOUR(board[(row + 1) * 8 + col].piece.role) == 0) {
            map |= (( uint64_t )1 << col) << ((row + 1) * 8);
        }
        if (row == 1 &&
            PIECE_COLOUR(board[(row + 1) * 8 + col].piece.role) == 0 &&
            PIECE_COLOUR(board[(row + 2) * 8 + col].piece.role) == 0) {
            map |= (( uint64_t )1 << col) << ((row + 2) * 8);
        }
        if (row < 7 && col > 0) {
            if (PIECE_COLOUR(board[(row + 1) * 8 + (col - 1)].piece.role) ==
                2 - colour + 1) {
                map |= (( uint64_t )1 << (col - 1)) << ((row + 1) * 8);
            }
        }
        if (row < 7 && col < 7) {
            if (PIECE_COLOUR(board[(row + 1) * 8 + (col + 1)].piece.role) ==
                2 - colour + 1) {
                map |= (( uint64_t )1 << (col + 1)) << ((row + 1) * 8);
            }
        }
        // TODO: En-Passant
    }

    return map;
}

uint64_t bishop_move_map(struct square_t *board, uint8_t row, uint8_t col)
{
    uint64_t map          = 0;
    uint8_t  piece_colour = PIECE_COLOUR(board[(row * 8) + col].piece.role);
    int8_t   r, c, idx;

    r = row - 1;
    c = col + 1;
    while (r >= 0 && c <= 7) {
        idx  = r * 8 + c;
        map |= (1ULL << idx);
        if (PIECE_ROLE(board[idx].piece.role) != 7) {
            if ((piece_colour == BLACK_ID &&
                 PIECE_COLOUR(board[idx].piece.role) == WHITE_ID) ||
                (piece_colour == WHITE_ID &&
                 PIECE_COLOUR(board[idx].piece.role) == BLACK_ID)) {
                break;
            }
            break;
        }
        --r;
        ++c;
    }

    r = row - 1;
    c = col - 1;
    while (r >= 0 && c >= 0) {
        idx  = r * 8 + c;
        map |= (1ULL << idx);
        if (PIECE_ROLE(board[idx].piece.role) != 7) {
            if ((piece_colour == BLACK_ID &&
                 PIECE_COLOUR(board[idx].piece.role) == WHITE_ID) ||
                (piece_colour == WHITE_ID &&
                 PIECE_COLOUR(board[idx].piece.role) == BLACK_ID)) {
                break;
            }
            break;
        }
        --r;
        --c;
    }

    r = row + 1;
    c = col + 1;
    while (r <= 7 && c <= 7) {
        idx  = r * 8 + c;
        map |= (1ULL << idx);
        if (PIECE_ROLE(board[idx].piece.role) != 7) {
            if ((piece_colour == BLACK_ID &&
                 PIECE_COLOUR(board[idx].piece.role) == WHITE_ID) ||
                (piece_colour == WHITE_ID &&
                 PIECE_COLOUR(board[idx].piece.role) == BLACK_ID)) {
                break;
            }
            break;
        }
        ++r;
        ++c;
    }

    r = row + 1;
    c = col - 1;
    while (r <= 7 && c >= 0) {
        idx  = r * 8 + c;
        map |= (1ULL << idx);
        if (PIECE_ROLE(board[idx].piece.role) != 7) {
            if ((piece_colour == BLACK_ID &&
                 PIECE_COLOUR(board[idx].piece.role) == WHITE_ID) ||
                (piece_colour == WHITE_ID &&
                 PIECE_COLOUR(board[idx].piece.role) == BLACK_ID)) {
                break;
            }
            break;
        }
        ++r;
        --c;
    }

    return map;
}

uint64_t rook_move_map(struct square_t *board, uint8_t row, uint8_t col)
{
    uint64_t map          = 0;
    uint8_t  piece_colour = PIECE_COLOUR(board[(row * 8) + col].piece.role);
    int8_t   r, c, idx;

    r = row - 1;
    c = col;
    while (r >= 0 && c <= 7) {
        idx = r * 8 + c;
        if (idx > 63)
            break;
        map |= (1ULL << idx);
        if (PIECE_ROLE(board[idx].piece.role) != 7) {
            if ((piece_colour == BLACK_ID &&
                 PIECE_COLOUR(board[idx].piece.role) == WHITE_ID) ||
                (piece_colour == WHITE_ID &&
                 PIECE_COLOUR(board[idx].piece.role) == BLACK_ID)) {
                break;
            }
            break;
        }
        --r;
    }

    r = row + 1;
    c = col;
    while (r >= 0 && c >= 0) {
        idx = r * 8 + c;
        if (idx > 63)
            break;
        map |= (1ULL << idx);
        if (PIECE_ROLE(board[idx].piece.role) != 7) {
            if ((piece_colour == BLACK_ID &&
                 PIECE_COLOUR(board[idx].piece.role) == WHITE_ID) ||
                (piece_colour == WHITE_ID &&
                 PIECE_COLOUR(board[idx].piece.role) == BLACK_ID)) {
                break;
            }
            break;
        }
        ++r;
    }

    r = row;
    c = col + 1;
    while (r <= 7 && c <= 7) {
        idx  = r * 8 + c;
        map |= (1ULL << idx);
        if (PIECE_ROLE(board[idx].piece.role) != 7) {
            if ((piece_colour == BLACK_ID &&
                 PIECE_COLOUR(board[idx].piece.role) == WHITE_ID) ||
                (piece_colour == WHITE_ID &&
                 PIECE_COLOUR(board[idx].piece.role) == BLACK_ID)) {
                break;
            }
            break;
        }
        ++c;
    }

    r = row;
    c = col - 1;
    while (r <= 7 && c >= 0) {
        idx  = r * 8 + c;
        map |= (1ULL << idx);
        if (PIECE_ROLE(board[idx].piece.role) != 7) {
            if ((piece_colour == BLACK_ID &&
                 PIECE_COLOUR(board[idx].piece.role) == WHITE_ID) ||
                (piece_colour == WHITE_ID &&
                 PIECE_COLOUR(board[idx].piece.role) == BLACK_ID)) {
                break;
            }
            break;
        }
        --c;
    }

    return map;
}

uint64_t queen_move_map(struct square_t *board, uint8_t row, uint8_t col)
{
    uint64_t map          = 0;
    uint8_t  piece_colour = PIECE_COLOUR(board[(row * 8) + col].piece.role);
    int8_t   r, c, idx;

    r = row - 1;
    c = col + 1;
    while (r >= 0 && c <= 7) {
        idx  = r * 8 + c;
        map |= (1ULL << idx);
        if (PIECE_ROLE(board[idx].piece.role) != 7) {
            if ((piece_colour == BLACK_ID &&
                 PIECE_COLOUR(board[idx].piece.role) == WHITE_ID) ||
                (piece_colour == WHITE_ID &&
                 PIECE_COLOUR(board[idx].piece.role) == BLACK_ID)) {
                break;
            }
            break;
        }
        --r;
        ++c;
    }

    r = row - 1;
    c = col - 1;
    while (r >= 0 && c >= 0) {
        idx = r * 8 + c;
        if (idx > 63)
            break;
        map |= (1ULL << idx);
        if (PIECE_ROLE(board[idx].piece.role) != 7) {
            if ((piece_colour == BLACK_ID &&
                 PIECE_COLOUR(board[idx].piece.role) == WHITE_ID) ||
                (piece_colour == WHITE_ID &&
                 PIECE_COLOUR(board[idx].piece.role) == BLACK_ID)) {
                break;
            }
            break;
        }
        --r;
        --c;
    }

    r = row + 1;
    c = col + 1;
    while (r <= 7 && c <= 7) {
        idx  = r * 8 + c;
        map |= (1ULL << idx);
        if (PIECE_ROLE(board[idx].piece.role) != 7) {
            if ((piece_colour == BLACK_ID &&
                 PIECE_COLOUR(board[idx].piece.role) == WHITE_ID) ||
                (piece_colour == WHITE_ID &&
                 PIECE_COLOUR(board[idx].piece.role) == BLACK_ID)) {
                break;
            }
            break;
        }
        ++r;
        ++c;
    }

    r = row + 1;
    c = col - 1;
    while (r <= 7 && c >= 0) {
        idx = r * 8 + c;
        if (idx > 63)
            break;
        map |= (1ULL << idx);
        if (PIECE_ROLE(board[idx].piece.role) != 7) {
            if ((piece_colour == BLACK_ID &&
                 PIECE_COLOUR(board[idx].piece.role) == WHITE_ID) ||
                (piece_colour == WHITE_ID &&
                 PIECE_COLOUR(board[idx].piece.role) == BLACK_ID)) {
                break;
            }
            break;
        }
        ++r;
        --c;
    }

    r = row - 1;
    c = col;
    while (r >= 0 && c <= 7) {
        idx = r * 8 + c;
        if (idx > 63)
            break;
        map |= (1ULL << idx);
        if (PIECE_ROLE(board[idx].piece.role) != 7) {
            if ((piece_colour == BLACK_ID &&
                 PIECE_COLOUR(board[idx].piece.role) == WHITE_ID) ||
                (piece_colour == WHITE_ID &&
                 PIECE_COLOUR(board[idx].piece.role) == BLACK_ID)) {
                break;
            }
            break;
        }
        --r;
    }

    r = row + 1;
    c = col;
    while (r >= 0 && c >= 0) {
        idx = r * 8 + c;
        if (idx > 63)
            break;
        map |= (1ULL << idx);
        if (PIECE_ROLE(board[idx].piece.role) != 7) {
            if ((piece_colour == BLACK_ID &&
                 PIECE_COLOUR(board[idx].piece.role) == WHITE_ID) ||
                (piece_colour == WHITE_ID &&
                 PIECE_COLOUR(board[idx].piece.role) == BLACK_ID)) {
                break;
            }
            break;
        }
        ++r;
    }

    r = row;
    c = col + 1;
    while (r <= 7 && c <= 7) {
        idx  = r * 8 + c;
        map |= (1ULL << idx);
        if (PIECE_ROLE(board[idx].piece.role) != 7) {
            if ((piece_colour == BLACK_ID &&
                 PIECE_COLOUR(board[idx].piece.role) == WHITE_ID) ||
                (piece_colour == WHITE_ID &&
                 PIECE_COLOUR(board[idx].piece.role) == BLACK_ID)) {
                break;
            }
            break;
        }
        ++c;
    }

    r = row;
    c = col - 1;
    while (r <= 7 && c >= 0) {
        idx = r * 8 + c;
        if (idx > 63)
            break;
        map |= (1ULL << idx);
        if (PIECE_ROLE(board[idx].piece.role) != 7) {
            if ((piece_colour == BLACK_ID &&
                 PIECE_COLOUR(board[idx].piece.role) == WHITE_ID) ||
                (piece_colour == WHITE_ID &&
                 PIECE_COLOUR(board[idx].piece.role) == BLACK_ID)) {
                break;
            }
            break;
        }
        --c;
    }

    return map;
}

uint64_t king_move_map(struct square_t *board, uint8_t row, uint8_t col)
{
    uint64_t map          = 0;
    uint8_t  piece_colour = PIECE_COLOUR(board[(row * 8) + col].piece.role);

    uint64_t top          = (( uint64_t )7 << (col - 1)) << ((row - 1) * 8);
    uint64_t bottom       = (( uint64_t )7 >> (col - 1)) << (row + 1 * 8);
    if (col < 1) {
        top    = (( uint64_t )3 << (col)) << ((row - 1) * 8);
        bottom = (( uint64_t )3 << (col)) << ((row + 1) * 8);
    }
    if (row == 0)
        top = 0;
    if (row == 7)
        bottom = 0;
    uint64_t lr = (( uint64_t )5 << (col - 1)) << (row * 8);
    if (col == 0)
        lr = (( uint64_t )0b00000010) << (row * 8);
    if (col == 7)
        lr = (( uint64_t )0b01000000) << (row * 8);

    map = top | bottom | lr;

    return map;
}

uint64_t move_mask(struct square_t *board, uint16_t piece)
{
    switch (PIECE_ROLE(piece)) {
    case PAWN_ID:
        return pawn_move_map(board, PIECE_ROW(piece), PIECE_COL(piece));
    case KNIGHT_ID:
        return knight_move_map(PIECE_ROW(piece), PIECE_COL(piece));
    case BISHOP_ID:
        return bishop_move_map(board, PIECE_ROW(piece), PIECE_COL(piece));
    case ROOK_ID:
        return rook_move_map(board, PIECE_ROW(piece), PIECE_COL(piece));
    case QUEEN_ID:
        return queen_move_map(board, PIECE_ROW(piece), PIECE_COL(piece));
    case KING_ID:
        return king_move_map(board, PIECE_ROW(piece), PIECE_COL(piece));
    default:
        return 0;
    }
}
