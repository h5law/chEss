/* pre.c
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

#include <string.h>
#include <stdio.h>

#include "raylib.h"

#include <net/network.h>

#include "pre.h"

extern int win_width;
extern int win_height;

void title_box(const char *text, int xoff)
{
    Color box_colour  = DARKBROWN;
    box_colour.a     *= 0.66;
    DrawRectangle(win_width / 8, 70, (GetScreenWidth() * 3) / 4,
                  (GetScreenHeight() / 4) - 70, box_colour);
    DrawText(text, win_width / 8 + xoff - 5, 84, 72, DARKBROWN);
    DrawText(text, win_width / 8 + xoff, 80, 70, BEIGE);
}

#define TOP_BUTTON_XPOS      win_width / 8.0
#define TOP_BUTTON_WIDTH     (GetScreenWidth() * 3) / 4.0
#define TOP_BUTTON_YPOS      280
#define TOP_BUTTON_HEIGHT    (GetScreenHeight() / 4.0) - 50

#define BOTTOM_BUTTON_XPOS   win_width / 8.0 - 10
#define BOTTOM_BUTTON_WIDTH  (GetScreenWidth() * 3) / 4.0 + 20
#define BOTTOM_BUTTON_YPOS   400
#define BOTTOM_BUTTON_HEIGHT (GetScreenHeight() / 4.0) - 50

#define BUTTON_TEXT_FG       BEIGE
#define BUTTON_TEXT_BG       DARKBROWN

Color box_colour = {76, 63, 47, 210};

void top_button(const char *text, Color fg, Color bg)
{
    if (!text)
        return;
    DrawRectangle(TOP_BUTTON_XPOS, TOP_BUTTON_YPOS, TOP_BUTTON_WIDTH,
                  TOP_BUTTON_HEIGHT, box_colour);
    DrawText(text, win_width / 8 - 35 + ((GetScreenWidth() * 3) / 32),
             (GetScreenHeight() - 348), 72, bg);
    DrawText(text, win_width / 8 - 30 + ((GetScreenWidth() * 3) / 32),
             (GetScreenHeight() - 340), 70, fg);
}

void bottom_button(const char *text, Color fg, Color bg)
{
    if (!text)
        return;
    DrawRectangle(BOTTOM_BUTTON_XPOS, BOTTOM_BUTTON_YPOS, BOTTOM_BUTTON_WIDTH,
                  BOTTOM_BUTTON_HEIGHT, box_colour);
    DrawText(text, win_width / 8 - 40 + ((GetScreenWidth() * 3) / 32),
             (GetScreenHeight() - 220), 72, bg);
    DrawText(text, win_width / 8 - 37 + ((GetScreenWidth() * 3) / 32),
             (GetScreenHeight() - 215), 70, fg);
}

int handle_button_events(const char *top, const char *bottom)
{
    Vector2 mp = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if ((mp.x >= TOP_BUTTON_XPOS &&
             mp.x <= TOP_BUTTON_XPOS + TOP_BUTTON_WIDTH) &&
            (mp.y >= TOP_BUTTON_YPOS &&
             mp.y <= TOP_BUTTON_YPOS + TOP_BUTTON_HEIGHT)) {
            top_button(top, BUTTON_TEXT_BG, BUTTON_TEXT_FG);
        }
        if ((mp.x >= BOTTOM_BUTTON_XPOS &&
             mp.x <= BOTTOM_BUTTON_XPOS + BOTTOM_BUTTON_WIDTH) &&
            (mp.y >= BOTTOM_BUTTON_YPOS &&
             mp.y <= BOTTOM_BUTTON_YPOS + BOTTOM_BUTTON_HEIGHT)) {
            bottom_button(bottom, BUTTON_TEXT_BG, BUTTON_TEXT_FG);
        }
    } else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        if ((mp.x >= TOP_BUTTON_XPOS &&
             mp.x <= TOP_BUTTON_XPOS + TOP_BUTTON_WIDTH) &&
            (mp.y >= TOP_BUTTON_YPOS &&
             mp.y <= TOP_BUTTON_YPOS + TOP_BUTTON_HEIGHT)) {
            return 1;
        }
        if ((mp.x >= BOTTOM_BUTTON_XPOS &&
             mp.x <= BOTTOM_BUTTON_XPOS + BOTTOM_BUTTON_WIDTH) &&
            (mp.y >= BOTTOM_BUTTON_YPOS &&
             mp.y <= BOTTOM_BUTTON_YPOS + BOTTOM_BUTTON_HEIGHT)) {
            return 2;
        }
    }
    return 0;
}

void show_game_code(void)
{
    char ip[16] = {0};
    get_external_ip(ip);
    Rectangle text_box = {TOP_BUTTON_XPOS, TOP_BUTTON_YPOS, TOP_BUTTON_WIDTH,
                          TOP_BUTTON_HEIGHT - 25};

    Color tbc          = {200, 200, 200, 150};
    DrawRectangleRec(text_box, tbc);
    DrawRectangleLines(( int )text_box.x, ( int )text_box.y,
                       ( int )text_box.width, ( int )text_box.height, tbc);

    DrawText(( char * )ip, ( int )text_box.x + 15, ( int )text_box.y + 16, 60,
             DARKBROWN);

    draw_tb_buttons(NULL, "Waiting...");
}

int draw_tb_buttons(const char *top, const char *bottom)
{
    top_button(top, BUTTON_TEXT_FG, BUTTON_TEXT_BG);
    bottom_button(bottom, BUTTON_TEXT_FG, BUTTON_TEXT_BG);
    return handle_button_events(top, bottom);
}

void handle_code_input(char buf[16], Rectangle text_box, int mouse_over_text)
{
    if (CheckCollisionPointRec(GetMousePosition(), text_box))
        mouse_over_text = 1;
    else

        mouse_over_text = 0;

    if (mouse_over_text) {
        SetMouseCursor(MOUSE_CURSOR_IBEAM);
        int key = GetCharPressed();
        while (key > 0) {
            int len = strlen(buf);
            if ((key >= 46 && key != 47) && (key <= 57) && (len <= 16)) {
                buf[strlen(buf)] = ( char )key;
            }

            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE)) {
            int len = strlen(buf);
            if (len > 0)
                buf[--len] = '\0';
        }
    } else
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
}

void game_code_entry(char buf[16])
{
    buf[16]              = '\0';
    Rectangle text_box   = {TOP_BUTTON_XPOS, TOP_BUTTON_YPOS, TOP_BUTTON_WIDTH,
                            TOP_BUTTON_HEIGHT - 25};

    bool mouse_over_text = false;
    handle_code_input(buf, text_box, mouse_over_text);

    Color tbc = {200, 200, 200, 150};
    DrawRectangleRec(text_box, tbc);
    if (mouse_over_text)
        DrawRectangleLines(( int )text_box.x, ( int )text_box.y,
                           ( int )text_box.width, ( int )text_box.height,
                           DARKBROWN);
    else
        DrawRectangleLines(( int )text_box.x, ( int )text_box.y,
                           ( int )text_box.width, ( int )text_box.height, tbc);

    DrawText(( char * )buf, ( int )text_box.x + 16, ( int )text_box.y + 16, 60,
             DARKBROWN);
}

int splash_screen(void) { return 0; }

/* vim: ft=c ts=4 sts=4 sw=4 ai et cin */
