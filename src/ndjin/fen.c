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
#include <string.h>

#include "fen.h"
#include "types.h"

extern char char_pieces[];

int parse_fen(char *fen, struct state_t *game_state)
{
    memset(game_state->bitboards, 0, sizeof(u64) * 12);
    memset(game_state->positions, 0, sizeof(u64) * 3);
    game_state->side      = white;
    game_state->castle    = 0;
    game_state->enpassant = no_sq;
    game_state->ply       = 0;
    game_state->fullmoves = 1;

    char buf[512];
    snprintf(buf, 512, "%s", fen);
    char *token = strtok(buf, " ");

    if (!token)
        return -1;

    int rank = 7;
    int file = 0;

    for (char *c = token; *c != '\0'; c++) {
        if (*c == '/') {
            if (file != 8)
                return -1;
            file = 0;
            rank--;
            if (rank < 0)
                return -1;
            continue;
        }

        if (*c >= '1' && *c <= '8') {
            file += (*c - '0');
            if (file > 8)
                return -1;
            continue;
        }

        int piece = -1;
        switch (*c) {
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
        default:
            return -1;
        }

        if (file >= 8 || rank < 0)
            return -1;

        int square                                = rank * 8 + file;
        game_state->bitboards[piece]             |= (1ULL << square);
        game_state->positions[piece < 6 ? 0 : 1] |= (1ULL << square);
        game_state->positions[2]                 |= (1ULL << square);
        file++;
    }

    if (rank != 0 || file != 8)
        return -1;

    token = strtok(NULL, " ");
    if (!token)
        return -1;
    if (token[0] == 'w')
        game_state->side = white;
    else if (token[0] == 'b')
        game_state->side = black;
    else
        return -1;

    token = strtok(NULL, " ");
    if (!token)
        return -1;
    if (token[0] != '-') {
        for (char *c = token; *c != '\0'; c++) {
            switch (*c) {
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
            default:
                return -1;
            }
        }
    }

    token = strtok(NULL, " ");
    if (!token)
        return -1;
    if (token[0] != '-') {
        if (strlen(token) != 2 || token[0] < 'a' || token[0] > 'h' ||
            token[1] < '1' || token[1] > '8') {
            return -1;
        }
        int file              = token[0] - 'a';
        int rank              = token[1] - '1';
        game_state->enpassant = rank * 8 + file;
    }

    token = strtok(NULL, " ");
    if (!token)
        return -1;
    for (char *c = token; *c != '\0'; c++) {
        if (*c < '0' || *c > '9')
            return -1;
    }
    game_state->ply = atoi(token);

    token           = strtok(NULL, " ");
    if (!token)
        return -1;
    for (char *c = token; *c != '\0'; c++) {
        if (*c < '0' || *c > '9')
            return -1;
    }
    game_state->fullmoves = atoi(token);

    return 0;
}

#ifdef _FEN_TEST
extern const char *square_to_coord[64];
extern const char  ascii_pieces[13];
extern const char *unicode_pieces[12];
extern const char  promoted_pieces[];
extern const char  piece_char[];

void print_bitboard(u64 bb)
{
    for (int rank = 7; rank >= 0; rank--) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            printf("%c ", (bb & (1ULL << square)) ? '1' : '.');
        }
        printf("\n");
    }
    printf("\n");
}

