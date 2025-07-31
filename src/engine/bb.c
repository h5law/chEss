/* bb.c
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

#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include "bb.h"

/******************************************************************************
 * Pieces
 ******************************************************************************/

/* clang-format off */
const char *square_to_coord[] = {
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
};

const char  ascii_pieces[13]   = "PNBRQKpnbrqk";
const char *unicode_pieces[12] = {"♟︎", "♞", "♝", "♜", "♛", "♚",
                                  "♙", "♘", "♗", "♖", "♕", "♔"};

const char promoted_pieces[] = {[Q] = 'q', [R] = 'r', [B] = 'b', [N] = 'n',
                                [q] = 'q', [r] = 'r', [b] = 'b', [n] = 'n'};
/* clang-format on */

/******************************************************************************
 * Board
 ******************************************************************************/

/*
 *                            WHITE PIECES
 *
 *
 *        Pawns                  Knights              Bishops
 *
 *  8  0 0 0 0 0 0 0 0    8  0 0 0 0 0 0 0 0    8  0 0 0 0 0 0 0 0
 *  7  0 0 0 0 0 0 0 0    7  0 0 0 0 0 0 0 0    7  0 0 0 0 0 0 0 0
 *  6  0 0 0 0 0 0 0 0    6  0 0 0 0 0 0 0 0    6  0 0 0 0 0 0 0 0
 *  5  0 0 0 0 0 0 0 0    5  0 0 0 0 0 0 0 0    5  0 0 0 0 0 0 0 0
 *  4  0 0 0 0 0 0 0 0    4  0 0 0 0 0 0 0 0    4  0 0 0 0 0 0 0 0
 *  3  0 0 0 0 0 0 0 0    3  0 0 0 0 0 0 0 0    3  0 0 0 0 0 0 0 0
 *  2  1 1 1 1 1 1 1 1    2  0 0 0 0 0 0 0 0    2  0 0 0 0 0 0 0 0
 *  1  0 0 0 0 0 0 0 0    1  0 1 0 0 0 0 1 0    1  0 0 1 0 0 1 0 0
 *
 *     a b c d e f g h       a b c d e f g h       a b c d e f g h
 *
 *
 *         Rooks                 Queens                 King
 *
 *  8  0 0 0 0 0 0 0 0    8  0 0 0 0 0 0 0 0    8  0 0 0 0 0 0 0 0
 *  7  0 0 0 0 0 0 0 0    7  0 0 0 0 0 0 0 0    7  0 0 0 0 0 0 0 0
 *  6  0 0 0 0 0 0 0 0    6  0 0 0 0 0 0 0 0    6  0 0 0 0 0 0 0 0
 *  5  0 0 0 0 0 0 0 0    5  0 0 0 0 0 0 0 0    5  0 0 0 0 0 0 0 0
 *  4  0 0 0 0 0 0 0 0    4  0 0 0 0 0 0 0 0    4  0 0 0 0 0 0 0 0
 *  3  0 0 0 0 0 0 0 0    3  0 0 0 0 0 0 0 0    3  0 0 0 0 0 0 0 0
 *  2  0 0 0 0 0 0 0 0    2  0 0 0 0 0 0 0 0    2  0 0 0 0 0 0 0 0
 *  1  1 0 0 0 0 0 0 1    1  0 0 0 1 0 0 0 0    1  0 0 0 0 1 0 0 0
 *
 *     a b c d e f g h       a b c d e f g h       a b c d e f g h
 *
 *
 *
 *                            BLACK PIECES
 *
 *
 *        Pawns                  Knights              Bishops
 *
 *  8  0 0 0 0 0 0 0 0    8  0 1 0 0 0 0 1 0    8  0 0 1 0 0 1 0 0
 *  7  1 1 1 1 1 1 1 1    7  0 0 0 0 0 0 0 0    7  0 0 0 0 0 0 0 0
 *  6  0 0 0 0 0 0 0 0    6  0 0 0 0 0 0 0 0    6  0 0 0 0 0 0 0 0
 *  5  0 0 0 0 0 0 0 0    5  0 0 0 0 0 0 0 0    5  0 0 0 0 0 0 0 0
 *  4  0 0 0 0 0 0 0 0    4  0 0 0 0 0 0 0 0    4  0 0 0 0 0 0 0 0
 *  3  0 0 0 0 0 0 0 0    3  0 0 0 0 0 0 0 0    3  0 0 0 0 0 0 0 0
 *  2  0 0 0 0 0 0 0 0    2  0 0 0 0 0 0 0 0    2  0 0 0 0 0 0 0 0
 *  1  0 0 0 0 0 0 0 0    1  0 0 0 0 0 0 0 0    1  0 0 0 0 0 0 0 0
 *
 *     a b c d e f g h       a b c d e f g h       a b c d e f g h
 *
 *
 *         Rooks                 Queens                 King
 *
 *  8  1 0 0 0 0 0 0 1    8  0 0 0 1 0 0 0 0    8  0 0 0 0 1 0 0 0
 *  7  0 0 0 0 0 0 0 0    7  0 0 0 0 0 0 0 0    7  0 0 0 0 0 0 0 0
 *  6  0 0 0 0 0 0 0 0    6  0 0 0 0 0 0 0 0    6  0 0 0 0 0 0 0 0
 *  5  0 0 0 0 0 0 0 0    5  0 0 0 0 0 0 0 0    5  0 0 0 0 0 0 0 0
 *  4  0 0 0 0 0 0 0 0    4  0 0 0 0 0 0 0 0    4  0 0 0 0 0 0 0 0
 *  3  0 0 0 0 0 0 0 0    3  0 0 0 0 0 0 0 0    3  0 0 0 0 0 0 0 0
 *  2  0 0 0 0 0 0 0 0    2  0 0 0 0 0 0 0 0    2  0 0 0 0 0 0 0 0
 *  1  0 0 0 0 0 0 0 0    1  0 0 0 0 0 0 0 0    1  0 0 0 0 0 0 0 0
 *
 *     a b c d e f g h       a b c d e f g h       a b c d e f g h
 *
 *
 *
 *                             POSITIONS
 *
 *
 *     White position       Black position       All positions
 *
 *  8  0 0 0 0 0 0 0 0    8  1 1 1 1 1 1 1 1    8  1 1 1 1 1 1 1 1
 *  7  0 0 0 0 0 0 0 0    7  1 1 1 1 1 1 1 1    7  1 1 1 1 1 1 1 1
 *  6  0 0 0 0 0 0 0 0    6  0 0 0 0 0 0 0 0    6  0 0 0 0 0 0 0 0
 *  5  0 0 0 0 0 0 0 0    5  0 0 0 0 0 0 0 0    5  0 0 0 0 0 0 0 0
 *  4  0 0 0 0 0 0 0 0    4  0 0 0 0 0 0 0 0    4  0 0 0 0 0 0 0 0
 *  3  0 0 0 0 0 0 0 0    3  0 0 0 0 0 0 0 0    3  0 0 0 0 0 0 0 0
 *  2  1 1 1 1 1 1 1 1    2  0 0 0 0 0 0 0 0    2  1 1 1 1 1 1 1 1
 *  1  1 1 1 1 1 1 1 1    1  0 0 0 0 0 0 0 0    1  1 1 1 1 1 1 1 1
 *
 *
 *
 *                            ALL TOGETHER
 *
 *
 *                        8  ♜ ♞ ♝ ♛ ♚ ♝ ♞ ♜
 *                        7  ♟︎ ♟︎ ♟︎ ♟︎ ♟︎ ♟︎ ♟︎ ♟︎
 *                        6  . . . . . . . .
 *                        5  . . . . . . . .
 *                        4  . . . . . . . .
 *                        3  . . . . . . . .
 *                        2  ♙ ♙ ♙ ♙ ♙ ♙ ♙ ♙
 *                        1  ♖ ♘ ♗ ♕ ♔ ♗ ♘ ♖
 *
 *                           a b c d e f g h
 *
 */

