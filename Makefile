
CC=g++

# TODO: Add more warning flags like -Wall and -Wextra
CFLAGS=-Wno-int-to-pointer-cast -O2

LDFLAGS=-L/usr/X11/lib -lX11 -lXext -lXfixes -lstdc++

OBJ_FILES=main.cpp.o hacks/bhop.cpp.o hacks/playerInfo.cpp.o client/client.cpp.o
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
