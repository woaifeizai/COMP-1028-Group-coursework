#ifndef DOC_LOADER_H
#define DOC_LOADER_H
#define MAX_FILENAME_LEN 256
#define MAX_PATH_LEN     512
// status of the docs
typedef enum {
    DOC_OK = 0,      
    DOC_EMPTY,         
    DOC_MISSING,       
    DOC_READ_ERROR     
} DocStatus;
// all the content in the doc
typedef struct {
    int       id;                          // make sure each doc have a unique number
    char      filename[MAX_FILENAME_LEN];  //doc name
    char      path[MAX_PATH_LEN];          //full path of the doc
    char     *content;                     // doc content
    long      size_bytes;                  // word count of the doc
    int       line_count;                  //line of the doc
    DocStatus status;                      // loading status
} Document;
//collection of the docs properties , and it can be extract infinite
typedef struct {
    Document *docs;       //docs
    int       count;      //docs number
    int       capacity;   // total capacity
    int       ok_count;   // number of docs that read successful
    char      folder[MAX_PATH_LEN]; // path of the doc
} DocCollection;
//initialize a collection for repeated use 
void init_collection(DocCollection *col);
//use to load the doc from the path
int load_collection(DocCollection *col, const char *folder, const char *manifest_file);
//print down all the words in the doc
void print_collection(const DocCollection *col);
//main function which use to find the doc by several id
const Document *get_document_by_id(const DocCollection *col, int id);
//to avoid the memory leak 
void free_collection(DocCollection *col);
//change the status of the doc to string format
const char *status_to_string(DocStatus s);
#endif
