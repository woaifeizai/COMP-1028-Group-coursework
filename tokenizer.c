#include <ctype.h>
#include "tokenizer.h"

#define TOKEN_SIZE 128

void tokenize_text(const char *text,  TokenCallback callback,  void *context)

char token[TOKEN_SIZE];
int length = 0;
int i = 0;

if (text == NULL | callback == NULL) {
    return;
  }

while (1) {
    unsigned char ch = (unsigned char)text[i];
if (isalnum(ch)) {
    if (length < TOKEN_SIZE - 1) {
        token[length] = (char)tolower(ch);
        length++;
    }
  }
  else if (length>0){
      token[length]='\0';
      callback(token,context);
      length = 0;
  }
if (text[i] == '\0') {
    break;
  }  

i++;
  }
}
