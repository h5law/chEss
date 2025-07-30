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

uint64_t check_check(struct square_t *board, uint8_t colour_id)
{
    uint64_t king       = 0;
    uint64_t king_idx   = 0;
    uint64_t mask       = 0;
    uint64_t rev_mask   = 0;
    uint64_t check_mask = 0;

    for (int i = 0; i < 64; ++i) {
        if (colour_id == WHITE_ID) {
            if ((0x00FF & board[i].piece.role) == (WHITE_MASK | KING_ID)) {
                king     = (1 << i);
                king_idx = i;
                break;
            }
        } else if (colour_id == BLACK_ID) {
            if ((0x00FF & board[i].piece.role) == (BLACK_MASK | KING_ID)) {
                king     = (1 << i);
                king_idx = i;
                break;
            }
        }
    }

    for (int i = 0; i < 64; ++i) {
        if (PIECE_COLOUR(board[i].piece.role) == colour_id)
            continue;
        switch (PIECE_ROLE(board[i].piece.role)) {
        // case PAWN_ID:
        //     rev_mask = pawn_move_map(board, PIECE_ROW(board[i].piece.role),
        //                              PIECE_COL(board[i].piece.role));
        //     break;
        case KNIGHT_ID:
            rev_mask = knight_move_map(PIECE_ROW(board[i].piece.role),
                                       PIECE_COL(board[i].piece.role));
            break;
        case BISHOP_ID:
            rev_mask = bishop_move_map(board, PIECE_ROW(board[i].piece.role),
                                       PIECE_COL(board[i].piece.role));
            break;
        case ROOK_ID:
            rev_mask = rook_move_map(board, PIECE_ROW(board[i].piece.role),
                                     PIECE_COL(board[i].piece.role));
            break;
        case QUEEN_ID:
            rev_mask = queen_move_map(board, PIECE_ROW(board[i].piece.role),
                                      PIECE_COL(board[i].piece.role));
            break;
        case KING_ID:
            rev_mask = king_move_map(board, PIECE_ROW(board[i].piece.role),
                                     PIECE_COL(board[i].piece.role));
            break;
        }
        if ((rev_mask & king) > 0)
            return (rev_mask & king) | (1 << i);
    }

    return 0;
}

int resolves_check(struct square_t *board, struct square_t *orig,
                   struct square_t *nsq, uint8_t colour_id)
{
    struct piece_s orig_piece = orig->piece;
    struct piece_s nsq_piece  = nsq->piece;

    nsq->piece                = orig->piece;
    nsq->piece.role =
            (nsq->row << 12) | (nsq->col << 8) | (0x00FF & nsq->piece.role);
    destroy_piece(NULL, &orig->piece);

    uint64_t check_mask = check_check(board, colour_id);

    orig->piece         = orig_piece;
    nsq->piece          = nsq_piece;

    return check_mask == 0;
}

int check_discovered_check(struct square_t *board, struct game_state_t *gs,
                           struct square_t *orig, struct square_t *nsq)
{
    uint64_t king       = 0;
    uint64_t king_idx   = 0;
    uint64_t mask       = 0;
    uint64_t rev_mask   = 0;
    uint64_t check_mask = 0;
    uint8_t  opp_colour = 2 - gs->player + 1;

    for (int i = 0; i < 64; ++i) {
        if (opp_colour == WHITE_ID) {
            if ((0x00FF & board[i].piece.role) == (WHITE_MASK | KING_ID)) {
                king     = (1ULL << i);
                king_idx = i;
                break;
            }
        } else if (opp_colour == BLACK_ID) {
            if ((0x00FF & board[i].piece.role) == (BLACK_MASK | KING_ID)) {
                king     = (1ULL << i);
                king_idx = i;
                break;
            }
        }
    }

    struct piece_s orig_piece = orig->piece;
    struct piece_s nsq_piece  = nsq->piece;
    nsq->piece                = orig->piece;
    nsq->piece.role =
            (nsq->row << 12) | (nsq->col << 8) | (0x00FF & nsq->piece.role);
    orig->piece.role = (orig->piece.role | PIECE_MASK) & 0x0007;

