/* players.h
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
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stdint.h>
#include <time.h>

#include <raylib.h>

#include "dsa.h"
#include "board.h"
#include "players.h"
#include "ui.h"

extern mem_ctx_t board_ctx;
// extern mem_ctx_t enemeies_ctx
extern mem_ctx_t pieces_ctx;

static const int screen_height = 640;
static const int screen_width  = 560;
static int       moved         = 0;

extern struct square_t *squares;
extern struct piece_s  *floating_piece;
extern struct square_t *original_sqr;

int pair_players(struct player_state_t *players, size_t player_count,
                 struct player_state_t *p1, struct player_state_t *p2)
{
    int a = rand() % player_count;
    int b = (rand() % player_count) ^ a;
    if (memcpy(( uint8_t * )p1,
               ( uint8_t * )(players + (a * sizeof(struct player_state_t))),
               sizeof(struct player_state_t) * sizeof(uint8_t)))
        return -1;
    if (memcpy(( uint8_t * )p2,
               ( uint8_t * )(players + (b * sizeof(struct player_state_t))),
               sizeof(struct player_state_t) * sizeof(uint8_t)))
        return -2;
    return 0;
}

int init_game(struct game_state_t *gs, struct player_state_t *p1,
              struct player_state_t *p2)
{
    if (!gs) {
        fprintf(stderr, "errpr: no valid gamestate provided\n");
        return -1;
    }

    strncpy(p1->user_banner, "Player 1", -1);
    strncpy(p2->user_banner, "Player 2", -1);
    p1->score            = 0;
    p1->move_count       = 0;
    p1->inactivity_count = 0;

    gs->player           = 0;
    gs->winner           = 0;

    gs->turn_count       = 0;
    gs->player1_count    = 0;
    gs->player2_count    = 0;
    gs->inactive_run     = 0;

    p1->remaining_time   = 10 * 60;
    p2->remaining_time   = 10 * 60;

    gs->player1_score    = 0;
    gs->player2_score    = 0;

    int a, b = 0;
    a = (rand() % 2) + 1; // 1 - 2
    b = 2 - a + 1;
    snprintf(gs->log_messages, 39, "Player %d -> WHITE : Player %d -> BLACK", a,
             b);

    p1->pieces = (a == 2 ? WHITE_ID /* W */ : BLACK_ID /* B */);
    p2->pieces = (b == 2 ? WHITE_ID /* W */ : BLACK_ID /* B */);
    if (p1->pieces == WHITE_ID)
        p1->active = 1;
    else
        p2->active = 1;

    return 0;
}

int play_game(struct game_state_t *gs, struct player_state_t *p1,
              struct player_state_t *p2)
{
    InitWindow(screen_width, screen_height, "chEss [E]  - board");
    SetTargetFPS(30);

    int rc = 0;
    load_assets();
    if ((rc = init_squares(&board_ctx, NULL)) != 0) {
        fprintf(stderr, "error: init_squares()\t%d\n", rc);
    }

    uint64_t pstate = checksum_board(squares);
    uint64_t nstate = checksum_board(squares);
    int      p1_elapsed;
    int      p2_elapsed;
    Color    p1c = p1->pieces == WHITE_ID ? WHITE : BLACK;
    Color    p2c = p2->pieces == WHITE_ID ? WHITE : BLACK;
    puts(gs->log_messages);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BROWN);

        draw_squares();

        char  p1a = ' ';
        char  p2a = ' ';
        int   current_pieces;
        Color current_colour;
        if (p1->active) {
            if (p1->start_time == 0) {
                p1->start_time = time(0);
                p2->start_time = 0;
            }
            current_pieces = p1->pieces;
            current_colour = p1c;
            p2a            = 'x';
            p1a            = ' ';
        } else if (p2->active) {
            if (p2->start_time == 0) {
                p2->start_time = time(0);
                p1->start_time = 0;
            }
            current_pieces = p2->pieces;
            current_colour = p2c;
            p2a            = ' ';
            p1a            = 'x';
        }

        rc = update_input(current_colour);
        if (floating_piece && floating_piece->role)
            draw_piece_middle(floating_piece->role, GetMouseX(), GetMouseY());
        nstate = checksum_board(squares);

        if (p1->active) {
            p1_elapsed     = (time(0) - p1->start_time);
            int p2_elapsed = 0;
        } else if (p2->active) {
            p2_elapsed     = (time(0) - p2->start_time);
            int p1_elapsed = 0;
        }

        char buf3[256] = {0};
        char buf4[256] = {0};
        snprintf(buf3, 256, "Moves: %d |  Time Remaining: %d:%02d",
                 p1->move_count,
                 ((p1->remaining_time - p1_elapsed) % 3600) / 60,
                 (p1->remaining_time - p1_elapsed) % 60);
        snprintf(buf4, 256, "Moves: %d | Time Remaining: %d:%02d",
                 p2->move_count,
                 ((p2->remaining_time - p2_elapsed) % 3600) / 60,
                 (p2->remaining_time - p2_elapsed) % 60);

        char buf1[11] = {'B', 'L', 'A', 'C', 'K', ' ', '[', p1a, ']', '\0'};
        char buf2[11] = {'[', p2a, ']', ' ', 'W', 'H', 'I', 'T', 'E', '\0'};
        DrawText(buf1, 10, 10, 20, LIGHTGRAY);
        DrawText(buf2, 450, 610, 20, LIGHTGRAY);

        if (p1->pieces == BLACK_ID)
            DrawText(buf3, 250, 10, 20, LIGHTGRAY);
        else if (p1->pieces == WHITE_ID)
            DrawText(buf3, 30, 610, 20, LIGHTGRAY);
        if (p2->pieces == WHITE_ID)
            DrawText(buf4, 30, 480, 20, LIGHTGRAY);
        else if (p2->pieces == BLACK_ID)
            DrawText(buf4, 250, 10, 20, LIGHTGRAY);

        if (nstate != pstate) {
            pstate = nstate;
            if (p1->active) {
                p1->active = 0;
                p2->active = 1;
                ++p1->move_count;
                p1->remaining_time -= time(0) - p1->start_time;
                ++gs->player1_count;
                p1->start_time = 0;
                p2->start_time = time(0);
                ++gs->turn_count;
            } else if (p2->active) {
                p1->active = 1;
                p2->active = 0;
                ++p2->move_count;
                p2->remaining_time -= time(0) - p2->start_time;
                ++gs->player2_count;
                p2->start_time = 0;
                p1->start_time = time(0);
                ++gs->turn_count;
            }
        }

        EndDrawing();
    }

    CloseWindow();

    destroy_board(&board_ctx, squares);

    return 0;
}

int end_game(struct game_state_t *gs, struct player_state_t *p1,
             struct player_state_t *p2, int outcome);
