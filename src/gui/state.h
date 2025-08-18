/* state.h
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

#ifndef STATE_H
#define STATE_H

#include <netinet/in.h>

struct game_t {
    char code[16];

    /* 0 1 - player 1 white
     * 1 0 - player 2 white
     */
    unsigned int white  : 2;

    /* 0 1 - player 1
     * 1 0 - player 2
     */
    unsigned int player : 2;

    struct state_t *game_state;

    /* 0000 0000 0000 0000 0011 1111 - source
     * 0000 0000 0000 1111 1100 0000 - target
     * 0000 0000 1111 0000 0000 0000 - piece
     * 0000 1111 0000 0000 0000 0000 - promotion piece
     * 0001 0000 0000 0000 0000 0000 - capture
     * 0010 0000 0000 0000 0000 0000 - double
     * 0100 0000 0000 0000 0000 0000 - e.p
     * 1000 0000 0000 0000 0000 0000 - castle
     */
    unsigned int last_move;
};

#endif /* STATE_H */

/* vim: ft=c ts=4 sts=4 sw=4 ai et cin */
