/* eval.c
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

#include <math.h>
#include <string.h>

#include "bb.h"
#include "eval.h"
#include "types.h"

static inline int count_bits(u64 bitboard)
{
    unsigned int count = 0;
    for (count = 0; bitboard; ++count)
        bitboard &= bitboard - 1;
    return count;
}

double material_eval(struct state_t *state)
{
    double result = 0;

    int k, kprime = 0;
    int q, qprime = 0;
    int r, rprime = 0;
    int b, bprime = 0;
    int n, nprime = 0;
    int p, pprime = 0;
    if (state->side == white) {
        p      = count_bits(state->bitboards[0]);
        pprime = count_bits(state->bitboards[6]);
        n      = count_bits(state->bitboards[1]);
        nprime = count_bits(state->bitboards[7]);
        b      = count_bits(state->bitboards[2]);
        bprime = count_bits(state->bitboards[8]);
        r      = count_bits(state->bitboards[3]);
        rprime = count_bits(state->bitboards[9]);
        q      = count_bits(state->bitboards[4]);
        qprime = count_bits(state->bitboards[10]);
        k      = count_bits(state->bitboards[5]);
        kprime = count_bits(state->bitboards[11]);
    } else {
        p      = count_bits(state->bitboards[6]);
        pprime = count_bits(state->bitboards[0]);
        n      = count_bits(state->bitboards[7]);
        nprime = count_bits(state->bitboards[1]);
        b      = count_bits(state->bitboards[8]);
        bprime = count_bits(state->bitboards[2]);
        r      = count_bits(state->bitboards[9]);
        rprime = count_bits(state->bitboards[3]);
        q      = count_bits(state->bitboards[10]);
        qprime = count_bits(state->bitboards[4]);
        k      = count_bits(state->bitboards[11]);
        kprime = count_bits(state->bitboards[5]);
    }

    result += KING_WEIGHT * (( double )k - ( double )kprime);
    result += QUEEN_WEIGHT * (( double )q - ( double )qprime);
    result += ROOK_WEIGHT * (( double )r - ( double )rprime);
    result += BISHOP_WEIGHT * (( double )b - ( double )bprime);
    result += KNIGHT_WEIGHT * (( double )n - ( double )nprime);
    result += PAWN_WEIGHT * (( double )p - ( double )pprime);

    return result;
}

void filter_legal(struct state_t *state, struct move_list_t *moves,
                  struct move_list_t *legal)
{
    struct state_t backup = {0};
    for (int i = 0; i < 64; ++i) {
        for (int j = 0; j < moves->squares[i].count; ++j) {
            BOARD_BACKUP(state, &backup);
            if (make_move(&backup, moves->squares[i].moves[j], all_moves) ==
                1) {
                legal->squares[i].moves[legal->squares[i].count++] =
                        moves->squares[i].moves[j];
                ++legal->count;
            }
        }
    }
    return;
}

double symmetric_eval(struct state_t *state, struct move_list_t *moves)
{
    double score                  = 0.0;
    double material               = material_eval(state);
    score                        += material;

    struct move_list_t legals[1]  = {0};
    filter_legal(state, moves, legals);
    if (legals->count == 0) {
        return NAN;
    }

    score += MOBILITY_WEIGHT * ( double )legals->count;

    return score;
}

/* vim: ft=c ts=4 sts=4 sw=4 ai et cin */
