/* moves.h
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

#ifndef MOVES_H
#define MOVES_H

#include <stdint.h>

#include "board.h"

#define SQUARE_AVAILABLE(square, move_map)                                     \
    (((( uint64_t )1 << square->col) << (square->row * 8) & move_map) > 0 ? 1  \
                                                                          : 0)

uint64_t move_mask(struct square_t *board, uint16_t piece);
uint64_t knight_move_map(uint8_t row, uint8_t col);
uint64_t pawn_move_map(struct square_t *board, uint8_t row, uint8_t col);
uint64_t bishop_move_map(struct square_t *board, uint8_t row, uint8_t col);
uint64_t rook_move_map(struct square_t *board, uint8_t row, uint8_t col);
uint64_t queen_move_map(struct square_t *board, uint8_t row, uint8_t col);
uint64_t king_move_map(struct square_t *board, uint8_t row, uint8_t col);

#endif /* MOVES_H */
