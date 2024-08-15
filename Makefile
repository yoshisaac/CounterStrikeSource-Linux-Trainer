
CC=g++

MAKEFLAGS := --jobs=$(shell nproc)

# TODO: Add more warning flags like -Wall and -Wextra
CFLAGS= -Wno-int-to-pointer-cast -O2 -m32 `pkg-config --cflags gtk+-3.0`

LDFLAGS=-L/usr/X11/lib -lX11 -lXext -lXfixes -lstdc++ -m32 `pkg-config --libs gtk+-3.0` 

OBJ_FILES=main.cpp.o hacks/bhop.cpp.o hacks/playerInfo.cpp.o GUI/GUI.cpp.o hacks/aimbot.cpp.o hacks/ESP.cpp.o hacks/draw.cpp.o hacks/crosshair.cpp.o hacks/fovCircle.cpp.o
OBJS=$(addprefix obj/, $(OBJ_FILES))
BIN=cs-source-hack

.PHONY: all clean

#-------------------------------------------------------------------------------

all: $(BIN)

clean:
	rm -f $(OBJS)
	rm -f $(BIN)

#-------------------------------------------------------------------------------

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

obj/%.cpp.o: %.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<
