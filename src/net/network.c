/* network.c
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

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <poll.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include <ndjin/types.h>

#include "network.h"

#ifndef NO_DEBUG
#define DEBUG(...)                                                             \
    do {                                                                       \
        fprintf(stderr, "%d: %s    ", __LINE__, __FILE__);                     \
        fprintf(stderr, __VA_ARGS__);                                          \
    } while (0);
#else
#define NO_DEBUG 1
#define DEBUG(...)
#endif

extern struct state_t *state;
extern int             apply_move(void *state, unsigned int move);

const int PORT                     = 54355;

const header_n NDJIN_PING          = PING_PONG(PING_NUMBER, 1);
const header_n NDJIN_PONG          = PING_PONG(PONG_NUMBER, 2);

const header_n NDJIN_HANDSHAKE     = HANDSHAKE_NUM(30, 1);
const header_n NDJIN_ACK_HANDSHAKE = HANDSHAKE_NUM(30, 2);

const header_n NDJIN_BAD_MOVE =
        NEW_HEADER_56(BAD_MOVE_NUMBER, MAGIC_NUMBER, 0x00000005, VERSION);

int   connection_stage = handshake;
gms_t stack;
int   ping_interval;

struct p2p connection = {0};

char *get_external_ip(void)
{
    char cmd[]     = "GET / HTTP/1.1\nHost: whatismyip.akamai.com\n\n";
    char buf[1000] = {0};
    int  rval, s = 0;

    struct addrinfo hints = {0}, *addrs;
    hints.ai_family       = AF_INET;
    hints.ai_socktype     = SOCK_STREAM;
    hints.ai_protocol     = 0;

    rval = getaddrinfo("whatismyip.akamai.com", "80", &hints, &addrs);
    if (rval != 0) {
        fprintf(stderr, "getaddrinfo failed: %s\n", gai_strerror(rval));
        goto cleanup;
    }

    s = socket(addrs->ai_family, addrs->ai_socktype, addrs->ai_protocol);
    if (s == -1) {
        fprintf(stderr, "get_ip(): connection socket failed\n");
        goto cleanup;
    }

    rval = connect(s, addrs->ai_addr, addrs->ai_addrlen);
    if (rval == -1) {
        fprintf(stderr, "get_ip(): connect failed\n");
        goto cleanup;
    }

    rval = send(s, cmd, strlen(cmd), 0);
    if (rval == -1) {
        fprintf(stderr, "get_ip(): send failed\n");
        goto cleanup;
    }

    rval = recv(s, buf, sizeof(buf), 0);
    if (rval == -1) {
        fprintf(stderr, "get_ip(): recv failed\n");
        goto cleanup;
    }

    char *start = buf, *end;
    end         = strchr(start, '\n');
    if (!strncmp(start, "HTTP/1.1 200 OK", end - start - 1)) {
        while (!(end[1] == '\r' && end[2] == '\n')) {
            start = end + 2;
            end   = strchr(start, '\n');
        }
        start = end + 3;
        end   = strchr(start, '\n');
        if (end)
            *end = 0;
        close(s);
        freeaddrinfo(addrs);
        return start;
    }

    close(s);
    freeaddrinfo(addrs);
cleanup:
    return "";
}

gms_t new_stack()
{
    gms_t stack = {0};
    return stack;
}

int push_to_stack(gms_t *stack, game_msg_n msg)
{
    if (stack->size == 4)
        return 1;
    stack->stack[stack->size++] = msg;
    return 0;
}

game_msg_n pop_from_stack(gms_t *stack)
{
    if (stack->size == 0)
        return (( game_msg_n )1);
    game_msg_n rval;
    rval                      = stack->stack[stack->size];
    stack->stack[stack->size] = 0;
    --stack->size;
    return rval;
}

int establish(int domain, int port)
{
    struct sockaddr_in addr      = {0};
    struct sockaddr_in peer_addr = {0};
    int                recv_sock = 0;
    int                send_sock = 0;

    if ((recv_sock = socket(CONN_PROT, CONN_TYPE, 0) < 0)) {
        fprintf(stderr, "establish(): failed to create receiver socket file "
                        "descriptor\n");
        return 1;
    }

    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(PORT);

    DEBUG("Local IP Address: %s:%d\n", inet_ntoa(addr.sin_addr),
          ( int )ntohs(addr.sin_port))

    if (bind(recv_sock, ( struct sockaddr * )&addr, sizeof(addr)) < 0) {
        fprintf(stderr, "establish(): failed to bind socket to address:port\n");
        return 2;
    }

    if (listen(recv_sock, 1) < 0) {
        fprintf(stderr, "establish(): failed to listen on socket\n");
        return 3;
    }

    if (fcntl(recv_sock, F_SETFL, O_NONBLOCK) != 0) {
        fprintf(stderr, "establish(): failed to set receiving socket file "
                        "descriptor as non-blocking\n");
        return 4;
    }

    connection.recv_address = &addr;
    connection.recv_port    = PORT;
    connection.recv_socket  = recv_sock;
    connection.listening    = 1;

    if ((send_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "establish(): failed to create sending socket file "
                        "descriptor\n");
        return 5;
    }

    peer_addr.sin_family      = AF_INET;
    peer_addr.sin_addr.s_addr = INADDR_ANY;
    peer_addr.sin_port        = htons(port);

    if (connect(send_sock, ( struct sockaddr * )&peer_addr, sizeof(peer_addr)) <
        0) {
        fprintf(stderr, "establish(): failed to connect to peer's socket\n");
        return 6;
    }

    connection.send_address = &peer_addr;
    connection.send_port    = port;
    connection.send_socket  = send_sock;
    connection.connected    = 1;
    if (fcntl(send_sock, F_SETFL, O_NONBLOCK) != 0) {
        fprintf(stderr, "establish(): failed to set sending socket file "
                        "descriptor as non-blocking\n");
        return 7;
    }

    int cross_thread_pipes[2];
    if (pipe(cross_thread_pipes) == -1) {
        fprintf(stderr,
                "establish(): failed to create thread communicating pipe\n");
        return 8;
    }

    stack                    = new_stack();

    pthread_mutex_t sm_mu    = PTHREAD_MUTEX_INITIALIZER;
    connection.send_stack_mu = sm_mu;

    pthread_t lt_id;
    pthread_create(&lt_id, NULL, &receiving_thread, ( void * )&connection);
    pthread_t sd_id;
    pthread_create(&sd_id, NULL, &sending_thread, ( void * )&connection);

    connection.receiving_thread_pid = lt_id;
    connection.sending_thread_pid   = sd_id;

    return 0;
}

void *receiving_thread(void *connection)
{
    int recv_fd      = (( struct p2p * )connection)->recv_socket;

    time_t  tslp     = time(NULL);
    ssize_t recv_len = 0;
    while (1) {
        if (time(NULL) - tslp >= ping_interval) {
            send_header((( struct p2p * )connection)->send_socket, NDJIN_PING);
            tslp = time(NULL);
            ++(( struct p2p * )connection)->awaiting_pong;
        }
        recv_len      = 0;
        char buf[256] = {0};

        if ((recv_len = recv(recv_len, buf, sizeof(buf), 0)) < 0) {
            if (errno == ENOTCONN || errno == ECONNREFUSED) {
                pthread_cancel(
                        (( struct p2p * )connection)->receiving_thread_pid);
                disconnect((( struct p2p * )connection));
            }
        }

        if (!recv_len)
            continue;
        DEBUG("Received: %ld bytes from connected peer\n", recv_len)

        unsigned int header = 0;
        memcpy(&header, buf, sizeof(unsigned int));
        char data[recv_len - sizeof(unsigned int)];
        memcpy(data, buf + sizeof(unsigned int),
               recv_len - sizeof(unsigned int));

        if (connection_stage > ack_shake) {
            if ((header & 0xFFFFFFFF) == PING_NUMBER) {
                if ((( struct p2p * )connection)->awaiting_pong >= 5) {
                    pthread_cancel(
                            (( struct p2p * )connection)->receiving_thread_pid);
                    disconnect((( struct p2p * )connection));
                    return 0;
                }
                header_n seq = ((( header_n )(header & 0xF0) << 48) >> 48);
                send_header((( struct p2p * )connection)->send_socket,
                            NDJIN_PING);
            } else if ((header & 0xFFFFFFFF) == PONG_NUMBER) {
                (( struct p2p * )connection)->awaiting_pong = 0;
            }
        }

        switch (connection_stage) {
        case handshake:
            if (header == NDJIN_HANDSHAKE) {
                connection_stage = ack_shake;
                send_header((( struct p2p * )connection)->send_socket,
                            HANDSHAKE_NUM(HANDSHAKE_NUMBER, 2));
            }
            break;

        case ack_shake:
            if (header == NDJIN_ACK_HANDSHAKE) {
                ping_interval    = ((header >> 28) & 0xFF);
                connection_stage = propose_init;
                char buf[96]     = {0};
                snprintf(buf, 96,
                         "rnbqkbnr/pppppppp/8/8/8/8/"
                         "PPPPPPPP/RNBQKBNR w KQkq - 0 1 ");
                unsigned int *checks      = ( unsigned int * )buf;
                unsigned int  sync_header = (checks[0] & 0xFFE00000) |
                                           (checks[1] & 0x001FFB00) |
                                           (checks[2] & 0x000003FF);
                (( struct p2p * )connection)->proposed_init_check = sync_header;
                propose_state((( struct p2p * )connection)->send_socket,
                              NEW_INIT_178(buf, strlen(buf), sync_header));
            }
            break;

        case propose_init:
            if (header == (( struct p2p * )connection)->proposed_init_check) {
                char buf[96] = {0};
                snprintf(buf, 96,
                         "rnbqkbnr/pppppppp/8/8/8/8/"
                         "PPPPPPPP/RNBQKBNR w KQkq - 0 1 ");
                unsigned int *checks      = ( unsigned int * )data;
                unsigned int  sync_header = (checks[0] & 0xFFE00000) |
                                           (checks[1] & 0x001FFB00) |
                                           (checks[2] & 0x000003FF);
                if (header == sync_header) {
                    connection_stage = ack_init;
                    send_header((( struct p2p * )connection)->send_socket,
                                NEW_INIT_178(buf, strlen(buf), sync_header));
                }
            }
            break;

        case ack_init:
            if (header == 0xFFFFFFFF) {
                connection_stage = synced;
            }
            break;

        case synced:
            connection_stage = game;
            fcntl(recv_len, F_SETFL, O_SYNC);
            goto receive_game_loop;
        }
    }

    tslp                          = 0;
    unsigned int previous_counter = 1;
    unsigned int previous_move    = 0;
receive_game_loop:
    while (1) {
        recv_len                           = 0;
        char buf[sizeof(unsigned int) * 2] = {0};

        if ((recv_len = recv(recv_len, buf, sizeof(buf), 0)) < 0) {
            if (errno == ENOTCONN || errno == ECONNREFUSED) {
                pthread_cancel(
                        (( struct p2p * )connection)->receiving_thread_pid);
                disconnect((( struct p2p * )connection));
            }
        }

        if (!recv_len)
            continue;
        DEBUG("Received: %ld bytes from connected peer\n", recv_len)

        if ((recv_len = recv(recv_len, buf, sizeof(buf), 0)) <= 0) {
            if (errno == ENOTCONN || errno == ECONNREFUSED) {
                pthread_cancel(
                        (( struct p2p * )connection)->receiving_thread_pid);
                disconnect(connection);
                return 0;
            }
            if (!recv_len)
                continue;
            if ((( game_msg_n )(*( game_msg_n * )buf +
                                (sizeof(game_msg_n) * 2)) &
                 0x0000FFFF) == BAD_MOVE_NUMBER) {
                DEBUG("recveiving_thread: adding to shared stack to retry "
                      "move\n");
                pthread_mutex_lock(
                        &(( struct p2p * )connection)->send_stack_mu);
                push_to_stack(
                        ((( struct p2p * )connection)->send_message_stack),
                        previous_move);
                pthread_mutex_unlock(
                        &(( struct p2p * )connection)->send_stack_mu);
            }
        }

        game_msg_n   received = ( game_msg_n )(*( game_msg_n * )buf);
        unsigned int move     = received & 0xFFFFFFFF;
        unsigned int prev     = (received >> 32) & 0xFFFFFFFF;
        unsigned int header   = (received >> 48) & 0x0000FFFF;
        unsigned int counter  = (received >> 64) & 0x0000FFFF;
        int          rc       = 0;
        if ((rc = apply_move(state, move)) > 0) {
            pthread_mutex_lock(&(( struct p2p * )connection)->send_stack_mu);
            push_to_stack(((( struct p2p * )connection)->send_message_stack),
                          previous_move);
            pthread_mutex_unlock(&(( struct p2p * )connection)->send_stack_mu);
        } else if (rc < 0) {
            goto stop_listening;
        } else if (rc == 0) {
            pthread_mutex_lock(&(( struct p2p * )connection)->send_stack_mu);
            push_to_stack(((( struct p2p * )connection)->send_message_stack),
                          NEW_INIT_96(0x00000000, move, CURRENT_MOVE_NUMBER,
                                      counter));
            pthread_mutex_unlock(&(( struct p2p * )connection)->send_stack_mu);
        }
    }

stop_listening:
    disconnect((( struct p2p * )connection));
    pthread_mutex_destroy(&(( struct p2p * )&connection)->send_stack_mu);
    pthread_cancel((( struct p2p * )connection)->sending_thread_pid);
    pthread_cancel((( struct p2p * )connection)->receiving_thread_pid);
    return 0;
}

void *sending_thread(void *connection)
{
    time_t     tslp          = 0;
    game_msg_n previous_move = 0;
    ssize_t    recv_len      = 0;
    while (1) {
        if (time(NULL) - tslp >= ping_interval) {
            send_header((( struct p2p * )connection)->send_socket, NDJIN_PING);
            tslp = time(NULL);
            ++(( struct p2p * )connection)->awaiting_pong;
        }

        if (((( struct p2p * )connection)->send_message_stack)->size == 0)
            continue;

        unsigned int prev_move       = previous_move & 0xFFFFFFFF;
        unsigned int prev_prev       = (previous_move >> 32) & 0xFFFFFFFF;
        unsigned int header          = (previous_move >> 48) & 0x0000FFFF;
        unsigned int prev_counter    = (previous_move >> 64) & 0x0000FFFF;

        game_msg_n current_best_move = 0;
        pthread_mutex_lock(&(( struct p2p * )connection)->send_stack_mu);
        int stack_size =
                ((( struct p2p * )connection)->send_message_stack)->size;
        DEBUG("Received: %d messaged from shared stack\n", stack_size);
        pthread_mutex_unlock(&(( struct p2p * )connection)->send_stack_mu);
        while (stack_size) {
            pthread_mutex_lock(&(( struct p2p * )connection)->send_stack_mu);
            game_msg_n msg = pop_from_stack(
                    (( struct p2p * )connection)->send_message_stack);
            stack_size =
                    ((( struct p2p * )connection)->send_message_stack)->size;
            pthread_mutex_unlock(&(( struct p2p * )connection)->send_stack_mu);

            switch (header) {
            case PREVIOUS_MOVE_NUMBER:
                DEBUG("send_move(): retrying previous move\n");
                send_move((( struct p2p * )connection)->send_socket,
                          NEW_INIT_96(prev_move, prev_prev,
                                      PREVIOUS_MOVE_NUMBER, prev_counter));
                break;
            case CURRENT_MOVE_NUMBER:
                DEBUG("send_move(): sending best current move\n");
                unsigned int move      = apply_move(state, 0x00000000);
                game_msg_n   next_move = NEW_INIT_96(
                        move, prev_move, CURRENT_MOVE_NUMBER, prev_counter + 1);
                send_move((( struct p2p * )connection)->send_socket, next_move);
                break;
            default:
                send_move((( struct p2p * )connection)->send_socket,
                          NEW_INIT_96(prev_move, prev_prev, BAD_MOVE_NUMBER,
                                      prev_counter));
                break;
            }
        }
    }

    disconnect((( struct p2p * )connection));
    pthread_mutex_destroy(&(( struct p2p * )&connection)->send_stack_mu);
    pthread_cancel((( struct p2p * )connection)->sending_thread_pid);
    pthread_cancel((( struct p2p * )connection)->receiving_thread_pid);
    return 0;
}

int send_header(int socket, header_n header)
{
    char buf[sizeof(header_n)] = {0};
    memcpy(buf, &header, sizeof(header_n));
    return send(socket, buf, sizeof(header_n), 0);
}

int propose_state(int socket, initial_state_n proposed)
{
    char buf[sizeof(initial_state_n)] = {0};
    memcpy(buf, &proposed, sizeof(initial_state_n));
    return send(socket, buf, sizeof(initial_state_n) + 96, 0);
}

int send_move(int socket, game_msg_n move)
{
    char buf[sizeof(game_msg_n)] = {0};
    memcpy(buf, &move, sizeof(game_msg_n));
    return send(socket, buf, sizeof(game_msg_n), 0);
}

int disconnect(struct p2p *connection)
{
    if (close(connection->recv_socket) == -1) {
        fprintf(stderr, "disconnect(): error closing receiving socket file "
                        "descriptor\n");
        return 1;
    }
    if (close(connection->send_socket) == -1) {
        fprintf(stderr, "disconnect(): error closing sending socket file "
                        "descriptor\n");
        return 2;
    }
    pthread_mutex_destroy(&connection->send_stack_mu);
    pthread_cancel(connection->receiving_thread_pid);
    pthread_cancel(connection->sending_thread_pid);
    return 0;
}

/* vim: ft=c ts=4 sts=4 sw=4 ai et cin */