static inline void print_board(struct state_t *state, int unicode)
{
    printf("Bitboards:\n");
    const char *piece_names[12] = {"WP", "WN", "WB", "WR", "WQ", "WK",
                                   "BP", "BN", "BB", "BR", "BQ", "BK"};
    for (int k = 0; k < 12; k++) {
        printf("%s:\n", piece_names[k]);
        print_bitboard(state->bitboards[k]);
    }

    for (int i = 7; i >= 0; --i) {
        for (int j = 0; j < 8; ++j) {
            int sq    = (i * 8) + j;
            int piece = -1;
            for (int k = 0; k < 12; ++k) {
                if (state->bitboards[k] & (1ULL << sq)) {
                    piece = k;
                    break;
                }
            }
            if (!j)
                printf("  %d ", i + 1);
            if (!unicode)
                printf(" %c", (piece == -1) ? '.' : ascii_pieces[piece]);
            else
                printf(" %s", (piece == -1) ? "." : unicode_pieces[piece]);
        }
        puts("");
    }
    puts("\n     a b c d e f g h\n");
    printf("Side:\t\t%s\n", (state->side == white ? "white" : "black"));
    printf("En-passant:\t%s\n",
           (state->enpassant == no_sq ? "none"
                                      : square_to_coord[state->enpassant]));
    printf("Castling:\t%c%c%c%c\n", (state->castle & WKCK) ? 'K' : '.',
           (state->castle & WKCQ) ? 'Q' : '.',
           (state->castle & BKCK) ? 'k' : '.',
           (state->castle & BKCQ) ? 'q' : '.');
    printf("Half moves:\t%d\n", state->ply);
    printf("Moves:\t\t%d\n", state->fullmoves);
}

