/* network.h
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

#ifndef NETWORK_H
#define NETWORK_H

#include <stdbit.h>
#include <stdlib.h>
#include <pthread.h>
#include <netinet/in.h>

#define PORT            54355
#define COMS_PORT       55345

#define CONN_PROT       AF_INET
#define CONN_TYPE       SOCK_STREAM
#define CONN_ADDR       INADDR_ANY

#define VERSION         0x00000001
#define MAGIC_NUMBER    0x00005157

#define DOMAIN_EXCHANGE 0xD0D05050
#define PING_NUMBER     0xDEADBEEF
#define PONG_NUMBER     0xBEEFBABE

#ifndef PING_INTERVAL
#define PING_INTERVAL 30
#endif /* PING_INTERVAL */

#define HANDSHAKE_NUMBER     0x1E015A23
#define HANDSHAKE_ACK_NUMBER 0x1E025A23
#define HANDSHAKE_GREET      0xBAD05157
#define HANDSHAKE_STEP_ONE   0x00000005
#define HANDSHAKE_ACK        0xBAD05157
#define HANDSHAKE_STEP_TWO   0x0000000A

#define BAD_MOVE_NUMBER      0xDEAD
#define CURRENT_MOVE_NUMBER  0xC247
#define PREVIOUS_MOVE_NUMBER 0x9234
#define PRODUCE_MOVE_NUMBER  0x920D

#define PING_SECONDS         ((( unsigned int )HANDSHAKE_NUMBER & 0xFF000000) >> 24)
#define SHAKE_NUM(n)         ((( unsigned int )n * 0x00FF0000) >> 16)

#define DEFAULT_BOARD                                                          \
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "

enum { handshake, ack_shake, propose_init, ack_init, synced, game, complete };

/*
 * struct initial_state {
 *     char               fen_string[96];
 *     unsigned int       fen_size : 8;
 *     unsigned long long checksum : 64;
 * }
 */
typedef _BitInt(168) initial_state_n;

/*
 * struct header {
 *     unsigned int data         : 32;
 *     unsigned int magic_number : 16;
 *     unsigned int status       : 4;
 *     unsigned int version      : 4;
 * }
 */
typedef _BitInt(56) header_n;

/*
 * struct game_message {
 *     unsigned int encoded_move      : 32;
 *     unsigned int previous          : 32;
 *     unsigned int header            : 16;
 *     unsigned int full_move_counter : 16;
 * }
 */
typedef _BitInt(96) game_msg_n;

#define NEW_INIT_178(fen, size, check)                                         \
    (( initial_state_n )0 |                                                    \
     (( initial_state_n )(*( unsigned int * )( char * )fen)) |                 \
     ((( initial_state_n )size & 0x000000FF) << 96) |                          \
     ((( initial_state_n )check & 0xFFFFFFFF) << 104))

#define NEW_HEADER_56(data, magic, status, version)                            \
    (( header_n )0 | (( header_n )data & 0xFFFFFFFF) |                         \
     ((( header_n )magic & 0x0000FFFF) << 32) |                                \
     ((( header_n )status & 0x0000000F) << 48) |                               \
     ((( header_n )version & 0x0000000F) << 52))

#define NEW_INIT_96(move, prev, head, fmc)                                     \
    (( game_msg_n )0 | (( game_msg_n )move & 0xFFFFFFFF) |                     \
     ((( game_msg_n )prev & 0xFFFFFFFF) << 32) |                               \
     ((( game_msg_n )head & 0x0000FFFF) << 64) |                               \
     ((( game_msg_n )fmc & 0x0000FFFF) << 80))

typedef struct {
    game_msg_n   stack[32];
    unsigned int size;
} gms_t;

struct p2p {
    struct sockaddr_in *recv_address;
    int                 recv_port;
    int                 recv_socket;
    int                 listening;

    struct sockaddr_in *send_address;
    int                 send_port;
    int                 send_socket;
    int                 connected;

    pid_t receiving_thread_pid;
    pid_t sending_thread_pid;

    unsigned int proposed_init_check;
    int          synced;
    int          ping_interval;
    int          awaiting_pong;

    pthread_mutex_t send_stack_mu;
    gms_t           send_message_stack[8];
};

gms_t      new_stack(void);
int        push_to_stack(gms_t *stack, game_msg_n msg);
game_msg_n pop_from_stack(gms_t *stack);

#define PING_PONG(num, seq) NEW_HEADER_56(num, MAGIC_NUMBER, seq, VERSION)

#define HANDSHAKE_NUM(ping_interval, shake_step)                               \
    NEW_HEADER_56(( header_n )((( header_n )ping_interval & 0xF) << 28) |      \
                          ((( header_n )shake_step & 0xF) << 24) |             \
                          (( header_n )0x0000 << 16),                          \
                  MAGIC_NUMBER, HANDSHAKE_STEP_ONE, VERSION)

int  listen_for_domain(char domain[16]);
int  send_domain(char domain[16]);
void get_external_ip(char buf[16]);

int establish(in_addr_t domain);

void *receiving_thread(void *connection);

void *sending_thread(void *connection);

int send_header(int socket, header_n header);
int send_header(int socket, header_n header);

int propose_state(int socket, initial_state_n proposed_header);

int send_move(int socket, game_msg_n move);
int recv_move(int socket, game_msg_n move);

int disconnect(struct p2p *connection);

#endif /* NETWORK_H */

/* vim: ft=c ts=4 sts=4 sw=4 ai et cin */
