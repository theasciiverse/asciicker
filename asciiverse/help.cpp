// The help panels behind CONTROLS > KEYBOARD and MOUSE, which upstream left
// as empty menu entries, also shown with H (or ?) during a game. Drawn over
// whatever is on screen; any key or click closes them.

#include <string.h>

#include "../render.h"
#include "../platform.h"
#include "../game.h"
#include "help.h"

int av_help = 0;

static const char* keyboard_lines[] =
{
    "KEYBOARD",
    "",
    "Arrows, WASD    walk",
    "Q / E           turn the view",
    "Space           jump",
    "Enter           attack (with a weapon in hand)",
    "1 ... 9         pick up the item shown below you",
    "I               open and close the inventory",
    "U               use or wear the highlighted item",
    "Y               take the highlighted item to move it",
    "0               drop the highlighted item",
    "Tab             chat; Tab again sends the line",
    "                (a line starting with \\ runs as script)",
    "Esc or \\        game menu: video, sound, exit",
    "H or ?          this help",
    "Ctrl+C          quit",
    "",
    "Held keys are smooth in terminals with the kitty",
    "keyboard protocol (kitty, Ghostty, foot, WezTerm).",
    0
};

static const char* mouse_lines[] =
{
    "MOUSE",
    "",
    "Hold the left button    walk toward the pointer",
    "Left held, then right   jump",
    "Right button, drag      turn the view",
    "Click an item below     pick it up",
    "Drag into inventory     put it in your pack",
    "Drag out of inventory   drop it",
    "Click your portrait     open the inventory",
    "",
    "A gamepad works too: the left stick walks.",
    0
};

static void put(AnsiCell* ptr, int w, int h, int x, int y, uint8_t gl, uint8_t fg, uint8_t bk)
{
    if (x < 0 || y < 0 || x >= w || y >= h)
        return;
    // the buffer's first row is the bottom of the screen
    AnsiCell* c = ptr + (h - 1 - y) * w + x;
    c->gl = gl;
    c->fg = fg;
    c->bk = bk;
}

static uint8_t rgb(int r, int g, int b) { return 16 + 36 * r + 6 * g + b; }

void AV_PaintHelp(AnsiCell* ptr, int width, int height)
{
    if (!av_help)
        return;
    const char** lines = av_help == 2 ? mouse_lines : keyboard_lines;
    int n = 0, wide = 0;
    while (lines[n])
    {
        int l = (int)strlen(lines[n]);
        if (l > wide)
            wide = l;
        n++;
    }
    const char* foot = "any key closes this";
    int w = wide + 6, h = n + 2;
    int x0 = (width - w) / 2, y0 = (height - h) / 2;
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;

    uint8_t bk = rgb(0, 0, 1), border = rgb(5, 4, 0), text = rgb(5, 5, 5), keys = rgb(5, 5, 2), dim = rgb(2, 3, 4);
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
        {
            uint8_t gl = ' ';
            if (y == 0 || y == h - 1)
                gl = x == 0 ? (y == 0 ? 0xC9 : 0xC8) : x == w - 1 ? (y == 0 ? 0xBB : 0xBC) : 0xCD;
            else if (x == 0 || x == w - 1)
                gl = 0xBA;
            put(ptr, width, height, x0 + x, y0 + y, gl, border, bk);
        }
    for (int i = 0; i < n; i++)
    {
        const char* s = lines[i];
        int l = (int)strlen(s);
        // the first line is the title; the key column runs to the first double space
        int keycol = 0;
        const char* gap = strstr(s, "  ");
        if (i > 0 && gap && s[0] != ' ')
            keycol = (int)(gap - s);
        for (int k = 0; k < l; k++)
        {
            uint8_t fg = i == 0 ? border : k < keycol ? keys : text;
            if (i > 0 && !keycol)
                fg = dim;
            put(ptr, width, height, x0 + 3 + k, y0 + 1 + i, (uint8_t)s[k], fg, bk);
        }
    }
    int fl = (int)strlen(foot);
    for (int k = 0; k < fl; k++)
        put(ptr, width, height, x0 + w - 2 - fl + k, y0 + h - 1, (uint8_t)foot[k], dim, bk);
}

bool AV_HelpKeyb(GAME_KEYB keyb, int key)
{
    // with the kitty protocol a key comes as a key down and then its
    // character: the key that closes the panel must not also act in the game
    static bool swallow_char = false;
    if (keyb == GAME_KEYB::KEYB_UP)
    {
        swallow_char = false;
        return false;   // releases still go to the game, so no key stays held
    }
    if (swallow_char && keyb == GAME_KEYB::KEYB_CHAR)
    {
        swallow_char = false;
        return true;
    }
    if (av_help)
    {
        av_help = 0;
        swallow_char = keyb == GAME_KEYB::KEYB_DOWN;
        return true;
    }
    if (keyb == GAME_KEYB::KEYB_CHAR && (key == 'h' || key == 'H' || key == '?'))
    {
        av_help = 1;
        return true;
    }
    return false;
}

bool AV_HelpMouse(GAME_MOUSE mouse)
{
    if (!av_help)
        return false;
    if (mouse == GAME_MOUSE::MOUSE_LEFT_BUT_DOWN || mouse == GAME_MOUSE::MOUSE_RIGHT_BUT_DOWN)
        av_help = 0;
    return true;
}