/* piece bitboards */
u64 bitboards[12];

/* position bitboards */
u64 positions[3];

/* side to move */
int side      = -1;

int enpassant = no_sq;

int castle;

u64 checksum;

/* positions repetition table */
u64 repetition_table[1000]; /* 1000 plies (500 moves) in the entire game */

/* repetition index */
int repetition_index;

/* half move counter */
int ply;

/* fifty move rule counter */
int fifty;

/* Full move counter */
int moves;

/******************************************************************************
 * Time
 ******************************************************************************/

/* exit from engine */
int quit    = 0;

/* flag time control availability */
int timeset = 0;

/* flag time is out */
int stopped = 0;

/******************************************************************************
 * Functions
 ******************************************************************************/

int get_time_ms(void)
{
    struct timeval time_value;
    gettimeofday(&time_value, NULL);
    return time_value.tv_sec * 1000 + time_value.tv_usec / 1000;
}

int count_bits(u64 bitboard)
{
    unsigned int count = 0;
    for (count = 0; bitboard; ++count)
        bitboard &= bitboard - 1;
    return count;
}

int get_lsb_index(u64 bitboard)
{
    if (bitboard)
        return count_bits((bitboard & -bitboard) - 1);
    else
        return -1;
}

void print_tiles(void)
{
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            int sq = (i * 8) + j;
            if (!j)
                printf("  %d ", 8 - i);
            printf(" %s ", ( char * )square_to_coord[sq]);
        }
        puts("");
    }
    puts("\n     a   b   c   d   e   f   g   h\n");
}

