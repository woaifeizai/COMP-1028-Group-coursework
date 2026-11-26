#include <stdlib.h>
#include <string.h>
#include "word_stats.h"

void word_stats_init(WordStats *stats)
{
    if (stats == NULL) {
        return;
    }

    stats->total_words = 0;
    stats->unique_count = 0;
    stats->capacity = 0;
    stats->words = NULL;
    stats->memory_error = 0;
}

void word_stats_add_token(const char *token, void *context)
{
    WordStats *stats = (WordStats *)context;
    size_t i;
    char **new_words;
    char *copy;
    size_t length;

    if (stats == NULL || token == NULL || stats->memory_error) {
        return;
    }

    stats->total_words++;

    for (i = 0; i < stats->unique_count; i++) {
        if (strcmp(stats->words[i], token) == 0) {
            return;
        }
    }

    if (stats->unique_count == stats->capacity) {
        size_t new_capacity;

        if (stats->capacity == 0) {
            new_capacity = 16;
        } else {
            new_capacity = stats->capacity * 2;
        }

        new_words = realloc(stats->words,
                            new_capacity * sizeof(char *));

        if (new_words == NULL) {
            stats->memory_error = 1;
            return;
        }

        stats->words = new_words;
        stats->capacity = new_capacity;
    }

    length = strlen(token);
    copy = malloc(length + 1);

    if (copy == NULL) {
        stats->memory_error = 1;
        return;
    }

    strcpy(copy, token);
    stats->words[stats->unique_count] = copy;
    stats->unique_count++;
}

void word_stats_free(WordStats *stats)
{
    size_t i;

    if (stats == NULL) {
        return;
    }

    for (i = 0; i < stats->unique_count; i++) {
        free(stats->words[i]);
    }

    free(stats->words);
    word_stats_init(stats);
}
