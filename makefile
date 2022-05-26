CPP = g++
CFLAGS = -Wall -Wextra -Wpedantic -Werror -O2 -std=c++17
INCLUDES = -I "./src"
LDFLAGS = -llua

OS = $(shell uname -s)
ifeq ($(OS),Darwin)
	LIB_FLAGS = -bundle -undefined dynamic_lookup
else
	LIB_FLAGS = -shared -fPIC
endif

SRCDIR = src/
OUTDIR = build/

.phony: test clean

all:  filesystem

filesystem: $(OUTDIR)filesystem.so

$(OUTDIR)filesystem.so: $(SRCDIR)filesystem.cpp
	@mkdir -p build
	$(CPP) $(CFLAGS) $(LIB_FLAGS) -o $@ $+

test: filesystem
	lua test/main.lua

clean:
	rm -rf $(OUTDIR)
