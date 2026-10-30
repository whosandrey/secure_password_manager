#ifndef INPUT_H
#define INPUT_H

#include <stddef.h>

int get_text_input(
    const char *prompt,
    char *buffer,
    size_t buffer_size
);

int get_hidden_input(
    const char *prompt,
    char *buffer,
    size_t buffer_size
);

#endif