# Changes from upstream

This is the Asciiverse packaging of Asciicker, Marcin Sokalski's (gumix)
3D ASCII action RPG ("Let's kick some ASCII", asciicker.com), MIT. Upstream
is github.com/msokalski/asciicker (the `upstream` remote); this branch,
`asciiverse`, starts from its master at `8ff75d0` (2024-01-08, the merge of
`pre-y9`), with the full history.

Upstream builds the same game for the browser (Emscripten), an X11/GL
window, Windows and the terminal. Asciiverse ships the terminal game only,
built by `asciiverse/Makefile`. Every change to the game's own files is
behind a define that upstream's makefiles do not set, so their builds are
as before.

## Imported

- `asciiverse/quickjs/`: QuickJS-ng 0.17.0, the engine's source files
  unchanged from the `v0.17.0` tag (archive sha256
  `559bc4c420475e55c7ab4510adbc562f55d7524d75e8e89d79ce4bb02f5687d9`), MIT.
- `asciiverse/miniaudio/`: miniaudio 0.11.25, playback only, as in our
  ASCII Patrol packaging; public domain or MIT No Attribution.

## Changes

- **QuickJS instead of V8** (`AV_QUICKJS`). The game embeds a JavaScript
  engine for its scripting console: a chat line (Tab) starting with `\`
  runs as script, with the `ak` API to the game (position, items, say,
  frame callbacks). Upstream links a V8 monolith built by hand, which made
  the game very hard to build. `asciiverse/js.cpp` gives `init_v8()`,
  `free_v8()`, `akAPI_Exec()` and `akAPI_CB()` on QuickJS; game_app.cpp
  leaves its V8 code out when `AV_QUICKJS` is set. `akPrint` writes to
  stderr instead of over the screen.
- **Sound through miniaudio** (`AV_MINIAUDIO`, audio.cpp). It opens what
  the system has (PulseAudio, PipeWire or ALSA on Linux, Core Audio on
  macOS) and loads it at run time; without sound the game runs silent
  instead of not starting. The game's mixer and samples are unchanged.
- **Files** (`AV_PATHS`, game_app.cpp):
  - the game finds its data beside its binary also when started through
    PATH (`/proc/self/exe`, `_NSGetExecutablePath`; `realpath(argv[0])`
    looked in the current folder);
  - `asciicker.cfg` is kept in `$XDG_DATA_HOME/asciicker`, else
    `~/.local/share/asciicker`; an existing `~/asciicker.cfg` is used
    where it is;
  - no `keylog.txt` of every key pressed in the current folder (a debug
    log; where that folder is read-only, fprintf on a null FILE crashed).
- **Frame rate** (`AV_FPS=60`, game_app.cpp). The loop polled input
  without waiting and drew as fast as it could (about 480 frames a second,
  a whole core); it now waits for input until the next frame is due.
  About a sixth of a core at 100x30.
- **Fills big terminals** (`AV_CENTRE`, game_app.cpp, mainmenu.cpp).
  Upstream stopped the picture at 160x90 and drew it in the top left
  corner; it now grows up to 320x180, the size `Print()`'s output buffer
  is made for, and is centred beyond that, with mouse positions moved to
  match and a resize clearing what is left around it. The main menu's
  dithering rows were fixed arrays of 160 columns, which a wider menu ran
  past (heap corruption at 256x90); they are 320 now.
- **Only changed cells** (`AV_DIFF`, game_app.cpp). Each frame sends only
  the cells that changed since the last one; upstream sent the whole
  screen every frame (about 45 KB at 100x30). A resize or a clear sends
  the next frame whole.
- **The game's own colours** (`AV_COLOURS`, game_app.cpp). The game's
  palette is an even cube (0, 51, ... 255), unlike xterm's. Upstream
  redefined colours 16 to 231 of an xterm-like terminal with OSC 4 and
  never set them back, so the terminal's palette stayed changed after
  quitting (and recordings ignore it). Now the colours go out as
  truecolor where `COLORTERM` is `truecolor` or `24bit`, else as the
  nearest of the terminal's standard 256.
- **No messages on the screen.** Startup messages (exec path, base path,
  terminal type, "MAIN MENU OK") go to stderr only when it is not the
  terminal, so nothing is left in the scrollback.
- **Gamepads only** (`AV_GAMEPAD`, game_app.cpp). The game took the
  first `/dev/input/js*` it could open as a gamepad. Graphics tablet pads
  (a Wacom Cintiq's pad and ExpressKey Remote) and 3D mice (3Dconnexion)
  are joystick devices too, and their resting axes turned the camera
  without end. Only devices with gamepad or joystick buttons (0x120 to
  0x13f) and no digitizer buttons (0x140 to 0x14f, which the Wacom pads
  report even while claiming gamepad buttons) are used now.
- **Esc opens the game menu** (`AV_ESC_MENU`, game.cpp) when no chat box
  or inventory is open, and closes it again; in the terminal only `\`
  did.
- **The kitty keyboard protocol** (`AV_KITTY`, game_app.cpp). At start
  the game asks the terminal (`CSI ? u`); kitty, Ghostty, foot, WezTerm
  and others answer, and then every key arrives with its press, repeat
  and release, which the game gets as `KEYB_DOWN` and `KEYB_UP` as in its
  window builds: held keys walk and turn smoothly, and keys are let go
  when the window loses focus. Punctuation goes to the game as characters
  only, as it always did in the terminal (the window builds' debug mount
  keys sit on `,` `.` `/`). Ctrl+C still quits. `ASCIICKER_KITTY=0`
  turns it off, `=1` on without asking. Upstream's own kitty mode, for
  kitty's old `?2017` protocol, is switched off in its code and filled an
  array nothing read.
- **Keys without the protocol** (`AV_KEYS`). WASD and Q/E reached the game
  only as characters, which it reads for chat, so only the arrows walked
  and the view could not be turned from the keyboard; they now walk and
  turn. A key with no release is held until its repeat stops (560 ms
  until the first repeat, 120 ms after), where upstream let go after
  50 ms, before the first repeat came, so walking stuttered.
- **Help panels** (`AV_HELP`, `asciiverse/help.cpp`). CONTROLS >
  KEYBOARD and MOUSE, empty menu entries upstream, show the keys and the
  mouse controls, in the main menu and the game menu; H or ? shows the
  keys during play. Any key or click closes them.
- **`ASCIICKER_FPS`** sets the frame rate (60 by default); the catalog
  preview is recorded at 25 by `asciiverse/record-preview.sh`.

## Not changed

- Online play needs `-url` with a server; the asciicker.com servers did
  not answer on 2026-10-21, so the catalog presents the single-player
  game. `game_svr.cpp` (a LAN server) is not built.
- Keys: upstream's kitty mode is switched off in its own code, so keys
  are held by repeat with a timeout, as upstream plays in other terminals.
- The main menu's New Game offers upstream's "PRE Y9" world and a "TEST"
  submenu whose entries all start the same game; Credits is empty upstream.
- Linux console: on a real tty the game loads its CP437 font with
  `setfont` and restores the old one on exit, as upstream; the `.psf`
  fonts ship for that.

## Licences

Asciicker is MIT (LICENSE, "Copyright (c) 2021 gumix / msokalski"); the
repository's art, meshes, world and samples carry no other notice.
QuickJS-ng and miniaudio ship their licences as QUICKJS-LICENSE and
MINIAUDIO-LICENSE in each archive.

## Ours

- **How to play it, at the top of README.md.** A short note on installing and
  starting it with av, or from a release archive; the original text follows
  unchanged.
