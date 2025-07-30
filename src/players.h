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

#ifndef PLAYERS_H
#define PLAYERS_H

#include <sys/types.h>
#include <stdint.h>

struct game_state_t {
    unsigned int player : 2;
    unsigned int winner : 2;

    uint32_t turn_count;
    uint32_t player1_count;
    uint32_t player2_count;
    uint32_t inactive_run;

    uint32_t round_start;
    uint32_t player1_remaining;
    uint32_t player2_remaining;

    uint16_t player1_score;
    uint16_t player2_score;

    char log_messages[256];
};

struct player_state_t {
    unsigned int active : 2;
    unsigned int pieces : 2;

    uint8_t  score;
    uint16_t move_count;
    uint16_t inactivity_count;

    uint32_t start_time;
    uint32_t remaining_time;

    char user_banner[32];
};

int pair_players(struct player_state_t *players, size_t player_count,
                 struct player_state_t *p1, struct player_state_t *p2);
int init_game(struct game_state_t *gs, struct player_state_t *p1,
              struct player_state_t *p2);
int play_game(struct game_state_t *gs, struct player_state_t *p1,
              struct player_state_t *p2);
int end_game(struct game_state_t *gs, struct player_state_t *p1,
             struct player_state_t *p2, int outcome);

#endif /* PLAYERS_H */