void print_bitboard(u64 bitboard)
{
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            int sq = (i * 8) + j;
            if (!j)
                printf("  %d ", 8 - i);
            printf(" %d", get_bit(bitboard, sq) ? 1 : 0);
        }
        puts("");
    }
    puts("\n     a b c d e f g h\n");
    printf("Bitboard: %llu (%d : %d)\n\n", bitboard, get_lsb_index(bitboard),
           count_bits(bitboard));
}

void print_board(int unicode)
{
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            int sq    = (i * 8) + j;
            int piece = -1;
            for (int k = 0; k < 12; ++k) {
                if (get_bit(bitboards[k], sq) > 0)
                    piece = k;
            }
            if (!j)
                printf("  %d ", 8 - i);
            if (!unicode)
                printf(" %c", (piece == -1) ? '.' : ascii_pieces[piece]);
            else
                printf(" %s", (piece == -1) ? "." : unicode_pieces[piece]);
        }
        puts("");
    }
    puts("\n     a b c d e f g h\n");
    printf("Side:\t\t%s\n", (!side ? "white" : "black"));
    printf("En-passant:\t%s\n",
           (enpassant == no_sq ? "none" : square_to_coord[enpassant]));
    printf("Castling:\t%c%c%c%c\n", (castle & WKCK) ? 'K' : '.',
           (castle & WKCQ) ? 'Q' : '.', (castle & BKCK) ? 'k' : '.',
           (castle & BKCQ) ? 'q' : '.');
    printf("Half moves:\t%d\n", ply);
    printf("Moves:\t\t%d\n", moves);
}

/******************************************************************************
 * Attack Tables
 ******************************************************************************/

/*
 *      not_a_file                     not_ab_file
 *
 *  8  0 1 1 1 1 1 1 1             8  0 0 1 1 1 1 1 1
 *  7  0 1 1 1 1 1 1 1             7  0 0 1 1 1 1 1 1
 *  6  0 1 1 1 1 1 1 1             6  0 0 1 1 1 1 1 1
 *  5  0 1 1 1 1 1 1 1             5  0 0 1 1 1 1 1 1
 *  4  0 1 1 1 1 1 1 1             4  0 0 1 1 1 1 1 1
 *  3  0 1 1 1 1 1 1 1             3  0 0 1 1 1 1 1 1
 *  2  0 1 1 1 1 1 1 1             2  0 0 1 1 1 1 1 1
 *  1  0 1 1 1 1 1 1 1             1  0 0 1 1 1 1 1 1
 *
 *    a b c d e f g h                a b c d e f g h
 *
 *
 *
 *      not_h_file                      not_hg_file
 *
 *  8  1 1 1 1 1 1 1 0              8  1 1 1 1 1 1 0 0
 *  7  1 1 1 1 1 1 1 0              7  1 1 1 1 1 1 0 0
 *  6  1 1 1 1 1 1 1 0              6  1 1 1 1 1 1 0 0
 *  5  1 1 1 1 1 1 1 0              5  1 1 1 1 1 1 0 0
 *  4  1 1 1 1 1 1 1 0              4  1 1 1 1 1 1 0 0
 *  3  1 1 1 1 1 1 1 0              3  1 1 1 1 1 1 0 0
 *  2  1 1 1 1 1 1 1 0              2  1 1 1 1 1 1 0 0
 *  1  1 1 1 1 1 1 1 0              1  1 1 1 1 1 1 0 0
 *
 *    a b c d e f g h                 a b c d e f g h
 *
 */
const u64 not_a_file  = 18374403900871474942ULL;
const u64 not_ab_file = 18229723555195321596ULL;
const u64 not_h_file  = 9187201950435737471ULL;
const u64 not_hg_file = 4557430888798830399ULL;

/* Pawns
 * [sides] white black
 * [square] 0 - 63
 */
