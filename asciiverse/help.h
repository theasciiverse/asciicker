#ifndef AV_HELP_H
#define AV_HELP_H

// 0: no help panel, 1: keyboard, 2: mouse
extern int av_help;

void AV_PaintHelp(AnsiCell* ptr, int width, int height);
// true when the event was taken by the help (opening or closing it)
bool AV_HelpKeyb(GAME_KEYB keyb, int key);
bool AV_HelpMouse(GAME_MOUSE mouse);

#endif
