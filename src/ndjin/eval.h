/* eval.h
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

#ifndef EVAL_H
#define EVAL_H

#include "types.h"

#define KING_WEIGHT     ( double )200.0
#define QUEEN_WEIGHT    ( double )9.0
#define ROOK_WEIGHT     ( double )5.0
#define BISHOP_WEIGHT   ( double )3.0
#define KNIGHT_WEIGHT   ( double )3.0
#define PAWN_WEIGHT     ( double )1.0
#define BAD_PAWN_WEIGHT ( double )-0.5 // TODO: Implement bad pawn finder
#define MOBILITY_WEIGHT ( double )0.1

void   filter_legal(struct state_t *state, struct move_list_t *moves,
                    struct move_list_t *legal);
double material_count(struct state_t *state);
double symmetric_eval(struct state_t *state, struct move_list_t *moves);

#endif /* EVAL_H */

/* vim: ft=c ts=4 sts=4 sw=4 ai et cin */