/* clang-format off */
const u64 pawn_attacks[2][64] = {
        /* white attacks */
        {
         0ULL,                          0ULL,
         0ULL,                          0ULL,
         0ULL,                          0ULL,
         0ULL,                          0ULL,
         2ULL,                          5ULL,
         10ULL,                         20ULL,
         40ULL,                         80ULL,
         160ULL,                        64ULL,
         512ULL,                        1280ULL,
         2560ULL,                       5120ULL,
         10240ULL,                      20480ULL,
         40960ULL,                      16384ULL,
         131072ULL,                     327680ULL,
         655360ULL,                     1310720ULL,
         2621440ULL,                    5242880ULL,
         10485760ULL,                   4194304ULL,
         33554432ULL,                   83886080ULL,
         167772160ULL,                  335544320ULL,
         671088640ULL,                  1342177280ULL,
         2684354560ULL,                 1073741824ULL,
         8589934592ULL,                 21474836480ULL,
         42949672960ULL,                85899345920ULL,
         171798691840ULL,               343597383680ULL,
         687194767360ULL,               274877906944ULL,
         2199023255552ULL,              5497558138880ULL,
         10995116277760ULL,             21990232555520ULL,
         43980465111040ULL,             87960930222080ULL,
         175921860444160ULL,            70368744177664ULL,
         562949953421312ULL,            1407374883553280ULL,
         2814749767106560ULL,           5629499534213120ULL,
         11258999068426240ULL,          22517998136852480ULL,
         45035996273704960ULL,          18014398509481984ULL},

        /* black attacks */
        {
         512ULL,                        1280ULL,
         2560ULL,                       5120ULL,
         10240ULL,                      20480ULL,
         40960ULL,                      16384ULL,
         131072ULL,                     327680ULL,
         655360ULL,                     1310720ULL,
         2621440ULL,                    5242880ULL,
         10485760ULL,                   4194304ULL,
         33554432ULL,                   83886080ULL,
         167772160ULL,                  335544320ULL,
         671088640ULL,                  1342177280ULL,
         2684354560ULL,                 1073741824ULL,
         8589934592ULL,                 21474836480ULL,
         42949672960ULL,                85899345920ULL,
         171798691840ULL,               343597383680ULL,
         687194767360ULL,               274877906944ULL,
         2199023255552ULL,              5497558138880ULL,
         10995116277760ULL,             21990232555520ULL,
         43980465111040ULL,             87960930222080ULL,
         175921860444160ULL,            70368744177664ULL,
         562949953421312ULL,            1407374883553280ULL,
         2814749767106560ULL,           5629499534213120ULL,
         11258999068426240ULL,          22517998136852480ULL,
         45035996273704960ULL,          18014398509481984ULL,
         144115188075855872ULL,         360287970189639680ULL,
         720575940379279360ULL,         1441151880758558720ULL,
         2882303761517117440ULL,        5764607523034234880ULL,
         11529215046068469760ULL,       4611686018427387904ULL,
         0ULL,                          0ULL,
         0ULL,                          0ULL,
         0ULL,                          0ULL,
         0ULL,                          0ULL,
    },
};

const u64 knight_attacks[64] = {
        132096ULL,                      329728ULL,
        659712ULL,                      1319424ULL,
        2638848ULL,                     5277696ULL,
        10489856ULL,                    4202496ULL,
        33816580ULL,                    84410376ULL,
        168886289ULL,                   337772578ULL,
        675545156ULL,                   1351090312ULL,
        2685403152ULL,                  1075839008ULL,
        8657044482ULL,                  21609056261ULL,
        43234889994ULL,                 86469779988ULL,
        172939559976ULL,                345879119952ULL,
        687463207072ULL,                275414786112ULL,
        2216203387392ULL,               5531918402816ULL,
        11068131838464ULL,              22136263676928ULL,
        44272527353856ULL,              88545054707712ULL,
        175990581010432ULL,             70506185244672ULL,
        567348067172352ULL,             1416171111120896ULL,
        2833441750646784ULL,            5666883501293568ULL,
        11333767002587136ULL,           22667534005174272ULL,
        45053588738670592ULL,           18049583422636032ULL,
        145241105196122112ULL,          362539804446949376ULL,
        725361088165576704ULL,          1450722176331153408ULL,
        2901444352662306816ULL,         5802888705324613632ULL,
        11533718717099671552ULL,        4620693356194824192ULL,
        288234782788157440ULL,          576469569871282176ULL,
        1224997833292120064ULL,         2449995666584240128ULL,
        4899991333168480256ULL,         9799982666336960512ULL,
        1152939783987658752ULL,         2305878468463689728ULL,
        1128098930098176ULL,            2257297371824128ULL,
        4796069720358912ULL,            9592139440717824ULL,
        19184278881435648ULL,           38368557762871296ULL,
        4679521487814656ULL,            9077567998918656ULL,
};

const u64 king_attacks[64] = {
        770ULL,                         1797ULL,
        3594ULL,                        7188ULL,
        14376ULL,                       28752ULL,
        57504ULL,                       49216ULL,
        197123ULL,                      460039ULL,
        920078ULL,                      1840156ULL,
        3680312ULL,                     7360624ULL,
        14721248ULL,                    12599488ULL,
        50463488ULL,                    117769984ULL,
        235539968ULL,                   471079936ULL,
        942159872ULL,                   1884319744ULL,
        3768639488ULL,                  3225468928ULL,
        12918652928ULL,                 30149115904ULL,
        60298231808ULL,                 120596463616ULL,
        241192927232ULL,                482385854464ULL,
        964771708928ULL,                825720045568ULL,
        3307175149568ULL,               7718173671424ULL,
        15436347342848ULL,              30872694685696ULL,
        61745389371392ULL,              123490778742784ULL,
        246981557485568ULL,             211384331665408ULL,
        846636838289408ULL,             1975852459884544ULL,
        3951704919769088ULL,            7903409839538176ULL,
        15806819679076352ULL,           31613639358152704ULL,
        63227278716305408ULL,           54114388906344448ULL,
        216739030602088448ULL,          505818229730443264ULL,
        1011636459460886528ULL,         2023272918921773056ULL,
        4046545837843546112ULL,         8093091675687092224ULL,
        16186183351374184448ULL,        13853283560024178688ULL,
        144959613005987840ULL,          362258295026614272ULL,
        724516590053228544ULL,          1449033180106457088ULL,
        2898066360212914176ULL,         5796132720425828352ULL,
        11592265440851656704ULL,        4665729213955833856ULL,
};

