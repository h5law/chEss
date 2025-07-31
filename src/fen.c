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
#include <stdio.h>

#include "engine/bb.h"
#include "fen.h"
#include "types.h"

extern char char_pieces[];

int parse_fen(char *fen, u64 bitboards[12], u64 positions[3], int *side,
              int *castle, int *enpassant, int *ply, int *moves)
{
    char c;
    int  stage  = rank8;
    int  indent = 0;
    while (*fen && stage <= fullmove) {
        if (stage <= rank1) {
            c = *fen++;
            if (c == '8') {
                positions[0] ^= (0xFF << (8 * (7 - stage)));
                positions[1] ^= (0xFF << (8 * (7 - stage)));
                positions[2] ^= (0xFF << (8 * (7 - stage)));
                ++fen; // /
                ++stage;
                indent = 0;
                continue;
            }
            if (c == '/' || c == ' ') {
                ++stage;
                indent = 0;
                continue;
            }
            int count = 1;
            if (c <= '7' && c >= '1') {
                count = atoi(&c);
                c     = *fen++;
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
            case '/':
                ++stage;
                indent = 0;
                break;
            }
            if (piece >= 0) {
                for (int i = indent; i < indent + count; ++i) {
                    bitboards[piece] |= (1ULL << ((8 * (7 - stage)) + i));
                    if (piece < 6)
                        positions[0] |= (1ULL << ((8 * (7 - stage)) + i));
                    else
                        positions[1] |= (1ULL << ((8 * (7 - stage)) + i));
                    positions[2] |= (1ULL << ((8 * (7 - stage)) + i));
                }
                indent += count;
            }
        } else if (stage == turn) {
            c = *fen++;
            switch (c) {
            case 'w':
                *side = white;
                ++stage;
                ++fen;
                break;
            case 'b':
                *side = black;
                ++stage;
                ++fen;
                break;
            case ' ':
                ++stage;
                break;
            }
        } else if (stage == castling) {
            c = *fen++;
            switch (c) {
            case '.':
                break;
            case 'K':
                *castle |= WKCK;
                break;
            case 'Q':
                *castle |= WKCQ;
                break;
            case 'k':
                *castle |= BKCK;
                break;
            case 'q':
                *castle |= BKCQ;
                break;
            case '-':
                ++stage;
                break;
            case ' ':
                ++stage;
                break;
            }
        } else if (stage == epass) {
            c = *fen++;
            switch (c) {
            case '-':
                ++stage;
                break;
            default:
                char square[2] = {c, *fen};
                if (c >= 'a' && c <= 'h' && square[1] >= '1' &&
                    square[1] <= '8') {
                    *enpassant = (atoi(&square[1]) * 8);
                    ++fen;
                }
                ++stage;
                break;
            }
            if (*--fen != ' ') {
                while (*fen != ' ')
                    ++fen;
            }
        } else if (stage == halfmove) {
            *ply = atoi(++fen);
            ++stage;
            while (*fen != ' ')
                ++fen;
        } else if (stage == fullmove) {
            ++fen;
            *moves = atoi(fen);
            ++stage;
        }
    }
    return 0;
}
