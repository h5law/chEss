/* board.c
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

#include <raylib.h>

#include "dsa.h"
#include "board.h"
#include "moves.h"

extern struct square_t *squares;
extern struct piece_s  *floating_piece;
extern struct square_t *original_sqr;

struct square_t *get_square(struct square_t *board, int x, int y)
{
    int              t_row = ( int )(y / 70);
    int              t_col = ( int )(x / 70);
    struct square_t *t_sq  = &board[t_row * 8 + t_col];
    return t_sq;
}

uint64_t checksum_board(struct square_t *board)
{
    uint64_t tracker = 0;
    for (int i = 0; i < 64; ++i) {
        if ((board[i].piece.role & PIECE_MASK) < 7)
            tracker |= (1ULL << i);
    }

    return tracker;
}

int move_piece(struct square_t *orig, struct square_t *nsq)
{
    int rc = 0;
    fprintf(stderr, "move_piece(): moving piece at %dx%d\n", orig->row,
            orig->col);
    if (((nsq->piece.role & PIECE_MASK) < 7 &&
         PIECE_COLOUR(orig->piece.role) == PIECE_COLOUR(nsq->piece.role)) &&
        (SQUARE_AVAILABLE(nsq, move_mask(squares, orig->piece.role)))) {
        fprintf(stderr, "move_piece(): moving back to initial square\n");
        floating_piece      = &orig->piece;
        original_sqr->piece = *floating_piece;
        rc                  = 1;
    } else if (SQUARE_AVAILABLE(nsq, move_mask(squares, orig->piece.role))) {
        fprintf(stderr, "move_piece(): moving the held piece currently\n");
        if (floating_piece && floating_piece->state > MOVED) {
            destroy_piece(NULL, &nsq->piece);
        } else {
            nsq->piece      = orig->piece;
            nsq->piece.role = (nsq->row << 12) | (nsq->col << 8) |
                              (0x00FF & nsq->piece.role);
            if (floating_piece && floating_piece->state == UNMOVED) {
                floating_piece->state = MOVED;
            }
        }
        rc = 0;
        fprintf(stderr, "move_piece(): removing piece from previous spot\n");
        floating_piece = NULL;
        destroy_piece(NULL, &orig->piece);
    }
    floating_piece = NULL;
    destroy_piece(NULL, floating_piece);
    return rc;
}

int destroy_piece(mem_ctx_t *ctx, struct piece_s *p)
{
    if (!p)
        return 1;
    if (!ctx)
        p->role = (p->role | PIECE_MASK) & 0x0007;
    else
        mem_free(ctx, p, ZERO_REGION_OP);
    return 0;
}

int destroy_board(mem_ctx_t *ctx, struct square_t *board)
{
    if (!board || !ctx)
        return 1;
    mem_free(ctx, board, ZERO_REGION_OP);
    mem_deinit(ctx);
    return 0;
}
