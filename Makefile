.RECIPEPREFIX := >

CC := gcc
CFLAGS := -std=c17 -Wall -Wextra -Wpedantic -g
CPPFLAGS := $(shell pkg-config --cflags libsodium)
LDLIBS := $(shell pkg-config --libs libsodium)

TARGET := build/password-manager
SANITIZER_TARGET := build/password-manager-sanitize

SOURCES := src/main.c src/input.c src/vault.c
HEADERS := src/input.h src/vault.h

.PHONY: all run sanitize clean

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
>mkdir -p build
>$(CC) $(CFLAGS) $(CPPFLAGS) $(SOURCES) -o $(TARGET) $(LDLIBS)

run: $(TARGET)
>./$(TARGET)

sanitize: $(SOURCES) $(HEADERS)
>mkdir -p build
>$(CC) $(CFLAGS) -fsanitize=address,undefined \
>-fno-omit-frame-pointer $(CPPFLAGS) $(SOURCES) \
>-o $(SANITIZER_TARGET) $(LDLIBS)

clean:
>rm -f $(TARGET) $(SANITIZER_TARGET) build/*.o
