/* fen.c
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

#include <stdlib.h>
#include <string.h>

#include "fen.h"
#include "types.h"

extern char char_pieces[];

int parse_fen(char *fen, struct state_t *game_state)
{
    memset(game_state->bitboards, 0, sizeof(u64) * 12);
    memset(game_state->positions, 0, sizeof(u64) * 3);

    int stage = rank8;
    int file  = 0;
    int rank  = 7; // Start at rank 8 (7 in 0-based indexing)

    while (*fen && stage <= rank1) {
        char c = *fen;
        ++fen;

        if (c == ' ') {
            stage = turn;
            break;
        }

        if (c == '/') {
            file = 0;
            --rank;
            continue;
        }

        if (c >= '1' && c <= '8') {
            file += c - '0';
            continue;
        }

        int piece = -1;
        switch (c) {
        case 'P':
            piece = P;
            break;
        case 'N':
            piece = N;
            break;
        case 'B':
            piece = B;
            break;
        case 'R':
            piece = R;
            break;
        case 'Q':
            piece = Q;
            break;
        case 'K':
            piece = K;
            break;
        case 'p':
            piece = p;
            break;
        case 'n':
            piece = n;
            break;
        case 'b':
            piece = b;
            break;
        case 'r':
            piece = r;
            break;
        case 'q':
            piece = q;
            break;
        case 'k':
            piece = k;
            break;
        }

        if (piece >= 0) {
            int square                                = rank * 8 + file;
            game_state->bitboards[piece]             |= (1ULL << square);
            game_state->positions[piece < 6 ? 0 : 1] |= (1ULL << square);
            game_state->positions[2]                 |= (1ULL << square);
            ++file;
        }
    }

    // Handle remaining stages (turn, castling, en passant, halfmove, fullmove)
    while (*fen && stage <= fullmove) {
        char c = *fen;
        ++fen;

        if (stage == turn) {
            game_state->side = (c == 'w') ? white : black;
            ++stage;
            continue;
        }

        if (stage == castling) {
            if (c == ' ') {
                ++stage;
                continue;
            }
            if (c == '-') {
                if (*fen == ' ') {
                    ++fen;
                    ++stage;
                }
                continue;
            }
            switch (c) {
            case 'K':
                game_state->castle |= WKCK;
                break;
            case 'Q':
                game_state->castle |= WKCQ;
                break;
            case 'k':
                game_state->castle |= BKCK;
                break;
            case 'q':
                game_state->castle |= BKCQ;
                break;
            }
            continue;
        }

        if (stage == epass) {
            if (c == ' ') {
                ++stage;
                continue;
            }
            if (c == '-') {
                if (*fen == ' ') {
                    ++fen;
                    ++stage;
                }
                continue;
            }
            if (c >= 'a' && c <= 'h' && *fen >= '1' && *fen <= '8') {
                int i                 = (*fen - '1');
                int j                 = c - 'a';
                game_state->enpassant = i * 8 + j;
                ++fen;
                if (*fen == ' ') {
                    ++fen;
                    ++stage;
                }
            }
            continue;
        }

        if (stage == halfmove) {
            if (c >= '0' && c <= '9') {
                char *endptr;
                game_state->ply = strtol(fen - 1, &endptr, 10);
                fen             = endptr;
                if (*fen == ' ') {
                    ++fen;
                    ++stage;
                }
            }
            continue;
        }

        if (stage == fullmove) {
            if (c >= '0' && c <= '9') {
                char *endptr;
                game_state->fullmoves = strtol(fen - 1, &endptr, 10);
                fen                   = endptr;
                ++stage;
            }
        }
    }

    return 0;
}

/* vim: ft=c ts=4 sts=4 sw=4 ai et cin */
