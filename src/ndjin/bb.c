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

#include "types.h"
#include "bb.h"

////////////////////////////////////////////////////////////////////////////////
//                                 Extern                                     //
////////////////////////////////////////////////////////////////////////////////

extern const char *square_to_coord[64];
extern const char  ascii_pieces[13];
extern const char *unicode_pieces[12];
extern const char  promoted_pieces[];
extern const int   char_pieces[];
extern const char  piece_char[];

////////////////////////////////////////////////////////////////////////////////
//                                 Board                                      //
////////////////////////////////////////////////////////////////////////////////

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

struct state_t game_state;

////////////////////////////////////////////////////////////////////////////////
//                                   Time                                     //
////////////////////////////////////////////////////////////////////////////////

/* exit from engine */
int quit    = 0;

/* flag time control availability */
int timeset = 0;

/* flag time is out */
int stopped = 0;

////////////////////////////////////////////////////////////////////////////////
//                                 Functions                                  //
////////////////////////////////////////////////////////////////////////////////

static inline int get_time_ms(void)
{
    struct timeval time_value;
    gettimeofday(&time_value, NULL);
    return time_value.tv_sec * 1000 + time_value.tv_usec / 1000;
}

static inline int count_bits(u64 bitboard)
{
    unsigned int count = 0;
    for (count = 0; bitboard; ++count)
        bitboard &= bitboard - 1;
    return count;
}

static inline int get_lsb_index(u64 bitboard)
{
    if (bitboard)
        return count_bits((bitboard & -bitboard) - 1);
    else
        return -1;
}

////////////////////////////////////////////////////////////////////////////////
//                                Printing                                    //
////////////////////////////////////////////////////////////////////////////////