u64 bishop_masks[64];

u64 rook_masks[64];

u64 bishop_attacks[64][512]; /* [square][positions] */

u64 rook_attacks[64][4096]; /* [square][positions] */
/* clang-format on */

u64 mask_pawn_attacks(int square, int side)
{
    u64 attacks  = 0ULL;
    u64 bitboard = 0ULL;
    set_bit(bitboard, square);

    if (!side) {
        if ((bitboard >> 7) & not_a_file)
            attacks |= (bitboard >> 7);
        if ((bitboard >> 9) & not_h_file)
            attacks |= (bitboard >> 9);
    } else {
        if ((bitboard << 7) & not_h_file)
            attacks |= (bitboard << 7);
        if ((bitboard << 9) & not_a_file)
            attacks |= (bitboard << 9);
    }

    return attacks;
}

u64 mask_knight_attacks(int square)
{
    u64 attacks  = 0ULL;
    u64 bitboard = 0ULL;
    set_bit(bitboard, square);

    /* 6 10 15 17 */
    if ((bitboard >> 17) & not_h_file)
        attacks |= (bitboard >> 17);
    if ((bitboard >> 15) & not_a_file)
        attacks |= (bitboard >> 15);
    if ((bitboard >> 10) & not_hg_file)
        attacks |= (bitboard >> 10);
    if ((bitboard >> 6) & not_ab_file)
        attacks |= (bitboard >> 6);
    if ((bitboard << 17) & not_a_file)
        attacks |= (bitboard << 17);
    if ((bitboard << 15) & not_h_file)
        attacks |= (bitboard << 15);
    if ((bitboard << 10) & not_ab_file)
        attacks |= (bitboard << 10);
    if ((bitboard << 6) & not_hg_file)
        attacks |= (bitboard << 6);

    return attacks;
}

u64 mask_king_attacks(int square)
{
    u64 attacks  = 0ULL;
    u64 bitboard = 0ULL;
    set_bit(bitboard, square);

    /* 9 8 7 1 1 7 8 9 */
    if ((bitboard >> 9) & not_h_file)
        attacks |= (bitboard >> 9);
    if (bitboard >> 8)
        attacks |= (bitboard >> 8);
    if ((bitboard >> 7) & not_a_file)
        attacks |= (bitboard >> 7);
    if ((bitboard >> 1) & not_h_file)
        attacks |= (bitboard >> 1);
    if ((bitboard << 9) & not_a_file)
        attacks |= (bitboard << 9);
    if (bitboard << 8)
        attacks |= (bitboard << 8);
    if ((bitboard << 7) & not_h_file)
        attacks |= (bitboard << 7);
    if ((bitboard << 1) & not_a_file)
        attacks |= (bitboard << 1);

    return attacks;
}

u64 mask_bishop_attacks(int square)
{
    u64 attacks  = 0ULL;
    u64 bitboard = 0ULL;
    set_bit(bitboard, square);

    int i, j = 0; /* rank, file */
    int r, f = 0; /* target rank, file */

    r = square / 8;
    f = square % 8;

    for (i = r + 1, j = f + 1; i <= 6 && j <= 6; ++i, ++j)
        attacks |= (1ULL << ((i * 8) + j));
    for (i = r - 1, j = f + 1; i >= 1 && j <= 6; --i, ++j)
        attacks |= (1ULL << ((i * 8) + j));
    for (i = r + 1, j = f - 1; i <= 6 && j >= 1; ++i, --j)
        attacks |= (1ULL << ((i * 8) + j));
    for (i = r - 1, j = f - 1; i >= 1 && j >= 1; --i, --j)
        attacks |= (1ULL << ((i * 8) + j));

    return attacks;
}

