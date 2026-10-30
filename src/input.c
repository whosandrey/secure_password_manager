#include "input.h"

#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <sodium.h>

static void discard_remaining_input(void)
{
    int character;

    while ((character = getchar()) != '\n' && character != EOF) {
    }
}

int get_text_input(
    const char *prompt,
    char *buffer,
    size_t buffer_size)
{
    size_t length;

    if (prompt == NULL || buffer == NULL || buffer_size < 2) {
        return -1;
    }

    fputs(prompt, stdout);
    fflush(stdout);

    if (fgets(buffer, (int) buffer_size, stdin) == NULL) {
        return -1;
    }

    length = strcspn(buffer, "\n");

    if (buffer[length] == '\n') {
        buffer[length] = '\0';
    } else {
        discard_remaining_input();
        sodium_memzero(buffer, buffer_size);
        return -1;
    }

    if (length == 0) {
        sodium_memzero(buffer, buffer_size);
        return -1;
    }

    return 0;
}

int get_hidden_input(
    const char *prompt,
    char *buffer,
    size_t buffer_size)
{
    struct termios old_settings;
    struct termios new_settings;
    int echo_disabled = 0;
    size_t length;

    if (prompt == NULL || buffer == NULL || buffer_size < 2) {
        return -1;
    }

    fputs(prompt, stdout);
    fflush(stdout);

    if (isatty(STDIN_FILENO)) {
        if (tcgetattr(STDIN_FILENO, &old_settings) != 0) {
            fputc('\n', stdout);
            return -1;
        }

        new_settings = old_settings;
        new_settings.c_lflag &= (tcflag_t) ~ECHO;

        if (tcsetattr(
                STDIN_FILENO,
                TCSAFLUSH,
                &new_settings) != 0) {
            fputc('\n', stdout);
            return -1;
        }

        echo_disabled = 1;
    }

    if (fgets(buffer, (int) buffer_size, stdin) == NULL) {
        if (echo_disabled) {
            tcsetattr(
                STDIN_FILENO,
                TCSANOW,
                &old_settings
            );
        }

        fputc('\n', stdout);
        return -1;
    }

    if (echo_disabled) {
        if (tcsetattr(
                STDIN_FILENO,
                TCSANOW,
                &old_settings) != 0) {
            sodium_memzero(buffer, buffer_size);
            fputc('\n', stdout);
            return -1;
        }
    }

    fputc('\n', stdout);

    length = strcspn(buffer, "\n");

    if (buffer[length] == '\n') {
        buffer[length] = '\0';
    } else {
        discard_remaining_input();
        sodium_memzero(buffer, buffer_size);
        return -1;
    }

    if (length == 0) {
        sodium_memzero(buffer, buffer_size);
        return -1;
    }

    return 0;
}