    for (int i = 0; i < 64; ++i) {
        if (i == (nsq->row * 8 + nsq->col) ||
            PIECE_COLOUR(board[i].piece.role) != gs->player)
            continue;
        switch (PIECE_ROLE(board[i].piece.role)) {
        case PAWN_ID:
            rev_mask = pawn_move_map(board, PIECE_ROW(board[i].piece.role),
                                     PIECE_COL(board[i].piece.role));
            break;
        case KNIGHT_ID:
            rev_mask = knight_move_map(PIECE_ROW(board[i].piece.role),
                                       PIECE_COL(board[i].piece.role));
            break;
        case BISHOP_ID:
            rev_mask = bishop_move_map(board, PIECE_ROW(board[i].piece.role),
                                       PIECE_COL(board[i].piece.role));
            break;
        case ROOK_ID:
            rev_mask = rook_move_map(board, PIECE_ROW(board[i].piece.role),
                                     PIECE_COL(board[i].piece.role));
            break;
        case QUEEN_ID:
            rev_mask = queen_move_map(board, PIECE_ROW(board[i].piece.role),
                                      PIECE_COL(board[i].piece.role));
            break;
        case KING_ID:
            rev_mask = king_move_map(board, PIECE_ROW(board[i].piece.role),
                                     PIECE_COL(board[i].piece.role));
            break;
        }
        if ((rev_mask & king) > 0) {
            nsq->piece      = nsq_piece;
            nsq->piece.role = (PIECE_ROW(nsq_piece.role) << 12) |
                              (PIECE_COL(nsq_piece.role) << 8) |
                              (0x00FF & nsq_piece.role);
            orig->piece      = orig_piece;
            orig->piece.role = (PIECE_ROW(orig_piece.role) << 12) |
                               (PIECE_COL(orig_piece.role) << 8) |
                               (0x00FF & orig_piece.role);
            return 1;
        }
    }

    return 0;
}

int move_piece(struct square_t *board, struct game_state_t *gs,
               struct square_t *orig, struct square_t *nsq)
{
    fprintf(stderr,
            "move_piece(): attempting to move piece at %dx%d to %dx%d\n",
            orig->row, orig->col, nsq->row, nsq->col);

    int rc      = 0;

    uint64_t cc = check_check(squares, gs->player);
    gs->check   = cc ? gs->player : 0;

    if (nsq == orig ||
        (((nsq->piece.role & PIECE_MASK) < 7 &&
          PIECE_COLOUR(orig->piece.role) == PIECE_COLOUR(nsq->piece.role)))) {
        fprintf(stderr, "move_piece(): invalid move (same square or same color "
                        "piece)\n");
        floating_piece      = &orig->piece;
        original_sqr->piece = *floating_piece;
        floating_piece      = NULL;
        destroy_piece(NULL, floating_piece);
        return 1;
    }

    if (!SQUARE_AVAILABLE(nsq, move_mask(squares, orig->piece.role))) {
        fprintf(stderr, "move_piece(): move not in valid move mask\n");
        floating_piece      = &orig->piece;
        original_sqr->piece = *floating_piece;
        floating_piece      = NULL;
        destroy_piece(NULL, floating_piece);
        return 1;
    }

    if (gs->check == gs->player) {
        if (!resolves_check(squares, orig, nsq, gs->player)) {
            fprintf(stderr, "move_piece(): move does not resolve check\n");
            floating_piece      = &orig->piece;
            original_sqr->piece = *floating_piece;
            floating_piece      = NULL;
            destroy_piece(NULL, floating_piece);
            return 1;
        }
        gs->check = 0;
    }

    struct piece_s orig_piece = orig->piece;
    struct piece_s nsq_piece  = nsq->piece;
    nsq->piece                = orig->piece;
    nsq->piece.role =
            (nsq->row << 12) | (nsq->col << 8) | (0x00FF & nsq->piece.role);
    orig->piece.role = (orig->piece.role | PIECE_MASK) & 0x0007;
    floating_piece   = &orig->piece;

    if (check_check(squares, PIECE_COLOUR(orig->piece.role)) > 0) {
        fprintf(stderr, "move_piece(): move puts own king in check\n");
        floating_piece      = &orig->piece;
        original_sqr->piece = *floating_piece;
        floating_piece      = NULL;
        destroy_piece(NULL, floating_piece);
        return 1;
    }

    if (check_discovered_check(squares, gs, orig, nsq)) {
        fprintf(stderr, "move_piece(): move causes discovered check\n");
        gs->check           = 2 - gs->player + 1;
        floating_piece      = &orig->piece;
        original_sqr->piece = *floating_piece;
        floating_piece      = NULL;
        destroy_piece(NULL, floating_piece);
        return 1;
    }

    orig->piece = orig_piece;
    nsq->piece  = nsq_piece;

    fprintf(stderr, "move_piece(): moving the held piece\n");
    if (floating_piece && floating_piece->state > MOVED) {
        destroy_piece(NULL, &nsq->piece);
    } else {
        nsq->piece = orig->piece;
        nsq->piece.role =
                (nsq->row << 12) | (nsq->col << 8) | (0x00FF & nsq->piece.role);
        if (floating_piece && floating_piece->state == UNMOVED) {
            floating_piece->state = MOVED;
        }
        if (gs->check)
            gs->check = 0;
    }
    fprintf(stderr, "move_piece(): removing piece from previous spot\n");
    destroy_piece(NULL, &orig->piece);
    floating_piece = NULL;

    gs->player     = (gs->player == WHITE_ID) ? BLACK_ID : WHITE_ID;
    cc             = check_check(squares, gs->player);
    if (cc != 0) {
        gs->check = gs->player;
        fprintf(stderr, "move_piece(): opponent king in check\n");
    }

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