int main()
{
    struct state_t state = {0};
    int            result;

    printf("Testing standard starting position...\n");
    const char *fen1 =
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    result = parse_fen(( char * )fen1, &state);
    print_board(&state, 1);

    u64 expected_wp    = 0x000000000000FF00ULL;
    u64 expected_bp    = 0x00FF000000000000ULL;
    u64 expected_wn    = 0x0000000000000042ULL;
    u64 expected_bn    = 0x4200000000000000ULL;
    u64 expected_white = 0x000000000000FFFFULL;
    u64 expected_black = 0xFFFF000000000000ULL;
    u64 expected_all   = expected_white | expected_black;

    if (result != 0) {
        printf("Test 1 failed: parse_fen returned error\n");
        return 1;
    }
    if (state.bitboards[P] != expected_wp) {
        printf("Test 1 failed: white pawns incorrect\n");
        return 1;
    }
    if (state.bitboards[p] != expected_bp) {
        printf("Test 1 failed: black pawns incorrect\n");
        return 1;
    }
    if (state.bitboards[N] != expected_wn) {
        printf("Test 1 failed: white knights incorrect\n");
        return 1;
    }
    if (state.bitboards[n] != expected_bn) {
        printf("Test 1 failed: black knights incorrect\n");
        return 1;
    }
    if (state.positions[0] != expected_white) {
        printf("Test 1 failed: white pieces position incorrect\n");
        return 1;
    }
    if (state.positions[1] != expected_black) {
        printf("Test 1 failed: black pieces position incorrect\n");
        return 1;
    }
    if (state.positions[2] != expected_all) {
        printf("Test 1 failed: all pieces position incorrect\n");
        return 1;
    }
    if (state.side != white || state.castle != (WKCK | WKCQ | BKCK | BKCQ) ||
        state.enpassant != no_sq || state.ply != 0 || state.fullmoves != 1) {
        printf("Test 1 failed: game state fields incorrect\n");
        return 1;
    }
    printf("%s\n", fen1);
    printf("Test 1 passed\n");

    printf("\nTesting mid-game position...\n");
    const char *fen2 =
            "rnbqkb1r/pppp1ppp/5n2/8/8/5N2/PPPP1PPP/RNBQKB1R w KQkq - 0 1";
    result = parse_fen(( char * )fen2, &state);
    print_board(&state, 1);

    expected_wp    = 0x000000000000EF00ULL;
    expected_bp    = 0x00EF000000000000ULL;
    expected_wn    = 0x0000000000200002ULL;
    expected_bn    = 0x0200200000000000ULL;
    expected_white = 0x000000000020EFBFULL;
    expected_black = 0xBFEF200000000000ULL;
    expected_all   = expected_white | expected_black;

    if (result != 0) {
        printf("Test 2 failed: parse_fen returned error\n");
        return 1;
    }
    if (state.bitboards[P] != expected_wp) {
        printf("Test 2 failed: white pawns incorrect (got 0x%016llx, expected "
               "0x%016llx)\n",
               state.bitboards[P], expected_wp);
        return 1;
    }
    if (state.bitboards[p] != expected_bp) {
        printf("Test 2 failed: black pawns incorrect (got 0x%016llx, expected "
               "0x%016llx)\n",
               state.bitboards[p], expected_bp);
        return 1;
    }
    if (state.bitboards[N] != expected_wn) {
        printf("Test 2 failed: white knights incorrect (got 0x%016llx, "
               "expected "
               "0x%016llx)\n",
               state.bitboards[N], expected_wn);
        return 1;
    }
    if (state.bitboards[n] != expected_bn) {
        printf("Test 2 failed: black knights incorrect (got 0x%016llx, "
               "expected "
               "0x%016llx)\n",
               state.bitboards[n], expected_bn);
        return 1;
    }
    if (state.positions[0] != expected_white) {
        printf("Test 2 failed: white pieces incorrect (got 0x%016llx, expected "
               "0x%016llx)\n",
               state.positions[0], expected_white);
        return 1;
    }
    if (state.positions[1] != expected_black) {
        printf("Test 2 failed: black pieces incorrect (got 0x%016llx, expected "
               "0x%016llx)\n",
               state.positions[1], expected_black);
        return 1;
    }
    if (state.positions[2] != expected_all) {
        printf("Test 2 failed: all pieces incorrect (got 0x%016llx, expected "
               "0x%016llx)\n",
               state.positions[2], expected_all);
        return 1;
    }
    if (state.side != white || state.castle != (WKCK | WKCQ | BKCK | BKCQ) ||
        state.enpassant != no_sq || state.ply != 0 || state.fullmoves != 1) {
        printf("Test 2 failed: game state fields incorrect\n");
        return 1;
    }
    printf("%s\n", fen2);
    printf("Test 2 passed\n");

    printf("\nTesting invalid FEN string...\n");
    const char *fen3 = "rnbqkb1r/pppp1ppp/5n2/8/8/5N2/PPPP1PPP/RNBQKB1R w KQkq "
                       "invalid 0 1";
    result           = parse_fen(( char * )fen3, &state);
    if (result == 0) {
        printf("Test 3 failed: invalid FEN should return error\n");
        return 1;
    }
    printf("Test 3 passed\n");

    // Test 4: En passant position (after 1. e4 e6 2. e5 d5)
    printf("\nTesting en passant position...\n");
    const char *fen4 =
            "rnbqkb1r/ppp1pppp/5n2/3pP3/8/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 2";
    result = parse_fen(( char * )fen4, &state);
    print_board(&state, 1);

    expected_wp = 0x000000100000EF00ULL; // White pawns on a2, b2, c2, d2, e4,
                                         // f2, g2, h2
    expected_bp = 0x00F7000800000000ULL; // Black pawns on a7, b7, c7, d7, f7,
                                         // g7, h7, h6
    expected_wn = 0x0000000000000042ULL; // White knights on b1, g1
    expected_bn = 0x0200200000000000ULL; // Black knights on b8, g8
    expected_white = 0x000000100000EFFFULL; // All white pieces
    expected_black = 0xBFF7200800000000ULL; // All black pieces
    expected_all   = expected_white | expected_black;

    if (result != 0) {
        printf("Test 4 failed: parse_fen returned error\n");
        return 1;
    }
    if (state.bitboards[P] != expected_wp) {
        printf("Test 4 failed: white pawns incorrect (got 0x%016llx, expected "
               "0x%016llx)\n",
               state.bitboards[P], expected_wp);
        return 1;
    }
    if (state.bitboards[p] != expected_bp) {
        printf("Test 4 failed: black pawns incorrect (got 0x%016llx, expected "
               "0x%016llx)\n",
               state.bitboards[p], expected_bp);
        return 1;
    }
    if (state.bitboards[N] != expected_wn) {
        printf("Test 4 failed: white knights incorrect (got 0x%016llx, "
               "expected 0x%016llx)\n",
               state.bitboards[N], expected_wn);
        return 1;
    }
    if (state.bitboards[n] != expected_bn) {
        printf("Test 4 failed: black knights incorrect (got 0x%016llx, "
               "expected 0x%016llx)\n",
               state.bitboards[n], expected_bn);
        return 1;
    }
    if (state.positions[0] != expected_white) {
        printf("Test 4 failed: white pieces incorrect (got 0x%016llx, expected "
               "0x%016llx)\n",
               state.positions[0], expected_white);
        return 1;
    }
    if (state.positions[1] != expected_black) {
        printf("Test 4 failed: black pieces incorrect (got 0x%016llx, expected "
               "0x%016llx)\n",
               state.positions[1], expected_black);
        return 1;
    }
    if (state.positions[2] != expected_all) {
        printf("Test 4 failed: all pieces incorrect (got 0x%016llx, expected "
               "0x%016llx)\n",
               state.positions[2], expected_all);
        return 1;
    }
    if (state.side != white || state.castle != (WKCK | WKCQ | BKCK | BKCQ) ||
        state.enpassant != 43 || state.ply != 0 || state.fullmoves != 2) {
        printf("Test 4 failed: game state fields incorrect (enpassant got %d, "
               "expected 43)\n",
               state.enpassant);
        return 1;
    }
    printf("%s\n", fen4);
    printf("Test 4 passed\n");

    // Test 5: Limited castling position (after 1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 4.
    // Bxc6 dxc6)
    printf("\nTesting limited castling position...\n");
    const char *fen5 = "4k2r/6r1/8/8/8/8/3R4/R3K3 w Qk - 0 1";
    result           = parse_fen(( char * )fen5, &state);
    print_board(&state, 1);

    u64 expected_wr = 0x0000000000000801ULL;

    u64 expected_br = 0x8040000000000000ULL;

    u64 expected_wk = 0x0000000000000010ULL;
    u64 expected_bk = 0x1000000000000000ULL;
    expected_white  = 0x0000000000000811ULL; // All white pieces
    expected_black  = 0x9040000000000000ULL; // All black pieces
    expected_all    = expected_white | expected_black;

    if (result != 0) {
        printf("Test 5 failed: parse_fen returned error\n");
        return 1;
    }
    if (state.bitboards[K] != expected_wk) {
        printf("Test 5 failed: white kings incorrect (got 0x%016llx, expected "
               "0x%016llx)\n",
               state.bitboards[K], expected_wk);
        return 1;
    }
    if (state.bitboards[k] != expected_bk) {
        printf("Test 5 failed: black kings incorrect (got 0x%016llx, expected "
               "0x%016llx)\n",
               state.bitboards[k], expected_bk);
        return 1;
    }
    if (state.bitboards[R] != expected_wr) {
        printf("Test 5 failed: white rooks incorrect (got 0x%016llx, "
               "expected 0x%016llx)\n",
               state.bitboards[R], expected_wr);
        return 1;
    }
    if (state.bitboards[r] != expected_br) {
        printf("Test 5 failed: black rooks incorrect (got 0x%016llx, "
               "expected 0x%016llx)\n",
               state.bitboards[r], expected_br);
        return 1;
    }
    if (state.positions[0] != expected_white) {
        printf("Test 5 failed: white pieces incorrect (got 0x%016llx, expected "
               "0x%016llx)\n",
               state.positions[0], expected_white);
        return 1;
    }
    if (state.positions[1] != expected_black) {
        printf("Test 5 failed: black pieces incorrect (got 0x%016llx, expected "
               "0x%016llx)\n",
               state.positions[1], expected_black);
        return 1;
    }
    if (state.positions[2] != expected_all) {
        printf("Test 5 failed: all pieces incorrect (got 0x%016llx, expected "
               "0x%016llx)\n",
               state.positions[2], expected_all);
        return 1;
    }
    if (state.side != white || state.castle != (WKCQ | BKCK) ||
        state.enpassant != no_sq || state.ply != 0 || state.fullmoves != 1) {
        printf("Test 5 failed: game state fields incorrect\n");
        return 1;
    }
    printf("%s\n", fen5);
    printf("Test 5 passed\n");

    return 0;
}
#endif

/* vim: ft=c ts=4 sts=4 sw=4 ai et cin */
