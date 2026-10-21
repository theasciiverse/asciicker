#!/bin/sh
# Records the catalog preview, asciiverse/asciicker.cast, at 100x30: the
# title, then a walk through the world played by keys sent into a tmux
# session that runs asciinema. Run from the repo root after building
# (make -f asciiverse/Makefile); needs tmux and asciinema 3.
#
#   asciiverse/record-preview.sh [out.cast]
set -eu
cd "$(dirname "$0")/.."
out=${1:-asciiverse/asciicker.cast}
run=$(mktemp -d)
trap 'tmux kill-session -t avrec 2>/dev/null; rm -rf "$run"' EXIT

# the game reads its data beside its binary: link it all into one folder
for f in meshes sprites palettes images a3d samples fonts; do ln -s "$PWD/$f" "$run/$f"; done
cp out/asciicker "$run/"
mkdir "$run/home"

tmux new-session -d -s avrec -x 100 -y 30 \
  "cd '$run' && HOME='$run/home' XDG_DATA_HOME='$run/home/.local/share' \
   USER=player LOGNAME=player TERM=xterm-256color COLORTERM=truecolor LANG=C.UTF-8 \
   ASCIICKER_FPS=25 PATH='$run':\"\$PATH\" \
   asciinema rec --overwrite --window-size 100x30 --idle-time-limit 2 \
     --capture-env TERM,COLORTERM,LANG --title Asciicker --command asciicker \
     '$run/take.cast' 2>/dev/null"

key() { tmux send-keys -t avrec "$@"; }
# hold a key: repeated presses, as a terminal's key repeat sends them
hold() { n=$2; while [ "$n" -gt 0 ]; do key "$1"; sleep 0.05; n=$((n - 1)); done; }

sleep 4                      # title screen
key Right; sleep 1           # New Game
key Enter; sleep 3           # PRE Y9
hold Up 30
hold e 25                    # turn the view
hold Up 30
key Space; sleep 0.6         # jump
hold Right 30
hold q 40
hold Up 15
key Enter; sleep 0.4; key Enter; sleep 0.4; key Enter; sleep 0.6   # swing
hold e 30
hold Down 20
key i; sleep 2.5; key i; sleep 0.5   # inventory
hold Left 25
hold q 25
hold Up 20
sleep 1
key -l '\'; sleep 1.5     # the game menu (backslash)
for n in 1 2 3 4 5 6; do key Down; sleep 0.2; done   # EXIT?, the last entry
key Enter; sleep 1
key Down; sleep 0.6          # YES
key Enter; sleep 2
cp "$run/take.cast" "$out"
ls -l "$out"