u64 mask_rook_attacks(int square)
{
    u64 attacks  = 0ULL;
    u64 bitboard = 0ULL;
    set_bit(bitboard, square);

    int i, j = 0; /* rank, file */
    int r, f = 0; /* target rank, file */

    r = square / 8;
    f = square % 8;

    for (i = r + 1, j = f; i <= 6; ++i)
        attacks |= (1ULL << ((i * 8) + j));
    for (i = r - 1, j = f; i >= 1; --i)
        attacks |= (1ULL << ((i * 8) + j));
    for (i = r, j = f + 1; j <= 6; ++j)
        attacks |= (1ULL << ((i * 8) + j));
    for (i = r, j = f - 1; j >= 1; --j)
        attacks |= (1ULL << ((i * 8) + j));

    return attacks;
}

u64 generate_bishop_attacks(int square, u64 block)
{
    u64 attacks  = 0ULL;
    u64 bitboard = 0ULL;
    set_bit(bitboard, square);

    int i, j = 0; /* rank, file */
    int r, f = 0; /* target rank, file */

    r = square / 8;
    f = square % 8;

    for (i = r + 1, j = f + 1; i <= 6 && j <= 6; ++i, ++j) {
        attacks |= (1ULL << ((i * 8) + j));
        if ((1ULL << ((i * 8) + j)) & block)
            break;
    }
    for (i = r - 1, j = f + 1; i >= 1 && j <= 6; --i, ++j) {
        attacks |= (1ULL << ((i * 8) + j));
        if ((1ULL << ((i * 8) + j)) & block)
            break;
    }
    for (i = r + 1, j = f - 1; i <= 6 && j >= 1; ++i, --j) {
        attacks |= (1ULL << ((i * 8) + j));
        if ((1ULL << ((i * 8) + j)) & block)
            break;
    }
    for (i = r - 1, j = f - 1; i >= 1 && j >= 1; --i, --j) {
        attacks |= (1ULL << ((i * 8) + j));
        if ((1ULL << ((i * 8) + j)) & block)
            break;
    }

    return attacks;
}

u64 generate_rook_attacks(int square, u64 block)
{
    u64 attacks  = 0ULL;
    u64 bitboard = 0ULL;
    set_bit(bitboard, square);

    int i, j = 0; /* rank, file */
    int r, f = 0; /* target rank, file */

    r = square / 8;
    f = square % 8;

    for (i = r + 1, j = f; i <= 6; ++i) {
        attacks |= (1ULL << ((i * 8) + j));
        if ((1ULL << ((i * 8) + f)) & block)
            break;
    }
    for (i = r - 1, j = f; i >= 1; --i) {
        attacks |= (1ULL << ((i * 8) + j));
        if ((1ULL << ((i * 8) + f)) & block)
            break;
    }
    for (i = r, j = f + 1; j <= 6; ++j) {
        attacks |= (1ULL << ((i * 8) + j));
        if ((1ULL << ((r * 8) + j)) & block)
            break;
    }
    for (i = r, j = f - 1; j >= 1; --j) {
        attacks |= (1ULL << ((i * 8) + j));
        if ((1ULL << ((r * 8) + j)) & block)
            break;
    }

    return attacks;
}

u64 set_positions(int idx, int bit_count, u64 mask)
{
    u64 positions = 0ULL;

    for (int i = 0; i < bit_count; ++i) {
        int square = get_lsb_index(mask);
        pop_bit(mask, square);
        if (idx & (1 << i))
            positions |= (1ULL << square);
    }

    return positions;
}

/******************************************************************************
 * Magic
 ******************************************************************************/

/* clang-format off */
const int bishop_bits[64] = {
    6,  5,  5,  5,  5,  5,  5,  6,
    5,  5,  5,  5,  5,  5,  5,  5,
    5,  5,  7,  7,  7,  7,  5,  5,
    5,  5,  7,  9,  9,  7,  5,  5,
    5,  5,  7,  9,  9,  7,  5,  5,
    5,  5,  7,  7,  7,  7,  5,  5,
    5,  5,  5,  5,  5,  5,  5,  5,
    6,  5,  5,  5,  5,  5,  5,  6
};

const int rook_bits[64] = {
    12,  11,  11,  11,  11,  11,  11,  12,
    11,  10,  10,  10,  10,  10,  10,  11,
    11,  10,  10,  10,  10,  10,  10,  11,
    11,  10,  10,  10,  10,  10,  10,  11,
    11,  10,  10,  10,  10,  10,  10,  11,
    11,  10,  10,  10,  10,  10,  10,  11,
    11,  10,  10,  10,  10,  10,  10,  11,
    12,  11,  11,  11,  11,  11,  11,  12
};

