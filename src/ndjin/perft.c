/* perft.c
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

#include "perft.h"

/* clang-format off */
struct perft_t initial_position[14] = {
    {
        0ULL,   1ULL,                   0ULL,       0ULL, 0ULL,
        0ULL,   0ULL,                   0ULL,       0ULL, 0ULL
    },
    {
        1,      20ULL,                  0ULL,       0ULL, 0ULL,
        0ULL,   0ULL,                   0ULL,       0ULL, 0ULL
    },
    {
        2,      400ULL,                 0ULL,       0ULL, 0ULL,
        0ULL,   0ULL,                   0ULL,       0ULL, 0ULL
    },
    {
        3,      8902ULL,                34ULL,      0ULL, 0ULL,
        0ULL,   12ULL,                  0ULL,       0ULL, 0ULL
    },
    {
        4,      197281ULL,              1576ULL,    0ULL, 0ULL,
        0ULL,   469ULL,                 0ULL,       0ULL, 8ULL
    },
    {
        5,      4865609ULL,             82719ULL,   258ULL,     0ULL,
        0ULL,   27351ULL,               6ULL,       0ULL,       347ULL
    },
    {
        6,      119060324ULL,           2812008ULL,  5248ULL,   0ULL,
        0ULL,   809099ULL,              329ULL,      46ULL,     10828ULL
    },
    {
        7,      3195901860ULL,          108329926ULL,   319617ULL,  883453ULL,
        0ULL,   33103848ULL,            18026ULL,       1628ULL,    435767ULL
    },
    {
        8,      84998978956ULL,         3523740106ULL,  7187977ULL, 23605205ULL,
        0ULL,   968981593ULL,           847039ULL,      147215ULL,  9852036ULL
    },
    {
        9,           2439530234167ULL,  125208536153ULL, 319496827ULL, 1784356000ULL,
        17334376ULL, 36095901903ULL,    37101713ULL,     5547231ULL,   400191963ULL
    },
    {
        10,     69352859712417ULL,
        0ULL,   0ULL,   0ULL,   0ULL,   0ULL,   0ULL,   0ULL,   0ULL
    },
    {
        11,     2097651003696806ULL,
        0ULL,   0ULL,   0ULL,   0ULL,   0ULL,   0ULL,   0ULL,   0ULL
    },
    {
        12,     62854969236701747ULL,
        0ULL,   0ULL,   0ULL,   0ULL,   0ULL,   0ULL,   0ULL,   0ULL
    },
    {
        13,     1981066775000396239ULL,
        0ULL,   0ULL,   0ULL,   0ULL,   0ULL,   0ULL,   0ULL,   0ULL
    },
    // {14, 61885021521585529237ULL,    0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL},
    // {15, 2015099950053364471960ULL,  0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL},
};

struct perft_t position_two[6] = {
    {
        1,              48ULL,          8ULL,           0ULL,       2ULL,
        0ULL,           0ULL,           0ULL,           0ULL,       0ULL
    },
    {
        2,              2039ULL,        351ULL,         1ULL,       91ULL,
        0ULL,           3ULL,           0ULL,           0ULL,       0ULL
    },
    {
        3,              97862ULL,       17102ULL,       45ULL,      3162ULL,
        0ULL,           993ULL,         0ULL,           0ULL,       1ULL
    },
    {
        4,              4085603ULL,     757163ULL,      1929ULL,    128013ULL,
        15172ULL,       25523ULL,       42ULL,          6ULL,       43ULL
    },
    {
        5,              193690690ULL,   35043416ULL,    73365ULL,   4993637ULL,
        8392ULL,        3309887ULL,     19883ULL,       2637ULL,    30171ULL
    },
    {
        6,              8031647685ULL,  1558445089ULL,  3577504ULL, 184513607ULL,
        56627920ULL,    92238050ULL,    568417ULL,      54948ULL,   360003ULL
    },
};

struct perft_t position_three[8] = {
    {
        1,              14ULL,          1ULL,           0ULL,       0ULL,
        0ULL,           2ULL,           0ULL,           0ULL,       0ULL
    },
    {
        2,              191ULL,         14ULL,          0ULL,       0ULL,
        0ULL,           10ULL,          0ULL,           0ULL,       0ULL
    },
    {
        3,              2812ULL,        209ULL,         2ULL,       0ULL,
        0ULL,           267ULL,         3ULL,           0ULL,       0ULL
    },
    {
        4,              43238ULL,       3348ULL,        123ULL,     0ULL,
        0ULL,           1680ULL,        106ULL,         0ULL,       17ULL
    },
    {
        5,              674624ULL,      52051ULL,       1165ULL,    0ULL,
        0ULL,           52950ULL,       1292ULL,        3ULL,       0ULL
    },
    {
        6,              11030083ULL,    940350ULL,      33325ULL,   0ULL,
        7552ULL,        452473ULL,      26067ULL,       0ULL,       2733ULL
    },
    {
        7,              178633661ULL,   14519036ULL,    294874ULL,  0ULL,
        140024ULL,      12797406ULL,    370630ULL,      3612ULL,    87ULL
    },
    {
        8,              3009794393ULL,  267586558ULL,   8009239ULL, 0ULL,
        6578076ULL,     135626805ULL,   7181487ULL,     1630ULL,    450410ULL
    },
};
/* clang-format on */

/* vim: ft=c ts=4 sts=4 sw=4 ai et cin */
