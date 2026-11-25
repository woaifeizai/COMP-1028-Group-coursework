
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "input_and_output.h"
#define INITIAL_CAPACITY 16
//delete all the space, enter and /n
static void trim_right(char *s){
    size_t len = strlen(s);
    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r' ||
                       s[len - 1] == ' '  || s[len - 1] == '\t')) {
        s[--len] = '\0';
    }
}
//check the file is a txt file
static int has_txt_extension(const char *name){
    size_t len = strlen(name);
    return len > 4 && strcmp(name + len - 4, ".txt") == 0;
}

//if the capactiy of the collection are full , then double the memory
static int ensure_capacity(DocCollection *col){
    Document *bigger;
    int new_cap;

    if (col->count < col->capacity)
        return 1;
    new_cap = (col->capacity == 0) ? INITIAL_CAPACITY : col->capacity * 2;
    bigger = (Document *)realloc(col->docs, new_cap * sizeof(Document));
    if (bigger == NULL) {
        fprintf(stderr, "Error: out of memory.\n");
        return 0;
    }
    col->docs = bigger;
    col->capacity = new_cap;
    return 1;
}
//read the txt file into the doc file and whenever the reading is success or fail it will file in the status
static void read_document(Document *doc){
    FILE *fp;
    long size;
    size_t n_read;
    long i;
    doc->content = NULL;
    doc->size_bytes = 0;
    doc->line_count = 0;
    //open the file by binary so that the ftell will get the accuracy value
    fp = fopen(doc->path, "rb");
    if (fp == NULL) {
        doc->status = DOC_MISSING;
        return;
    }
    //use fseek+ftell to get the capacity of the file and return to the start using rewind
    if (fseek(fp, 0, SEEK_END) != 0) {
        doc->status = DOC_READ_ERROR;
        fclose(fp);
        return;
    }
    size = ftell(fp);
    rewind(fp);
    if (size < 0) {
        doc->status = DOC_READ_ERROR;
        fclose(fp);
        return;
    }
    if (size == 0) {
        doc->status = DOC_EMPTY;
        fclose(fp);
        return;
    }
    //extra one space to put \0 so the content can be used as a string
    doc->content = (char *)malloc((size_t)size + 1);
    if (doc->content == NULL) {
        doc->status = DOC_READ_ERROR;
        fclose(fp);
        return;
    }
    //read all the file content into the memory
    n_read = fread(doc->content, 1, (size_t)size, fp);
    fclose(fp);
    if (n_read == 0) {
        free(doc->content);
        doc->content = NULL;
        doc->status = DOC_READ_ERROR;
        return;
    }
    doc->content[n_read] = '\0';
    doc->size_bytes = (long)n_read;
    //calculate all the lines in the file 
    for (i = 0; i < doc->size_bytes; i++) {
        if (doc->content[i] == '\n')
            doc->line_count++;
    }
    if (doc->content[doc->size_bytes - 1] != '\n')
        doc->line_count++;

    doc->status = DOC_OK;
}
void init_collection(DocCollection *col){
    col->docs = NULL;
    col->count = 0;
    col->capacity = 0;
    col->ok_count = 0;
    col->folder[0] = '\0';
}

int load_collection(DocCollection *col, const char *folder, const char *manifest_file){
    FILE *mf;
    char line[MAX_FILENAME_LEN];
    Document *doc;
    free_collection(col);
    init_collection(col);
    mf = fopen(manifest_file, "r");
    if (mf == NULL) {
        fprintf(stderr, "Error: cannot open file list '%s'.\n", manifest_file);
        return -1;
    }
    strncpy(col->folder, folder, MAX_PATH_LEN - 1);
    col->folder[MAX_PATH_LEN - 1] = '\0';
    /* 逐行读取文件名 */
    //read file name line by line
    while (fgets(line, sizeof(line), mf) != NULL) {
        trim_right(line);
        // jump through all the empty line and #'s
        if (line[0] == '\0' || line[0] == '#')
            continue;

        if (!has_txt_extension(line)) {
            printf("  [skip] '%s' is not a .txt file\n", line);
            continue;
        }
        if (!ensure_capacity(col))
            break;
        doc = &col->docs[col->count];
        doc->id = col->count + 1;  //start from id 1 make sure that no reapeated file will be read

        strncpy(doc->filename, line, MAX_FILENAME_LEN - 1);
        doc->filename[MAX_FILENAME_LEN - 1] = '\0';
        snprintf(doc->path, MAX_PATH_LEN, "%s/%s", folder, line);
        read_document(doc);
        if (doc->status == DOC_OK) {
            col->ok_count++;
        } else {
            printf("  [warning] %s: %s\n", doc->filename, status_to_string(doc->status));
        }

        col->count++;
    }
    fclose(mf);
    return col->ok_count;
}
void print_collection(const DocCollection *col){
    int i;
    const Document *d;
    if (col->count == 0) {
        printf("No documents loaded.\n");
        return;
    }
    printf("\n%-4s %-25s %-10s %-7s %s\n", "ID", "Filename", "Bytes", "Lines", "Status");
    printf("------------------------------------------------------------\n");
    for (i = 0; i < col->count; i++) {
        d = &col->docs[i];
        printf("%-4d %-25s %-10ld %-7d %s\n",
               d->id, d->filename, d->size_bytes, d->line_count,
               status_to_string(d->status));
    }
    printf("------------------------------------------------------------\n");
    printf("Total listed: %d   Loaded OK: %d   Problems: %d\n",
           col->count, col->ok_count, col->count - col->ok_count);
}
const Document *get_document_by_id(const DocCollection *col, int id){
    //id is follow by 1,2,3,4 so id-1 is the previous one
    if (id < 1 || id > col->count)
        return NULL;
    return &col->docs[id - 1];
}

void free_collection(DocCollection *col){
    int i;
    if (col->docs != NULL) {
        for (i = 0; i < col->count; i++) {
            free(col->docs[i].content); 
        }
        free(col->docs);
    }
    col->docs = NULL;
    col->count = 0;
    col->capacity = 0;
    col->ok_count = 0;
}
const char *status_to_string(DocStatus s){
    switch (s) {
        case DOC_OK:         return "OK";
        case DOC_EMPTY:      return "EMPTY";
        case DOC_MISSING:    return "MISSING/UNREADABLE";
        case DOC_READ_ERROR: return "READ ERROR";
        default:             return "UNKNOWN";
    }
}
