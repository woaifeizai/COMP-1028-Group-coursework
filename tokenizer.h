#ifndef TOKENIZER_H
#define TOKENIZER_H

typedef void (*TokenCallback)(const char *token, void *context);

void tokenize_text(const char *text,
                   TokenCallback callback,
                   void *context);

#endif