static inline void print_tiles(void)
{
    for (int i = 7; i >= 0; --i) {
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

static inline void print_bitboard(u64 bitboard)
{
    for (int i = 7; i >= 0; --i) {
        for (int j = 0; j < 8; ++j) {
            int sq = (i * 8) + j;
            if (!j)
                printf("  %d ", i + 1);
            printf(" %d", get_bit(bitboard, sq) ? 1 : 0);
        }
        puts("");
    }
    puts("\n     a b c d e f g h\n");
    printf("Bitboard: %llu (%d : %d)\n\n", bitboard, get_lsb_index(bitboard),
           count_bits(bitboard));
}

static inline void print_attacked(int side)
{
    for (int i = 7; i >= 0; --i) {
        for (int j = 0; j < 8; ++j) {
            int sq = (i * 8) + j;
            if (!j)
                printf("  %d ", i + 1);
            printf(" %d", get_attacked(&game_state, sq, side) ? 1 : 0);
        }
        puts("");
    }
    puts("\n     a b c d e f g h\n");
    printf("Attacked by:\t%s\n", (side == white ? "white" : "black"));
}

static inline void print_board(int unicode)
{
    for (int i = 7; i >= 0; --i) {
        for (int j = 0; j < 8; ++j) {
            int sq    = (i * 8) + j;
            int piece = -1;
            for (int k = 0; k < 12; ++k) {
                if (get_bit(game_state.bitboards[k], sq) > 0)
                    piece = k;
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
    printf("Side:\t\t%s\n", (game_state.side == white ? "white" : "black"));
    printf("En-passant:\t%s\n",
           (game_state.enpassant == no_sq
                    ? "none"
                    : square_to_coord[game_state.enpassant]));
    printf("Castling:\t%c%c%c%c\n", (game_state.castle & WKCK) ? 'K' : '.',
           (game_state.castle & WKCQ) ? 'Q' : '.',
           (game_state.castle & BKCK) ? 'k' : '.',
           (game_state.castle & BKCQ) ? 'q' : '.');
    printf("Half moves:\t%d\n", game_state.ply);
    printf("Moves:\t\t%d\n", game_state.fullmoves);
}

static inline void print_move(unsigned int move, int unicode)
{
    int source, target, piece, promo;
    DECODE_MOVE(move, &source, &target, &piece, &promo);
    printf("%s%s    ", square_to_coord[source], square_to_coord[target]);
    if (!unicode)
        printf("%c      -> %c\t\t", ascii_pieces[piece], ascii_pieces[promo]);
    else
        printf("%s      -> %s\t\t", unicode_pieces[piece],
               unicode_pieces[promo]);
    printf("%d       %d      %d        %d\n", (MOVE_CAPTURE_FLAG(move) ? 1 : 0),
           (MOVE_DOUBLE_FLAG(move) ? 1 : 0), (MOVE_PASSANT_FLAG(move) ? 1 : 0),
           (MOVE_CASTLE_FLAG(move) ? 1 : 0));
    return;
}

static inline void print_move_list(struct move_list_t *moves, int unicode)
{
    printf("\nmove    piece (-> promo.) \tcapture double en-pass. castle\n");
    for (int i = 0; i < 64; ++i)
        for (int j = 0; j < moves->squares[i].count; ++j)
            print_move(moves->squares[i].moves[j], unicode);
    printf("Total move count: \t%d\n", moves->count);
}

////////////////////////////////////////////////////////////////////////////////
//                                Attack Tables                               //
////////////////////////////////////////////////////////////////////////////////

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
            512ULL,                     1280ULL,
            2560ULL,                    5120ULL,
            10240ULL,                   20480ULL,
            40960ULL,                   16384ULL,
            131072ULL,                  327680ULL,
            655360ULL,                  1310720ULL,
            2621440ULL,                 5242880ULL,
            10485760ULL,                4194304ULL,
            33554432ULL,                83886080ULL,
            167772160ULL,               335544320ULL,
            671088640ULL,               1342177280ULL,
            2684354560ULL,              1073741824ULL,
            8589934592ULL,              21474836480ULL,
            42949672960ULL,             85899345920ULL,
            171798691840ULL,            343597383680ULL,
            687194767360ULL,            274877906944ULL,
            2199023255552ULL,           5497558138880ULL,
            10995116277760ULL,          21990232555520ULL,
            43980465111040ULL,          87960930222080ULL,
            175921860444160ULL,         70368744177664ULL,
            562949953421312ULL,         1407374883553280ULL,
            2814749767106560ULL,        5629499534213120ULL,
            11258999068426240ULL,       22517998136852480ULL,
            45035996273704960ULL,       18014398509481984ULL,
            144115188075855872ULL,      360287970189639680ULL,
            720575940379279360ULL,      1441151880758558720ULL,
            2882303761517117440ULL,     5764607523034234880ULL,
            11529215046068469760ULL,    4611686018427387904ULL,
            0ULL,                       0ULL,
            0ULL,                       0ULL,
            0ULL,                       0ULL,
            0ULL,                       0ULL,
        },

        /* black attacks */
        {
            0ULL,                       0ULL,
            0ULL,                       0ULL,
            0ULL,                       0ULL,
            0ULL,                       0ULL,
            2ULL,                       5ULL,
            10ULL,                      20ULL,
            40ULL,                      80ULL,
            160ULL,                     64ULL,
            512ULL,                     1280ULL,
            2560ULL,                    5120ULL,
            10240ULL,                   20480ULL,
            40960ULL,                   16384ULL,
            131072ULL,                  327680ULL,
            655360ULL,                  1310720ULL,
            2621440ULL,                 5242880ULL,
            10485760ULL,                4194304ULL,
            33554432ULL,                83886080ULL,
            167772160ULL,               335544320ULL,
            671088640ULL,               1342177280ULL,
            2684354560ULL,              1073741824ULL,
            8589934592ULL,              21474836480ULL,
            42949672960ULL,             85899345920ULL,
            171798691840ULL,            343597383680ULL,
            687194767360ULL,            274877906944ULL,
            2199023255552ULL,           5497558138880ULL,
            10995116277760ULL,          21990232555520ULL,
            43980465111040ULL,          87960930222080ULL,
            175921860444160ULL,         70368744177664ULL,
            562949953421312ULL,         1407374883553280ULL,
            2814749767106560ULL,        5629499534213120ULL,
            11258999068426240ULL,       22517998136852480ULL,
            45035996273704960ULL,       18014398509481984ULL,
        }
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

static inline u64 mask_pawn_attacks(int square, int side)
{
    u64 attacks  = 0ULL;
    u64 bitboard = 0ULL;
    set_bit(bitboard, square);

    if (!side) {
        if ((bitboard << 7) & not_h_file)
            attacks |= (bitboard << 7);
        if ((bitboard << 9) & not_a_file)
            attacks |= (bitboard << 9);
    } else {
        if ((bitboard >> 7) & not_a_file)
            attacks |= (bitboard >> 7);
        if ((bitboard >> 9) & not_h_file)
            attacks |= (bitboard >> 9);
    }

    return attacks;
}

static inline u64 mask_knight_attacks(int square)
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

static inline u64 mask_king_attacks(int square)
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

static inline u64 mask_bishop_attacks(int square)
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

static inline u64 mask_rook_attacks(int square)
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

static inline u64 generate_bishop_attacks(int square, u64 block)
{
    u64 attacks  = 0ULL;
    u64 bitboard = 0ULL;
    set_bit(bitboard, square);

    int i, j = 0; /* rank, file */
    int r, f = 0; /* target rank, file */

    r = square / 8;
    f = square % 8;

    for (i = r + 1, j = f + 1; i <= 7 && j <= 7; ++i, ++j) {
        attacks |= (1ULL << ((i * 8) + j));
        if ((1ULL << ((i * 8) + j)) & block)
            break;
    }
    for (i = r - 1, j = f + 1; i >= 0 && j <= 7; --i, ++j) {
        attacks |= (1ULL << ((i * 8) + j));
        if ((1ULL << ((i * 8) + j)) & block)
            break;
    }
    for (i = r + 1, j = f - 1; i <= 7 && j >= 0; ++i, --j) {
        attacks |= (1ULL << ((i * 8) + j));
        if ((1ULL << ((i * 8) + j)) & block)
            break;
    }
    for (i = r - 1, j = f - 1; i >= 0 && j >= 0; --i, --j) {
        attacks |= (1ULL << ((i * 8) + j));
        if ((1ULL << ((i * 8) + j)) & block)
            break;
    }

    return attacks;
}

static inline u64 generate_rook_attacks(int square, u64 block)
{
    u64 attacks  = 0ULL;
    u64 bitboard = 0ULL;
    set_bit(bitboard, square);

    int i, j = 0; /* rank, file */
    int r, f = 0; /* target rank, file */

    r = square / 8;
    f = square % 8;

    for (i = r + 1, j = f; i <= 7; ++i) {
        attacks |= (1ULL << ((i * 8) + j));
        if ((1ULL << ((i * 8) + f)) & block)
            break;
    }
    for (i = r - 1, j = f; i >= 0; --i) {
        attacks |= (1ULL << ((i * 8) + j));
        if ((1ULL << ((i * 8) + f)) & block)
            break;
    }
    for (i = r, j = f + 1; j <= 7; ++j) {
        attacks |= (1ULL << ((i * 8) + j));
        if ((1ULL << ((r * 8) + j)) & block)
            break;
    }
    for (i = r, j = f - 1; j >= 0; --j) {
        attacks |= (1ULL << ((i * 8) + j));
        if ((1ULL << ((r * 8) + j)) & block)
            break;
    }

    return attacks;
}

static inline u64 set_positions(int idx, int bit_count, u64 mask)
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

////////////////////////////////////////////////////////////////////////////////
//                                    Magic                                   //
////////////////////////////////////////////////////////////////////////////////

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
    598988671884197920ULL,          9011877149474820ULL,
    20284361773416448ULL,           10137636828106752ULL,
    72356936078589952ULL,           2324421474939568129ULL,
    13979473444929339493ULL,        6941806553628942370ULL,
    36314704436790308ULL,           612859053982367814ULL,
    9223380867849158789ULL,         2476065423489ULL,
    5188288677525192864ULL,         146369195538386960ULL,
    1441451227162361856ULL,         576460892024604936ULL,
    9007233760231556ULL,            577165132510070272ULL,
    2310944760413814792ULL,         9226901503676055552ULL,
    1161092902486016ULL,            142971005634563ULL,
    281758462378754ULL,             72620551675707776ULL,
    22590565971657728ULL,           1878010940256420096ULL,
    11261198109769984ULL,           9232388040809267208ULL,
    4629982235527856130ULL,         2310347709434839360ULL,
    15763148736862400ULL,           10448510616227350528ULL,
    1160948507536789504ULL,         649662534236242432ULL,
    9027583313183746ULL,            1128101078106240ULL,
    4756663275160174624ULL,         18588352169379968ULL,
    9560161262802047232ULL,         45247660852019713ULL,
    9556796812239587328ULL,         576621316975665672ULL,
    1317373543763177476ULL,         288230522250070017ULL,
    2383600540400493089ULL,         4629736254420844608ULL,
    2326188374664186888ULL,         1310618412766003328ULL,
    9944512112913090568ULL,         1166573367663591552ULL,
    18025488132087808ULL,           1154047405059479552ULL,
    10133238817488896ULL,           9042392250451968ULL,
    1156160734615437504ULL,         4505833014854656ULL,
    11531486639308423168ULL,        1170936179136137216ULL,
    603482350621820928ULL,          9223372586614919684ULL,
    580973148325937664ULL,          13835112206498662792ULL,
    4535519150600ULL,               616995556415799552ULL,
};

const u64 rook_magics[64] = {
    9259400997359468545ULL,         162129655342669832ULL,
    144132917868642944ULL,          324264121241438209ULL,
    4791832752368396288ULL,         144117389381075216ULL,
    4791840998646874368ULL,         144119590426255426ULL,
    36732494124580912ULL,           612630837640306688ULL,
    5478206871678488576ULL,         1266706190174208ULL,
    36733034350773248ULL,           141287311278592ULL,
    149463221223559424ULL,          74591007357796482ULL,
    36029072433758216ULL,           18019071971819584ULL,
    1153066090654605314ULL,         18025393929863680ULL,
    1127003714488320ULL,            22526799600682000ULL,
    4901196226734739480ULL,         9259684507877974148ULL,
    18155279879389184ULL,           1819471842721669120ULL,
    9715568211927556ULL,            36037595267862528ULL,
    398569118926111746ULL,          1130300100968576ULL,
    8813272957456ULL,               721711194724914185ULL,
    153192756955578496ULL,          18023263324078112ULL,
    4503670502727680ULL,            292753594388512ULL,
    2251973801806848ULL,            2455024815623374088ULL,
    15709683667985008641ULL,        882709926118424737ULL,
    1167699079397539840ULL,         35187861831681ULL,
    13591063767875600ULL,           2305860605696868360ULL,
    1593152766363074688ULL,         577023719445135362ULL,
    2401337723846736ULL,            9022055685160964ULL,
    3476779196343124352ULL,         18084767791580224ULL,
    9015996422553664ULL,            162270358434480256ULL,
    1170940335523692800ULL,         576742244460528384ULL,
    3395946925294592ULL,            4683762448066150912ULL,
    11673789834304426626ULL,        4962984665024626730ULL,
    109845747117066257ULL,          1298180186992345097ULL,
    5067685870191618ULL,            1155454796577245185ULL,
    90143600658229252ULL,           18708888469634ULL,
};
/* clang-format on */

u64 state[] = {
        8392127718274466268ULL,
};

static inline u64 xorshift64(void)
{
    u64 x   = *state;
    x      ^= x << 13;
    x      ^= x >> 7;
    x      ^= x << 17;
    *state  = x;
    return x;
}

static inline u64 rand_u64(void)
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

static inline u64 find_magic(int square, int bits, int piece)
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

static inline void init_slider_attacks(int piece)
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

static inline u64 get_bishop_attacks(int square, u64 position)
{
    position  &= bishop_masks[square];
    position  *= bishop_magics[square];
    position >>= 64 - bishop_bits[square];
    return bishop_attacks[square][position];
}

static inline u64 get_rook_attacks(int square, u64 position)
{
    position  &= rook_masks[square];
    position  *= rook_magics[square];
    position >>= 64 - rook_bits[square];
    return rook_attacks[square][position];
}

static inline u64 get_queen_attacks(int square, u64 position)
{
    u64 result       = 0ULL;
    u64 bishop_pos   = position;
    u64 rook_pos     = position;

    bishop_pos      &= bishop_masks[square];
    bishop_pos      *= bishop_magics[square];
    bishop_pos     >>= 64 - bishop_bits[square];
    result          |= bishop_attacks[square][bishop_pos];

    rook_pos        &= rook_masks[square];
    rook_pos        *= rook_magics[square];
    rook_pos       >>= 64 - rook_bits[square];
    result          |= rook_attacks[square][rook_pos];

    return result;
}

////////////////////////////////////////////////////////////////////////////////
//                                     Inits                                  //
////////////////////////////////////////////////////////////////////////////////

void init_all(void)
{
    init_slider_attacks(bishop);
    init_slider_attacks(rook);
}

void init_board(void)
{
    for (int i = 0; i < 12; ++i) {
        switch (i) {
        case P: /* Pawn - White */
            for (int j = 0; j < 8; ++j)
                game_state.bitboards[i] |= (1ULL << ((1 * 8) + j));
            break;
        case N: /* Knight - White */
            game_state.bitboards[i] = (1ULL << 1) | (1ULL << 6);
            break;
        case B: /* Bishops - White */
            game_state.bitboards[i] = (1ULL << 2) | (1ULL << 5);
            break;
        case R: /* Rooks - White */
            game_state.bitboards[i] = (1ULL << 0) | (1ULL << 7);
            break;
        case Q: /* Queen - White */
            game_state.bitboards[i] = (1ULL << 3);
            break;
        case K: /* King - White */
            game_state.bitboards[i] = (1ULL << 4);
            break;
        case p: /* Pawn - Black */
            for (int j = 0; j < 8; ++j)
                game_state.bitboards[i] |= (1ULL << ((6 * 8) + j));
            break;
        case n: /* Knight - Black */
            game_state.bitboards[i] =
                    (1ULL << ((7 * 8) + 1)) | (1ULL << ((7 * 8) + 6));
            break;
        case b: /* Bishops - Black */
            game_state.bitboards[i] =
                    (1ULL << ((7 * 8) + 2)) | (1ULL << ((7 * 8) + 5));
            break;
        case r: /* Rooks - Black */
            game_state.bitboards[i] =
                    (1ULL << ((7 * 8) + 0)) | (1ULL << ((7 * 8) + 7));
            break;
        case q: /* Queen - Black */
            game_state.bitboards[i] = (1ULL << ((7 * 8) + 3));
            break;
        case k: /* King - Black */
            game_state.bitboards[i] = (1ULL << ((7 * 8) + 4));
            break;
        }
    }

    game_state.side             = white;

    game_state.positions[white] = 0x000000000000FFFFULL;
    game_state.positions[black] = 0xFFFF000000000000ULL;
    game_state.positions[both]  = 0xFFFF00000000FFFFULL;

    game_state.enpassant        = no_sq;

    game_state.castle           = WKCK | WKCQ | BKCK | BKCQ;

    game_state.ply              = 0;

    game_state.fifty            = 0;

    game_state.fullmoves        = 1;

    return;
}

////////////////////////////////////////////////////////////////////////////////
//                                 Moves                                      //
////////////////////////////////////////////////////////////////////////////////

static inline void add_move(struct move_list_t *moves, unsigned int move)
{
    moves->squares[move & 63].moves[moves->squares[move & 63].count++] = move;
    ++moves->count;
    return;
}

int get_attacked(struct state_t *state, int square, int side)
{
    if (state->side < white && state->side > black)
        return 0;

    if (pawn_attacks[state->side][square] &
        ((side == white) ? state->bitboards[P] : state->bitboards[p]))
        return 1;
    if (knight_attacks[square] &
        ((side == white) ? state->bitboards[N] : state->bitboards[n]))
        return 1;
    if (king_attacks[square] &
        ((side == white) ? state->bitboards[K] : state->bitboards[k]))
        return 1;
    if (get_bishop_attacks(square, state->positions[both]) &
        ((side == white) ? state->bitboards[B] : state->bitboards[b]))
        return 1;
    if (get_rook_attacks(square, state->positions[both]) &
        ((side == white) ? state->bitboards[R] : state->bitboards[r]))
        return 1;
    if (get_queen_attacks(square, state->positions[both]) &
        ((side == white) ? state->bitboards[Q] : state->bitboards[q]))
        return 1;

    return 0;
}

/* Castling rights update
 * kings + rooks didn't move:       1111 & 1111 = 1111 15
 *
 * white king moved:                1111 & 1100 = 1100 12
 * white king's rook moved:         1111 & 1110 = 1110 14
 * white queen's rook moved:        1111 & 1101 = 1101 13
 *
 * black king moved:                1111 & 0011 = 0011 3
 * black king's rook moved:         1111 & 1011 = 1110 11
 * black queen's rook moved:        1111 & 0111 = 1101 7
 */
/* clang-format off */
const int castling_rights[64] = {
    13, 15, 15, 15, 12, 15, 15, 14,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    7, 15, 15, 15, 3, 15, 15, 11,
};
/* clang-format on */

int make_move(unsigned int move, int move_flag)
{
    struct state_t backup_state = {0};
    if (move_flag == all_moves) {
        BOARD_BACKUP(&game_state, &backup_state);

        int source, target, piece, promo;
        DECODE_MOVE(move, &source, &target, &piece, &promo);
        int capture  = MOVE_CAPTURE_FLAG(move) ? 1 : 0;
        int dpush    = MOVE_DOUBLE_FLAG(move) ? 1 : 0;
        int epass    = MOVE_PASSANT_FLAG(move) ? 1 : 0;
        int castling = MOVE_CASTLE_FLAG(move) ? 1 : 0;

        pop_bit(game_state.bitboards[piece], source);
        set_bit(game_state.bitboards[piece], target);
        pop_bit(game_state.positions[game_state.side], source);
        set_bit(game_state.positions[game_state.side], target);

        if (capture) {
            int start_piece, end_piece;
            if (game_state.side == white) {
                start_piece = p;
                end_piece   = k;
            } else {
                start_piece = P;
                end_piece   = K;
            }
            for (int i = start_piece; i <= end_piece; ++i) {
                if (get_bit(game_state.bitboards[i], target)) {
                    pop_bit(game_state.bitboards[i], target);
                    pop_bit(game_state.positions[1 - game_state.side], target);
                    break;
                }
            }
        }

        if (promo > piece) {
            pop_bit(game_state.bitboards[piece], target);
            set_bit(game_state.bitboards[promo], target);
        }

        if (epass) {
            if (game_state.side == white) {
                pop_bit(game_state.bitboards[p], target - 8);
                pop_bit(game_state.positions[black], target - 8);
            } else {
                pop_bit(game_state.bitboards[p], target + 8);
                pop_bit(game_state.positions[white], target + 8);
            }
        }
        game_state.enpassant = no_sq;

        if (dpush) {
            if (game_state.side == white)
                game_state.enpassant = target - 8;
            else
                game_state.enpassant = target + 8;
        }

        if (castling) {
            switch (target) {
            case g1:
                pop_bit(game_state.bitboards[R], h1);
                set_bit(game_state.bitboards[R], f1);
                pop_bit(game_state.positions[white], h1);
                set_bit(game_state.positions[white], f1);
                break;
            case c1:
                pop_bit(game_state.bitboards[R], a1);
                set_bit(game_state.bitboards[R], d1);
                pop_bit(game_state.positions[white], a1);
                set_bit(game_state.positions[white], d1);
                break;
            case g8:
                pop_bit(game_state.bitboards[r], h8);
                set_bit(game_state.bitboards[r], f8);
                pop_bit(game_state.positions[black], h8);
                set_bit(game_state.positions[black], f8);
                break;
            case c8:
                pop_bit(game_state.bitboards[r], a8);
                set_bit(game_state.bitboards[r], d8);
                pop_bit(game_state.positions[black], a8);
                set_bit(game_state.positions[black], d8);
                break;
            }
        }
        game_state.castle          &= castling_rights[source];
        game_state.castle          &= castling_rights[target];

        game_state.positions[both]  = 0ULL | game_state.positions[white] |
                                     game_state.positions[black];

        game_state.side ^= 1;

        if (get_attacked(&game_state,
                         get_lsb_index((game_state.side == white
                                                ? game_state.bitboards[k]
                                                : game_state.bitboards[K])),
                         game_state.side)) {
            BOARD_RESTORE(&backup_state, &game_state);
            return 0;
        } else if (get_attacked(
                           &game_state,
                           get_lsb_index((game_state.side == white
                                                  ? game_state.bitboards[K]
                                                  : game_state.bitboards[k])),
                           1 - game_state.side)) {
            game_state.check =
                    (game_state.side == white) ? white_check : black_check;
        } else
            game_state.check = no_check;

        ++game_state.fullmoves;

        return 1;
    } else {
        if (MOVE_CAPTURE_FLAG(move))
            make_move(move, all_moves);
        else
            return 0;
    }
    return 0;
}

void generate_moves(struct move_list_t *list)
{
    if (!list)
        return;

    u64 bitboard = 0ULL;
    u64 attacks  = 0ULL;

    int source, target = 0;

    list->count = 0;

    for (int piece = P; piece <= k; ++piece) {
        bitboard = game_state.bitboards[piece];
        /* white pawns & white king castling */
        if (game_state.side == white) {
            if (piece == P) {
                while (bitboard) {
                    source = get_lsb_index(bitboard);
                    target = source + 8;

                    if ((target >= 0 && target < 64) &&
                        !get_bit(game_state.positions[both], target)) {
                        /* promotion */
                        if (source >= a7 && source <= h7) {
                            add_move(list, ENCODE_MOVE(source, target, piece, B,
                                                       0, 0, 0, 0));
                            add_move(list, ENCODE_MOVE(source, target, piece, R,
                                                       0, 0, 0, 0));
                            add_move(list, ENCODE_MOVE(source, target, piece, N,
                                                       0, 0, 0, 0));
                            add_move(list, ENCODE_MOVE(source, target, piece, Q,
                                                       0, 0, 0, 0));
                        } else {
                            if (!get_bit(game_state.positions[both], target)) {
                                add_move(list,
                                         ENCODE_MOVE(source, target, piece,
                                                     piece, 0, 0, 0, 0));
                                if ((source >= a2 && source <= h2) &&
                                    !get_bit(game_state.positions[both],
                                             target + 8)) {
                                    add_move(list,
                                             ENCODE_MOVE(source, (target + 8),
                                                         piece, piece, 0, 1, 0,
                                                         0));
                                }
                            }
                        }
                    }

                    attacks = pawn_attacks[game_state.side][source] &
                              game_state.positions[black];

                    while (attacks) {
                        target = get_lsb_index(attacks);

                        if ((target >= 0 && target < 64) &&
                            get_bit(game_state.positions[black], target)) {
                            /* capture promotion */
                            if (source >= a7 && source <= h7) {
                                add_move(list,
                                         ENCODE_MOVE(source, target, piece, B,
                                                     1, 0, 0, 0));
                                add_move(list,
                                         ENCODE_MOVE(source, target, piece, R,
                                                     1, 0, 0, 0));
                                add_move(list,
                                         ENCODE_MOVE(source, target, piece, K,
                                                     1, 0, 0, 0));
                                add_move(list,
                                         ENCODE_MOVE(source, target, piece, Q,
                                                     1, 0, 0, 0));
                            } else {
                                if (get_bit(game_state.positions[black],
                                            target)) {
                                    add_move(list,
                                             ENCODE_MOVE(source, target, piece,
                                                         piece, 1, 0, 0, 0));
                                }
                            }
                        }

                        pop_bit(attacks, target);
                    }

                    /* en passant */
                    if (game_state.enpassant != no_sq) {
                        u64 enpassant_attacks =
                                pawn_attacks[game_state.side][source] &
                                (1ULL << game_state.enpassant);
                        if (enpassant_attacks) {
                            int target_enpassant =
                                    get_lsb_index(enpassant_attacks);
                            add_move(list,
                                     ENCODE_MOVE(source, target_enpassant,
                                                 piece, piece, 1, 0, 1, 0));
                        }
                    }

                    pop_bit(bitboard, source);
                }
            }

            /* castling */
            if (piece == K) {
                if (game_state.castle & WKCK) {
                    if (!get_bit(game_state.positions[both], f1) &&
                        !get_bit(game_state.positions[both], g1)) {
                        if (!get_attacked(&game_state, e1, black) &&
                            !get_attacked(&game_state, f1, black)) {
                            add_move(list, ENCODE_MOVE(e1, g1, piece, piece, 0,
                                                       0, 0, 1));
                        }
                    }
                }
                if (game_state.castle & WKCQ) {
                    if (!get_bit(game_state.positions[both], d1) &&
                        !get_bit(game_state.positions[both], c1) &&
                        !get_bit(game_state.positions[both], b1)) {
                        if (!get_attacked(&game_state, d1, black) &&
                            !get_attacked(&game_state, e1, black)) {
                            add_move(list, ENCODE_MOVE(e1, c1, piece, piece, 0,
                                                       0, 0, 1));
                        }
                    }
                }
            }
        }

        /* black pawns & black king castling */
        else if (game_state.side == black) {
            if (piece == p) {
                while (bitboard) {
                    source = get_lsb_index(bitboard);
                    target = source - 8;

                    if ((target >= 0 && target < 64) &&
                        !get_bit(game_state.positions[both], target)) {
                        /* promotion */
                        if (source >= a2 && source <= h2) {
                            add_move(list, ENCODE_MOVE(source, target, piece, b,
                                                       0, 0, 0, 0));
                            add_move(list, ENCODE_MOVE(source, target, piece, r,
                                                       0, 0, 0, 0));
                            add_move(list, ENCODE_MOVE(source, target, piece, n,
                                                       0, 0, 0, 0));
                            add_move(list, ENCODE_MOVE(source, target, piece, q,
                                                       0, 0, 0, 0));
                        } else {
                            if (!get_bit(game_state.positions[both], target)) {
                                add_move(list,
                                         ENCODE_MOVE(source, target, piece,
                                                     piece, 0, 0, 0, 0));
                                if ((source >= a7 && source <= h7) &&
                                    !get_bit(game_state.positions[both],
                                             target - 8)) {
                                    add_move(list,
                                             ENCODE_MOVE(source, (target - 8),
                                                         piece, piece, 0, 1, 0,
                                                         0));
                                }
                            }
                        }
                    }

                    attacks = pawn_attacks[game_state.side][source] &
                              game_state.positions[white];

                    while (attacks) {
                        target = get_lsb_index(attacks);

                        if ((target >= 0 && target < 64) &&
                            get_bit(game_state.positions[white], target)) {
                            /* capture promotion */
                            if (source >= a2 && source <= h2) {
                                add_move(list,
                                         ENCODE_MOVE(source, target, piece, b,
                                                     1, 0, 0, 0));
                                add_move(list,
                                         ENCODE_MOVE(source, target, piece, r,
                                                     1, 0, 0, 0));
                                add_move(list,
                                         ENCODE_MOVE(source, target, piece, n,
                                                     1, 0, 0, 0));
                                add_move(list,
                                         ENCODE_MOVE(source, target, piece, q,
                                                     1, 0, 0, 0));
                            } else {
                                if (get_bit(game_state.positions[white],
                                            target)) {
                                    add_move(list,
                                             ENCODE_MOVE(source, target, piece,
                                                         piece, 1, 0, 0, 0));
                                }
                            }
                        }

                        pop_bit(attacks, target);
                    }

                    /* en passant */
                    if (game_state.enpassant != no_sq) {
                        u64 enpassant_attacks =
                                pawn_attacks[game_state.side][source] &
                                (1ULL << game_state.enpassant);
                        if (enpassant_attacks) {
                            int target_enpassant =
                                    get_lsb_index(enpassant_attacks);
                            add_move(list,
                                     ENCODE_MOVE(source, target_enpassant,
                                                 piece, piece, 1, 0, 1, 0));
                        }
                    }

                    pop_bit(bitboard, source);
                }
            }

            /* castling */
            if (piece == k) {
                if (game_state.castle & BKCK) {
                    if (!get_bit(game_state.positions[both], f8) &&
                        !get_bit(game_state.positions[both], g8)) {
                        if (!get_attacked(&game_state, e8, white) &&
                            !get_attacked(&game_state, f8, white)) {
                            add_move(list, ENCODE_MOVE(e8, g8, piece, piece, 0,
                                                       0, 0, 1));
                        }
                    }
                }
                if (game_state.castle & BKCQ) {
                    if (!get_bit(game_state.positions[both], d8) &&
                        !get_bit(game_state.positions[both], c8) &&
                        !get_bit(game_state.positions[both], b8)) {
                        if (!get_attacked(&game_state, d8, white) &&
                            !get_attacked(&game_state, e8, white)) {
                            add_move(list, ENCODE_MOVE(e8, c8, piece, piece, 0,
                                                       0, 0, 1));
                        }
                    }
                }
            }
        }

        /* knight */
        if ((game_state.side == white) ? piece == N : piece == n) {
            while (bitboard) {
                source  = get_lsb_index(bitboard);

                attacks = knight_attacks[source] &
                          ((game_state.side == white)
                                   ? ~game_state.positions[white]
                                   : ~game_state.positions[black]);
                while (attacks) {
                    target = get_lsb_index(attacks);

                    if ((game_state.side == white)
                                ? get_bit(game_state.positions[black], target)
                                : get_bit(game_state.positions[white],
                                          target)) {
                        add_move(list, ENCODE_MOVE(source, target, piece, piece,
                                                   1, 0, 0, 0));
                    } else {
                        add_move(list, ENCODE_MOVE(source, target, piece, piece,
                                                   0, 0, 0, 0));
                    }

                    pop_bit(attacks, target);
                }

                pop_bit(bitboard, source);
            }
        }

        /* bishop */
        if ((game_state.side == white) ? piece == B : piece == b) {
            while (bitboard) {
                source = get_lsb_index(bitboard);

                attacks =
                        get_bishop_attacks(source, game_state.positions[both]) &
                        ((game_state.side == white)
                                 ? ~game_state.positions[white]
                                 : ~game_state.positions[black]);
                while (attacks) {
                    target = get_lsb_index(attacks);

                    if ((game_state.side == white)
                                ? get_bit(game_state.positions[black], target)
                                : get_bit(game_state.positions[white],
                                          target)) {
                        add_move(list, ENCODE_MOVE(source, target, piece, piece,
                                                   1, 0, 0, 0));
                    } else {
                        add_move(list, ENCODE_MOVE(source, target, piece, piece,
                                                   0, 0, 0, 0));
                    }

                    pop_bit(attacks, target);
                }

                pop_bit(bitboard, source);
            }
        }

        /* rook */
        if ((game_state.side == white) ? piece == R : piece == r) {
            while (bitboard) {
                source  = get_lsb_index(bitboard);

                attacks = get_rook_attacks(source, game_state.positions[both]) &
                          ((game_state.side == white)
                                   ? ~game_state.positions[white]
                                   : ~game_state.positions[black]);
                while (attacks) {
                    target = get_lsb_index(attacks);

                    if ((game_state.side == white)
                                ? get_bit(game_state.positions[black], target)
                                : get_bit(game_state.positions[white],
                                          target)) {
                        add_move(list, ENCODE_MOVE(source, target, piece, piece,
                                                   1, 0, 0, 0));
                    } else {
                        add_move(list, ENCODE_MOVE(source, target, piece, piece,
                                                   0, 0, 0, 0));
                    }

                    pop_bit(attacks, target);
                }

                pop_bit(bitboard, source);
            }
        }

        /* queen */
        if ((game_state.side == white) ? piece == Q : piece == q) {
            while (bitboard) {
                source = get_lsb_index(bitboard);

                attacks =
                        get_queen_attacks(source, game_state.positions[both]) &
                        ((game_state.side == white)
                                 ? ~game_state.positions[white]
                                 : ~game_state.positions[black]);
                while (attacks) {
                    target = get_lsb_index(attacks);

                    if ((game_state.side == white)
                                ? get_bit(game_state.positions[black], target)
                                : get_bit(game_state.positions[white],
                                          target)) {
                        add_move(list, ENCODE_MOVE(source, target, piece, piece,
                                                   1, 0, 0, 0));
                    } else {
                        add_move(list, ENCODE_MOVE(source, target, piece, piece,
                                                   0, 0, 0, 0));
                    }

                    pop_bit(attacks, target);
                }

                pop_bit(bitboard, source);
            }
        }

        /* king */
        if ((game_state.side == white) ? piece == K : piece == k) {
            while (bitboard) {
                source  = get_lsb_index(bitboard);

                attacks = king_attacks[source] &
                          ((game_state.side == white)
                                   ? ~game_state.positions[white]
                                   : ~game_state.positions[black]);
                while (attacks) {
                    target = get_lsb_index(attacks);

                    if ((game_state.side == white)
                                ? get_bit(game_state.positions[black], target)
                                : get_bit(game_state.positions[white],
                                          target)) {
                        add_move(list, ENCODE_MOVE(source, target, piece, piece,
                                                   1, 0, 0, 0));
                    } else {
                        add_move(list, ENCODE_MOVE(source, target, piece, piece,
                                                   0, 0, 0, 0));
                    }

                    pop_bit(attacks, target);
                }

                pop_bit(bitboard, source);
            }
        }
    }

    return;
}

////////////////////////////////////////////////////////////////////////////////
//                                  Test //
////////////////////////////////////////////////////////////////////////////////

#ifdef BB_TEST
#include <ndjin/fen.h>

int main(int argc, char **argv)
{
    // puts("white\n");
    // for (int i = 0; i < 64; i += 2) {
    //     printf("%lluULL, \t\t%lluULL\n", mask_pawn_attacks(i, white),
    //            mask_pawn_attacks(i + 1, white));
    // }
    //
    // puts("black\n");
    // for (int i = 0; i < 64; i += 2) {
    //     printf("%lluULL, \t\t%lluULL\n", mask_pawn_attacks(i, black),
    //            mask_pawn_attacks(i + 1, black));
    // }

    init_all();

    // init_board();
    parse_fen(STATE3, &game_state);
    print_board(1);
    puts("");

    struct move_list_t moves[1] = {0};
    generate_moves(moves);
    print_move_list(moves, 1);

    struct state_t backup_state = {0};

    for (int i = 0; i < 64; ++i) {
        for (int j = 0; j < moves->squares[i].count; ++j) {
            BOARD_BACKUP(&game_state, &backup_state);
            print_board(1);
            // print_bitboard(positions[white]);
            // print_bitboard(positions[black]);
            // print_bitboard(positions[both]);
            getchar();
            make_move(moves->squares[i].moves[j], all_moves);
            print_board(1);
            // print_bitboard(positions[white]);
            // print_bitboard(positions[black]);
            // print_bitboard(positions[both]);
            BOARD_RESTORE(&backup_state, &game_state);
            getchar();
        }
    }

    return 0;
}
#endif /* BB_TEST */

/* vim: ft=c ts=4 sts=4 sw=4 ai et cin */
