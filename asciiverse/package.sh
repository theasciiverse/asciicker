#!/bin/sh
# Packs dist/asciicker-<target>.tar.gz for av: the binary at the top with
# the game data it reads from beside itself, the licences, PATCHES.md and
# controls.txt. Only the world the game loads (a3d/game_map_y8.a3d), the
# .ogg samples and the console fonts (.psf, for setfont on a Linux tty)
# are taken from those folders.
#
#   asciiverse/package.sh x86_64-unknown-linux-gnu
set -eu
cd "$(dirname "$0")/.."
target="$1"
stage=stage/asciicker
rm -rf "$stage" && mkdir -p "$stage/a3d" "$stage/samples" "$stage/fonts" dist
cp out/asciicker LICENSE PATCHES.md controls.txt "$stage/"
cp asciiverse/quickjs/LICENSE "$stage/QUICKJS-LICENSE"
cp asciiverse/miniaudio/LICENSE "$stage/MINIAUDIO-LICENSE"
cp -r meshes sprites palettes images "$stage/"
cp a3d/game_map_y8.a3d "$stage/a3d/"
cp samples/*.ogg "$stage/samples/"
cp fonts/*.psf "$stage/fonts/"
tar -czf "dist/asciicker-$target.tar.gz" -C "$stage" .
ls -l dist
