/* main.c
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

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "raylib.h"

#include <ndjin/bb.h>
#include <ndjin/fen.h>
#include <net/network.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include "pre.h"
#include "game.h"
#include "state.h"

const int      win_height = 640;
const int      win_width  = 560;
struct state_t state      = {0};

extern struct p2p connection;

enum { splash, new_game, join_game, game_lobby, game_play };

void draw_squares(void)
{
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; j++) {
            Color colour;
            if ((i + j) % 2 == 1) {
                colour = BROWN;
            } else {
                colour = BEIGE;
            }
            DrawRectangle(i * 70, j * 70 + 40, 70, 70, colour);
        }
    }
}

static inline u64 xorshift64(void)
{
    u64 x  = rand() * time(NULL);
    x     ^= x << 13;
    x     ^= x >> 7;
    x     ^= x << 17;
    return x;
}

static inline unsigned int rand_u12(void)
{
    u64 w, x, y, z = 0ULL;
    w = xorshift64() & 0x7;
    x = xorshift64() & 0x7;
    y = xorshift64() & 0x7;
    z = xorshift64() & 0x7;
    return w | (x << 3) | (y << 6) | (z << 9);
}

int main(int argc, char **argv)
{
    init_all();

    struct game_t      data            = {0};
    struct move_list_t available_moves = {0};

    data.white                         = rand_u12() & 0x1 ? 1 : 2;
    data.game_state                    = &state;

    parse_fen(START_BOARD, &state);
    generate_moves(&state, &available_moves);

    InitWindow(win_width, win_height, "ndjin [chess]");
    SetTargetFPS(60);

    load_assets();

    char code[16] = {0};
    u64  nstate, pstate = state.positions[2];

    enum { no_opponent, manual_play, bot_play };

    int opponent = no_opponent;
    int ui_state = splash;
    int rc       = 0;

    char ip[16]  = {0};
    get_external_ip(ip);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(DARKBROWN);

        draw_squares();

        switch (ui_state) {
        case splash:
            /* --- START: Splash Screen --- */
            title_box("ndjin", 135);
            if ((rc = draw_tb_buttons("NEW GAME", "JOIN GAME")) > 0)
                ui_state = rc;
            /* --- END: Splash Screen --- */
            break;
        case new_game:
            /* --- START: New Game Screen --- */
            title_box("new game", 50);
            if ((rc = draw_tb_buttons("   manual", "  ndjin bot")) > 0)
                opponent = rc;
            if (opponent > no_opponent) {
                data.player = 0x1;
                if (opponent == manual_play)
                    ui_state = game_lobby;
            }
            break;
            /* --- END: New Game Screen --- */
        case join_game:
            /* --- START: New Game Screen --- */
            title_box("join game", 40);
            game_code_entry(code);
            if ((rc = draw_tb_buttons(NULL, "  connect")) == 2) {
                data.player  = 0x2;
                char buf[16] = {0};
                get_external_ip(buf);
                if (!send_domain(buf))
                    break;
                if (establish(inet_addr(code)))
                    ui_state = game_play;
            }
            break;
            /* --- END: New Game Screen --- */
        case game_lobby:
            /* --- START: New Game Lobby --- */
            title_box("pairing", 100);
            show_game_code(ip);
            char buf[16] = {0};
            if (!listen_for_domain(buf))
                break;
            if (establish(inet_addr(buf)) == 0)
                ui_state = game_play;
            break;
        /* --- END: New Game Lobby --- */
        case game_play:
            /* --- START: Game Screen --- */
            if ((nstate = state.positions[2]) != pstate) {
                pstate = nstate;
                fprintf(stderr, "generate_moves(): state changed, "
                                "generating moveset\n");
                generate_moves(&state, &available_moves);
            }
            update_input(&data, &available_moves);

            draw_move_scores(&data, &available_moves);
            draw_player_bar(&data);
            /* --- END: Game Screen --- */
            break;
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

/* vim: ft=c ts=4 sts=4 sw=4 ai et cin */
