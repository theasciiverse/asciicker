# The terminal game, built without V8 (QuickJS from asciiverse/quickjs
# instead) and without gpm. Run from the repo root:
#
#   make -f asciiverse/Makefile            # out/asciicker
#
# CC, CXX, CFLAGS, CXXFLAGS, LDFLAGS and LIBS can be set from outside.

CC ?= cc
CXX ?= c++
CFLAGS ?= -O2
CXXFLAGS ?= -O2
LIBS ?=

OUT := out
OBJ := $(OUT)/obj

GAME := game.cpp mainmenu.cpp game_api.cpp gamepad.cpp font1.cpp enemygen.cpp \
	game_app.cpp network.cpp render.cpp terrain.cpp world.cpp inventory.cpp \
	physics.cpp sprite.cpp rgba8.cpp audio.cpp stb_vorbis.cpp asciiverse/js.cpp
GAME_C := upng.c tinfl.c asciiverse/miniaudio/miniaudio.c
QJS := asciiverse/quickjs/quickjs.c asciiverse/quickjs/libregexp.c \
	asciiverse/quickjs/libunicode.c asciiverse/quickjs/dtoa.c

DEFS := -DGAME -DPURE_TERM -DAV_QUICKJS -DAV_MINIAUDIO -DAV_PATHS -DAV_FPS=60 -DAV_CENTRE -DAV_DIFF -DAV_COLOURS -pthread

OBJS := $(GAME:%.cpp=$(OBJ)/%.o) $(GAME_C:%.c=$(OBJ)/%.o) $(QJS:%.c=$(OBJ)/%.o)

$(OUT)/asciicker: $(OBJS)
	$(CXX) $(LDFLAGS) -pthread -o $@ $(OBJS) $(LIBS) -ldl -lm

$(OBJ)/asciiverse/quickjs/%.o: asciiverse/quickjs/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -std=gnu11 -w -c -o $@ $<

$(OBJ)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(DEFS) -c -o $@ $<

$(OBJ)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -std=c++17 $(DEFS) -c -o $@ $<

clean:
	rm -rf $(OUT)

.PHONY: clean
