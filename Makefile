.RECIPEPREFIX := >

CC := gcc
CFLAGS := -std=c17 -Wall -Wextra -Wpedantic -g
CPPFLAGS := $(shell pkg-config --cflags libsodium)
LDLIBS := $(shell pkg-config --libs libsodium)

TARGET := build/password-manager
SANITIZER_TARGET := build/password-manager-sanitize

SOURCES := src/main.c src/input.c src/vault.c
HEADERS := src/input.h src/vault.h

.PHONY: all run sanitize clean test

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

TEST_TARGET := build/test-vault

$(TEST_TARGET): tests/test_vault.c src/vault.c src/vault.h
>mkdir -p build
>$(CC) $(CFLAGS) $(CPPFLAGS) -UNDEBUG -Isrc tests/test_vault.c src/vault.c -o $@ $(LDLIBS)

test: $(TEST_TARGET) $(TARGET)
>test_dir=$$(mktemp -d) || exit 1; \
>trap 'rm -f -- "$$test_dir/passwords.vault"; rmdir -- "$$test_dir"' EXIT; \
>(cd "$$test_dir" && "$(abspath $(TEST_TARGET))") && \
>echo "PASS: vault tests"
>sh tests/test_cli.sh "$(abspath $(TARGET))"