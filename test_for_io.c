#include <stdio.h>
#include "input and output.h"

int main(int argc, char *argv[]){
    DocCollection col;
    const char *folder   = (argc > 1) ? argv[1] : "data";
    const char *manifest = (argc > 2) ? argv[2] : "data/files.txt";
    const Document *d;
    int loaded;
    init_collection(&col);
    printf("Loading documents from '%s' using list '%s'...\n", folder, manifest);
    loaded = load_collection(&col, folder, manifest);
    if (loaded < 0) {
        printf("Load failed.\n");
        return 1;
    }
    print_collection(&col);
    d = get_document_by_id(&col, 1);
    if (d != NULL && d->status == DOC_OK) {
        printf("\nPreview of doc %d (%s):\n%.100s...\n", d->id, d->filename, d->content);
    }

    free_collection(&col);
    return 0;
}