const u64 bishop_magics[64] = {
    599053590312779776ULL,          4652220897632452608ULL,
    9297781843822592ULL,            2537690222821384ULL,
    2306485124072472768ULL,         9232704725927854210ULL,
    1162201006089216ULL,            4755836614282838016ULL,
    2486094832464958464ULL,         360609595127119872ULL,
    293864346847545346ULL,          576610291694190596ULL,
    585470288089874432ULL,          56296112570769408ULL,
    292736180440007680ULL,          2305852906026445057ULL,
    9223372140370767872ULL,         36099234558201868ULL,
    72092851492651008ULL,           288240136473313281ULL,
    70378944807040ULL,              1170935921941418018ULL,
    4692770620193374243ULL,         490892635352334529ULL,
    20270615160537090ULL,           9229005934486905857ULL,
    2268885942272ULL,               11529782394739492896ULL,
    9800959788579815426ULL,         9007750639005698ULL,
    2314859830841966656ULL,         378309034624876552ULL,
    2972432946118656016ULL,         729883925093548036ULL,
    9511624406464725008ULL,         1161928723189465105ULL,
    4611686294451126272ULL,         4726529355248369666ULL,
    2630104399663489040ULL,         3499297481698215941ULL,
    21393215199117332ULL,           4611687122779246620ULL,
    36029627026636809ULL,           576513542350522368ULL,
    2361027635823509505ULL,         10449485900370444290ULL,
    36032275959250976ULL,           5476386771908428038ULL,
    4971978392179056640ULL,         9035793067410688ULL,
    576480544888594976ULL,          27022078805540864ULL,
    225215741803700768ULL,          288373347728228390ULL,
    4611831158597025796ULL,         288372252896985218ULL,
    11529215681734116352ULL,        162729197473285ULL,
    72057869217861642ULL,           4415227184160ULL,
    1155173308715828292ULL,         1224979107772238113ULL,
    9261934831330165248ULL,         40134875562112ULL,
};

const u64 rook_magics[64] = {
    4612820989575692288ULL,         288250168501870898ULL,
    292736180441579520ULL,          864711194677870592ULL,
    288241441078247456ULL,          1441170593966720000ULL,
    432354426959822913ULL,          4503754566041600ULL,
    73184052307558412ULL,           1126466859311648ULL,
    726205513129279616ULL,          289356430819999748ULL,
    615164128537346048ULL,          564083896135680ULL,
    2333163691857481728ULL,         2882866712688002082ULL,
    9295436571568242698ULL,         1152922054899548260ULL,
    216177197885456386ULL,          2310356504648679424ULL,
    197912160113032ULL,             864691678244504640ULL,
    11529848512942507008ULL,        2305948584077494272ULL,
    2305843157926952960ULL,         4683744918205630528ULL,
    276086194176ULL,                11817445989157175306ULL,
    4503960945754112ULL,            1585267077425399812ULL,
    9322451272144522240ULL,         18014407099433252ULL,
    1729945349135011856ULL,         9223372174574747714ULL,
    36028934474711044ULL,           4611686293574909952ULL,
    3458764532077441024ULL,         3535325716243558400ULL,
    580964420706894848ULL,          2306407355165773824ULL,
    144133408417906688ULL,          576462952418377826ULL,
    144115326052745217ULL,          1166731375634612228ULL,
    43033624576ULL,                 4611686035615678464ULL,
    5836666218735477760ULL,         9295992602320994416ULL,
    9352295491072ULL,               617002093290586240ULL,
    10996727030304ULL,              1152921676674236480ULL,
    9511602979959472144ULL,         7063896024456103968ULL,
    18014407637338114ULL,           9245929618500788736ULL,
    72057871065483286ULL,           10448351208523490308ULL,
    9223372073366454540ULL,         288230411065393476ULL,
    9127346196ULL,                  275165283336ULL,
    2467972750426390529ULL,         158259584897155106ULL,
};
/* clang-format on */

u64 state[] = {
        8392127718274466268ULL,
};

u64 xorshift64(void)
{
    u64 x   = *state;
    x      ^= x << 13;
    x      ^= x >> 7;
    x      ^= x << 17;
    *state  = x;
    return x;
}

u64 rand_u64(void)
{
    u64 w, x, y, z = 0ULL;
    w = xorshift64() & 0xFFFF;
    x = xorshift64() & 0xFFFF;
    y = xorshift64() & 0xFFFF;
    z = xorshift64() & 0xFFFF;
    return w | (x << 16) | (y << 32) | (z << 48);
}

static inline u64 rand_bits(int n)
{
    u64 r = rand_u64();
    for (int i = 0; i < n - 1; ++i) {
        r &= rand_u64();
    }
    return r;
}

