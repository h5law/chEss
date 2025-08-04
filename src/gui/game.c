/* game.c
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

#include "raylib.h"

#include <ndjin/bb.h>
#include <ndjin/types.h>

#include "game.h"
#include "state.h"

extern const char *square_to_coord[64];
extern const char  ascii_pieces[13];

struct square_t {
    int piece;
    int square;
};

struct square_t no_square = {-1, no_sq};

Texture2D wp, bp, wn, bn, wb, bb, wr, br, wq, bq, wk, bk;

int             floating_piece = -1;
struct square_t original_sqr   = {0};

void load_assets(void)
{
    Image img;
    img = LoadImage("assets/wp.png");
    ImageResizeNN(&img, 70 * 1, 70 * 1);
    wp  = LoadTextureFromImage(img);
    img = LoadImage("assets/bp.png");
    ImageResizeNN(&img, 70 * 1, 70 * 1);
    bp  = LoadTextureFromImage(img);
    img = LoadImage("assets/wb.png");
    ImageResizeNN(&img, 70 * 1, 70 * 1);
    wb  = LoadTextureFromImage(img);
    img = LoadImage("assets/bb.png");
    ImageResizeNN(&img, 70 * 1, 70 * 1);
    bb  = LoadTextureFromImage(img);
    img = LoadImage("assets/wn.png");
    ImageResizeNN(&img, 70 * 1, 70 * 1);
    wn  = LoadTextureFromImage(img);
    img = LoadImage("assets/bn.png");
    ImageResizeNN(&img, 70 * 1, 70 * 1);
    bn  = LoadTextureFromImage(img);
    img = LoadImage("assets/wr.png");
    ImageResizeNN(&img, 70 * 1, 70 * 1);
    wr  = LoadTextureFromImage(img);
    img = LoadImage("assets/br.png");
    ImageResizeNN(&img, 70 * 1, 70 * 1);
    br  = LoadTextureFromImage(img);
    img = LoadImage("assets/wq.png");
    ImageResizeNN(&img, 70 * 1, 70 * 1);
    wq  = LoadTextureFromImage(img);
    img = LoadImage("assets/bq.png");
    ImageResizeNN(&img, 70 * 1, 70 * 1);
    bq  = LoadTextureFromImage(img);
    img = LoadImage("assets/wk.png");
    ImageResizeNN(&img, 70 * 1, 70 * 1);
    wk  = LoadTextureFromImage(img);
    img = LoadImage("assets/bk.png");
    ImageResizeNN(&img, 70 * 1, 70 * 1);
    bk = LoadTextureFromImage(img);
}

void draw_piece(int piece, float x, float y)
{
    Vector2   pos = {x, y};
    Texture2D sprite;
    switch (piece) {
    case P:
        sprite = wp;
        break;
    case p:
        sprite = bp;
        break;
    case B:
        sprite = wb;
        break;
    case b:
        sprite = bb;
        break;
    case N:
        sprite = wn;
        break;
    case n:
        sprite = bn;
        break;
    case R:
        sprite = wr;
        break;
    case r:
        sprite = br;
        break;
    case Q:
        sprite = wq;
        break;
    case q:
        sprite = bq;
        break;
    case K:
        sprite = wk;
        break;
    case k:
        sprite = bk;
        break;
    }
    Rectangle rect = {0, 0, 70, 70};
    DrawTextureRec(sprite, rect, pos, WHITE);
}

void draw_piece_middle(int piece, float x, float y)
{
    float px = ( float )x - 70.0 / 2.0;
    float py = ( float )y - 70.0 / 2.0;
    return draw_piece(piece, px, py);
}

int draw_bitboard(struct game_t *data, unsigned long long bitboard, int piece,
                  int check)
{
    int x, y = 0;
    for (int i = 0; i < 64; ++i) {
        if ((1ULL << i) & bitboard) {
            x             = (i % 8) * 70;
            y             = (8 - (i / 8) - 1) * 70 + 40;

            Vector2   pos = {x, y};
            Texture2D sprite;
            Image     img = {0};
            switch (piece) {
            case P:
                sprite = wp;
                break;
            case p:
                sprite = bp;
                break;
            case B:
                sprite = wb;
                break;
            case b:
                sprite = bb;
                break;
            case N:
                sprite = wn;
                break;
            case n:
                sprite = bn;
                break;
            case R:
                sprite = wr;
                break;
            case r:
                sprite = br;
                break;
            case Q:
                sprite = wq;
                break;
            case q:
                sprite = bq;
                break;
            case K:
                sprite = wk;
                if (check == white_check)
                    DrawRectangle(x, y, 70, 70, RED);
                break;
            case k:
                sprite = bk;
                if (check == black_check)
                    DrawRectangle(x, y, 70, 70, RED);
                break;
            }

            Rectangle rect = {0, 0, 70, 70};
            DrawTextureRec(sprite, rect, pos, WHITE);
        }
    }
    return 0;
}

struct square_t get_square(struct game_t *data, struct state_t *state, int x,
                           int y)
{
    int i                  = ( int )(x / 70);
    int j                  = ( int )((y - 40) / 70);
    int sq                 = 0;
    sq                     = ((7 - j) * 8) + i;
    struct square_t square = {-1, sq};
    for (int x = 0; x < 12; ++x) {
        if ((1ULL << sq) & state->bitboards[x]) {
            square.piece = x;
            break;
        }
    }
    return square;
}

struct square_moves_t possibles = {0};
int update_input(struct game_t *data, struct move_list_t *list)
{
    int             rc = 0;
    struct square_t sq =
            get_square(data, data->game_state, GetMouseX(), GetMouseY());

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (sq.piece == -1)
            goto draw_floating;

        if ((sq.piece < 6 && data->game_state->side == black) ||
            (sq.piece >= 6 && data->game_state->side == white))
            goto draw_floating;

        possibles      = list->squares[sq.square];
        floating_piece = sq.piece;
        original_sqr   = sq;
    } else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        if (original_sqr.square < 64 && original_sqr.piece >= 0) {
            int source, target, piece, promo;
            for (int i = 0; i < possibles.count; ++i) {
                DECODE_MOVE(possibles.moves[i], &source, &target, &piece,
                            &promo);
                if (target == sq.square) {
                    make_move(data->game_state, possibles.moves[i], all_moves);
                    original_sqr = no_square;
                    memset(&possibles, 0, sizeof(struct square_moves_t));
                    break;
                }
            }
            floating_piece = -1;
        }
    } else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        floating_piece = -1;
        original_sqr   = no_square;
        memset(&possibles, 0, sizeof(struct square_moves_t));
    }

    for (int i = 0; i < possibles.count; ++i) {
        int source, target, piece, promo;
        DECODE_MOVE(possibles.moves[i], &source, &target, &piece, &promo);
        Color colour = {0, 228, 48, 100};
        DrawRectangle((target % 8) * 70, (7 - (target / 8)) * 70 + 40, 70, 70,
                      colour);
    }

draw_floating:
    if (floating_piece >= 0)
        draw_piece_middle(floating_piece, GetMouseX(), GetMouseY());
    for (int i = 0; i < 12; ++i) {
        draw_bitboard(data, data->game_state->bitboards[i], i,
                      data->game_state->check);
    }

    return 0;
}

char score_white[16] = {0};
char score_black[16] = {0};
void draw_move_scores(struct game_t *data, struct move_list_t *list)
{
    double eval = symmetric_eval(data->game_state, list);
    if (data->game_state->side == white)
        snprintf(score_white, 16, "%02f", eval);
    else
        snprintf(score_black, 16, "%02f", eval);
    DrawText(score_white, 230, 610, 20, LIGHTGRAY);
    DrawText(score_black, 230, 10, 20, LIGHTGRAY);
}

char tag1[14]   = {0};
char tag2[14]   = {0};
char check[6]   = {'C', 'H', 'E', 'C', 'K', '\0'};
char mate[10]   = {'C', 'H', 'E', 'C', 'K', 'M', 'A', 'T', 'E', '\0'};
char winner[10] = {'W', 'I', 'N', 'N', 'E', 'R', '\0'};
void draw_player_bar(struct game_t *data)
{
    char ws = ' ';
    char bs = ' ';
    if (data->game_state->side == white) {
        ws = 'x';
        bs = ' ';
    } else if (data->game_state->side == black) {
        ws = ' ';
        bs = 'x';
    }
    if (data->white == 0x1) {
        snprintf(tag1, 14, "P%d BLACK [%c]", 2, bs);
        snprintf(tag2, 14, "[%c] WHITE P%d", ws, 1);
    } else {
        snprintf(tag1, 14, "[%c] BLACK P%d ", bs, 1);
        snprintf(tag2, 14, "P%d WHITE [%c]", 2, ws);
    }

    DrawText(tag1, 10, 10, 20, LIGHTGRAY);
    DrawText(tag2, 420, 610, 20, LIGHTGRAY);
    if (data->game_state->check == white_check)
        DrawText(check, 10, 610, 20, LIGHTGRAY);
    else if (data->game_state->check == black_check)
        DrawText(check, 480, 10, 20, LIGHTGRAY);
}

/* vim: ft=c ts=4 sts=4 sw=4 ai et cin */
