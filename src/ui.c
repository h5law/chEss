/* ui.c
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
#include <string.h>

#include <raylib.h>

#include "dsa.h"
#include "board.h"
#include "ui.h"
#include "moves.h"

extern mem_ctx_t board_ctx;
// extern mem_ctx_t enemeies_ctx
extern mem_ctx_t pieces_ctx;

struct square_t *squares;
Texture2D        sprite_sheet;

struct piece_s  *floating_piece;
struct square_t *original_sqr;

int update_board(void)
{
    for (int i = 0; i < 8 * 8; i++) {
        struct square_t *sq = squares + i;
        piece_t         *p  = &sq->piece.role;
        if ((squares[i].piece.role & PIECE_MASK) < 7) {
            int row = i / 8;
            int col = i % 8;
            switch (PIECE_ROLE(*p)) {
            case PAWN_ID:
                break;
            case KNIGHT_ID:
                break;
            }
        }
    }

    return 0;
}

int init_squares(mem_ctx_t *board_ctx, mem_ctx_t *pieces_ctx)
{
    int rc = ERR_OKAY;
    if (board_ctx) {
        if ((rc = mem_init(board_ctx, 64 * sizeof(struct square_t) *
                                              (sizeof(struct piece_s) * 8 *
                                               8))) != ERR_OKAY)
            return rc;
    } else
        return 1;
    if (pieces_ctx) {
        if ((rc = mem_init(pieces_ctx, 64 * sizeof(struct piece_s))) !=
            ERR_OKAY)
            return rc;
    }

    squares = ( struct square_t * )mem_alloc(
            board_ctx, sizeof(struct square_t) * 8 * 8, ZERO_REGION_OP);
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            struct square_t *sq = &squares[i * 8 + j];
            sq->row             = i;
            sq->col             = j;
            if ((i + j) % 2 == 1) {
                sq->colour = BROWN;
            } else {
                sq->colour = BEIGE;
            }
            sq->piece.role = ((i << 12) | (j << 8)) | 0x0007;
        }
    }

    for (int i = 0; i < 8; i++) {
        squares[8 + i].piece.role = (1 << 12) | (i << 8) | PAWN_ID | BLACK_MASK;
        squares[(6 * 8) + i].piece.role =
                (6 << 12) | (i << 8) | PAWN_ID | WHITE_MASK;
    }
    squares[0].piece.role         = (0 << 12) | (0 << 8) | ROOK_ID | BLACK_MASK;
    squares[7].piece.role         = (0 << 12) | (7 << 8) | ROOK_ID | BLACK_MASK;
    squares[8 * 7].piece.role     = (7 << 12) | (0 << 8) | ROOK_ID | WHITE_MASK;
    squares[8 * 7 + 7].piece.role = (7 << 12) | (7 << 8) | ROOK_ID | WHITE_MASK;
    squares[1].piece.role = (0 << 12) | (1 << 8) | KNIGHT_ID | BLACK_MASK;
    squares[6].piece.role = (0 << 12) | (6 << 8) | KNIGHT_ID | BLACK_MASK;
    squares[8 * 7 + 1].piece.role =
            (7 << 12) | (1 << 8) | KNIGHT_ID | WHITE_MASK;
    squares[8 * 7 + 6].piece.role =
            (7 << 12) | (6 << 8) | KNIGHT_ID | WHITE_MASK;
    squares[2].piece.role = (0 << 12) | (2 << 8) | BISHOP_ID | BLACK_MASK;
    squares[5].piece.role = (0 << 12) | (5 << 8) | BISHOP_ID | BLACK_MASK;
    squares[8 * 7 + 2].piece.role =
            (7 << 12) | (2 << 8) | BISHOP_ID | WHITE_MASK;
    squares[8 * 7 + 5].piece.role =
            (7 << 12) | (5 << 8) | BISHOP_ID | WHITE_MASK;
    squares[3].piece.role = (0 << 12) | (3 << 8) | QUEEN_ID | BLACK_MASK;
    squares[4].piece.role = (0 << 12) | (4 << 8) | KING_ID | BLACK_MASK;
    squares[8 * 7 + 3].piece.role =
            (7 << 12) | (3 << 8) | QUEEN_ID | WHITE_MASK;
    squares[8 * 7 + 4].piece.role = (7 << 12) | (4 << 8) | KING_ID | WHITE_MASK;

    return update_board();
}

void load_assets(void)
{
    Image img = LoadImage("assets/sprites.png");
    ImageResizeNN(&img, 70 * 6, 70 * 2);
    sprite_sheet = LoadTextureFromImage(img);
}

int draw_piece(piece_t p, float x, float y)
{
    Vector2   pos          = {x, y};
    int       role         = PIECE_ROLE(p);
    int       sprite_index = role - 1;
    Rectangle rect         = {70 * sprite_index, 70 * PIECE_COLOUR(p), 70, 70};
    // printf("Drawing piece: %s, role=%d, sprite_x=%f, sprite_y=%f, "
    //        "pos=(%f,%f)\n",
    //        PIECE_STRING(p), role + 1, rect.x, rect.y, x, y);
    DrawTextureRec(sprite_sheet, rect, pos, WHITE);

    return 0;
}

int draw_piece_middle(piece_t p, float x, float y)
{
    float px = ( float )x - 70.0 / 2.0;
    float py = ( float )y - 70.0 / 2.0;
    return draw_piece(p, px, py);
}

int draw_squares(void)
{
    for (int i = 0; i < 64; i++) {
        float sqX = squares[i].col * 70;
        float sqY = squares[i].row * 70 + 40;
        DrawRectangle(sqX, sqY, 70, 70, squares[i].colour);
        if ((squares[i].piece.role & PIECE_MASK) < 7) {
            // printf("%f, %f -> %s\n", sqX, sqY,
            //        PIECE_STRING(squares[i].piece.role));
            draw_piece(squares[i].piece.role, sqX, sqY);
        }
    }

    return 0;
}

int update_input(Color current_colour)
{
    int              rc = 0;
    struct square_t *sq = get_square(squares, GetMouseX(), GetMouseY());

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                struct square_t *sq = &squares[i * 8 + j];
                sq->row             = i;
                sq->col             = j;
                if ((i + j) % 2 == 1) {
                    sq->colour = BROWN;
                } else {
                    sq->colour = BEIGE;
                }
            }
        }

        if (((sq->piece.role & PIECE_MASK) >= 7))
            return 0; // continue;

        if ((PIECE_COLOUR(sq->piece.role) == 0x01 &&
             memcmp(&current_colour, &WHITE, sizeof(Color)) == 0) ||
            (PIECE_COLOUR(sq->piece.role) == 0x02 &&
             memcmp(&current_colour, &BLACK, sizeof(Color)) == 0))
            return 0; // continue;

        fprintf(stderr, "updating_board(): backing up floating_piece\n");
        floating_piece = &sq->piece;
        original_sqr   = sq;
        if (sq->piece.role && !floating_piece)
            destroy_piece(NULL, &sq->piece);
    } else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        if (original_sqr) {
            if (move_piece(original_sqr, sq) != 0)
                fprintf(stderr, "move_piece(): reverted to original square\n");
        }
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                struct square_t *sq = &squares[i * 8 + j];
                sq->row             = i;
                sq->col             = j;
                if ((i + j) % 2 == 1) {
                    sq->colour = BROWN;
                } else {
                    sq->colour = BEIGE;
                }
            }
        }
    }

    if (original_sqr) {
        uint64_t mask = move_mask(squares, original_sqr->piece.role);
        for (int i = 0; i < 64; ++i) {
            if (((mask >> i) & 1) == 1)
                squares[i].colour = GREEN;
        }
    }
    draw_squares();

    return 0;
}
