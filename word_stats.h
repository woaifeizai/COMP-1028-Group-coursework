#ifndef WORD_STATS_H
#define WORD_STATS_H

#include <stddef.h>

typedef struct {
    size_t total_words;
    size_t unique_count;
    size_t capacity;
    char  **words;
    int memory_error;
} WordStats;

void word_stats_init(WordStats *stats);
void word_stats_add_token(const char *token, void *context);
void word_stats_free(WordStats *stats);

#endif
