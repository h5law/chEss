#include <stdio.h>

#include "bb.h"
#include "../fen.h"

int main(int argc, char **argv)
{
    // u64 blockers = 0ULL;
    // set_bit(blockers, e6);
    // set_bit(blockers, d5);
    // set_bit(blockers, c4);
    //
    // for (int i = 0; i < 64; ++i) {
    //     for (int j = 0; j < 4096; ++j) {
    //         u64 attack_mask = mask_rook_attacks(i);
    //         u64 positions =
    //                 set_positions(j, count_bits(attack_mask), attack_mask);
    //         print_bitboard(positions);
    //     }
    // }

    // for (int i = 0; i < 8; ++i) {
    //     for (int j = 0; j < 8; ++j) {
    //         int square = (i * 8) + j;
    //         printf("  %d,", count_bits(mask_rook_attacks(square)));
    //     }
    //     puts("");
    // }
    // puts("");

    // printf("%lluULL\n", rand());
    // for (int i = 0; i < 10; ++i)
    //     printf("%lluULL\n", xorshift64());

    // extern const int bishop_bits[64];
    // puts("Bishop magics");
    // for (int i = 0; i < 64; ++i) {
    //     u64 magic = find_magic(i, bishop_bits[i], bishop);
    //     printf("  %lluULL,\t", magic);
    //     if ((i + 1) % 2 == 0)
    //         puts("");
    // }
    // puts("");
    //
    // extern const int rook_bits[64];
    // puts("Rook magics");
    // for (int i = 0; i < 64; ++i) {
    //     u64 magic = find_magic(i, rook_bits[i], rook);
    //     printf("  %lluULL,\t", magic);
    //     if ((i + 1) % 2 == 0)
    //         puts("");
    // }

    init_all();

    // u64 position = 0ULL;
    // set_bit(position, d5);
    // set_bit(position, f3);
    // set_bit(position, g6);
    //
    // print_bitboard(position);
    // print_bitboard(get_bishop_attacks(e4, position));

    // init_board();

    print_board(1);

    puts("\n");

    extern u64 bitboards[12];
    // for (int i = 0; i < 12; ++i)
    // print_bitboard(bitboards['r' - 105]);
    //
    // puts("\n");
    //
    extern u64 positions[3];
    // for (int i = 0; i < 3; ++i)
    //     print_bitboard(positions[i]);

    extern int side;
    extern int castle;
    extern int enpassant;
    extern int ply;
    extern int moves;

    // puts(START_BOARD);
    // parse_fen(START_BOARD, bitboards, positions, &side, &castle, &enpassant,
    //           &ply, &moves);
    //
    // print_board(1);

    puts(STATE3);
    parse_fen(STATE3, bitboards, positions, &side, &castle, &enpassant, &ply,
              &moves);

    print_board(1);

    // for (int i = 0; i < 12; ++i)
    //     print_bitboard(bitboards[i]);
    // puts("\n");

    return 0;
}