u64 find_magic(int square, int bits, int piece)
{
    u64 mask, magic = 0;
    u64 positions[4096]    = {0ULL};
    u64 attacks[4096]      = {0ULL};
    u64 used_attacks[4096] = {0ULL};

    if (piece != bishop && piece != rook)
        return 0ULL;

    mask                 = (piece == bishop ? mask_bishop_attacks(square)
                                            : mask_rook_attacks(square));

    int position_indexes = 1 << bits;

    for (int i = 0; i < position_indexes; ++i) {
        positions[i] = set_positions(i, bits, mask);
        attacks[i] =
                (piece == bishop ? generate_bishop_attacks(square, positions[i])
                                 : generate_rook_attacks(square, positions[i]));
    }

    for (int i = 0; i < 100000000; ++i) {
        magic = rand_bits(3);
        if (count_bits((mask * magic) & 0xFF00000000000000ULL) < 6)
            continue;
        memset(used_attacks, 0ULL, sizeof(u64) * 4096);

        int fail = 0;
        for (int index = 0; index < position_indexes; ++index) {
            int magic_idx = ( int )((positions[index] * magic) >> (64 - bits));
            if (used_attacks[magic_idx] == 0ULL) {
                used_attacks[magic_idx] = attacks[index];
            } else if (used_attacks[magic_idx] != attacks[index]) {
                fail = 1;
            }
            if (fail)
                break;
        }

        if (!fail)
            return magic;
    }

    return 0ULL;
}

void init_slider_attacks(int piece)
{
    if (piece != bishop && piece != rook)
        return;

    for (int i = 0; i < 64; ++i) {
        bishop_masks[i] = mask_bishop_attacks(i);
        rook_masks[i]   = mask_rook_attacks(i);

        u64 attack_mask = (piece == bishop ? bishop_masks[i] : rook_masks[i]);
        int bits        = count_bits(attack_mask);
        int position_indexes = 1 << bits;

        for (int j = 0; j < position_indexes; ++j) {
            u64 position = set_positions(j, bits, attack_mask);
            if (piece == bishop) {
                int magic_idx =
                        (position * bishop_magics[i]) >> (64 - bishop_bits[i]);
                bishop_attacks[i][magic_idx] =
                        generate_bishop_attacks(i, position);
            } else {
                int magic_idx =
                        (position * rook_magics[i]) >> (64 - rook_bits[i]);
                rook_attacks[i][magic_idx] = generate_rook_attacks(i, position);
            }
        }
    }
}

u64 get_bishop_attacks(int square, u64 position)
{
    position  &= bishop_masks[square];
    position  *= bishop_magics[square];
    position >>= 64 - bishop_bits[square];
    return bishop_attacks[square][position];
}

u64 get_rook_attacks(int square, u64 position)
{
    position  &= rook_masks[square];
    position  *= rook_magics[square];
    position >>= 64 - rook_bits[square];
    return rook_attacks[square][position];
}

/******************************************************************************
 * Inits
 ******************************************************************************/

void init_all(void)
{
    init_slider_attacks(bishop);
    init_slider_attacks(rook);
}

void init_board(void)
{
    for (int i = 0; i < 12; ++i) {
        u64 bitboard = 0ULL;
        switch (i) {
        case P: /* Pawn - White */
            for (int j = 0; j < 8; ++j)
                bitboards[i] |= (1ULL << ((1 * 8) + j));
            break;
        case N: /* Knight - White */
            bitboards[i] = (1ULL << 1) | (1ULL << 6);
            break;
        case B: /* Bishops - White */
            bitboards[i] = (1ULL << 2) | (1ULL << 5);
            break;
        case R: /* Rooks - White */
            bitboards[i] = (1ULL << 0) | (1ULL << 7);
            break;
        case Q: /* Queen - White */
            bitboards[i] = (1ULL << 3);
            break;
        case K: /* King - White */
            bitboards[i] = (1ULL << 4);
            break;
        case p: /* Pawn - Black */
            for (int j = 0; j < 8; ++j)
                bitboards[i] |= (1ULL << ((6 * 8) + j));
            break;
        case n: /* Knight - Black */
            bitboards[i] = (1ULL << ((7 * 8) + 1)) | (1ULL << ((7 * 8) + 6));
            break;
        case b: /* Bishops - Black */
            bitboards[i] = (1ULL << ((7 * 8) + 2)) | (1ULL << ((7 * 8) + 5));
            break;
        case r: /* Rooks - Black */
            bitboards[i] = (1ULL << ((7 * 8) + 0)) | (1ULL << ((7 * 8) + 7));
            break;
        case q: /* Queen - Black */
            bitboards[i] = (1ULL << ((7 * 8) + 3));
            break;
        case k: /* King - Black */
            bitboards[i] = (1ULL << ((7 * 8) + 4));
            break;
        }
    }

    side              = white;

    positions[white]  = 0x000000000000FFFFULL;
    positions[black]  = 0xFFFF000000000000ULL;
    positions[both]   = 0xFFFF00000000FFFFULL;

    enpassant         = no_sq;

    castle           |= WKCK | WKCQ | BKCK | BKCQ;

    moves             = 1;

    return;
}

/* vim: ft=c ts=4 sts=4 sw=4 ai et cin */
