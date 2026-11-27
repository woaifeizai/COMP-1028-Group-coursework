#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TERMS      1000
#define MAX_TERM_LEN  64

//initiate structures
typedef struct Posting {   //the frequency of this word appears in document 
    int doc_id;
    int freq;
    struct Posting *next;     
} Posting;
typedef struct 
{
    char term[MAX_TERM_LEN + 1]; /* storage for all term entries */
    Posting *head;
    Posting *tail;             
} TermEntry;

typedef struct {
    TermEntry terms[MAX_TERMS];
    int nterms;
} Index;

/* Add one word occurrence to the index */
int index_add_term(Index *idx, const char *term, int doc_id)
{
    if (idx == NULL || term == NULL || term[0] == '\0' || doc_id < 0)//Validate the input 
        return -1;//-1 invalid argument

    //Look for the word among the entries
    int i;
    for ( i = 0; i < idx->nterms; i++) {
        if (strcmp(idx->terms[i].term, term) == 0)//found it
            break;
    }
    

    if (i == idx->nterms) {///* i == nterms. means not found 
            if (idx->nterms == MAX_TERMS){
            return -2;}//-2 full, no space for a new word
        
        //create new 
        strncpy(idx->terms[i].term, term, MAX_TERM_LEN); //Copy the word in
        idx->terms[i].term[MAX_TERM_LEN] = '\0';
        idx->terms[i].head = NULL;
        idx->terms[i].tail = NULL;//no postings yet
        idx->nterms++;
    }

    TermEntry *e = &idx->terms[i];//A shorter name for the entry

    // Case 1: the last posting already belongs to this document,appeared again
    if (e->tail != NULL && e->tail->doc_id == doc_id) {
        e->tail->freq++;
        return 0;
    }

    //otherwise create a new posting
    Posting *p = malloc(sizeof(Posting));
    if (p == NULL)
        return -3;// malloc fail error
    p->doc_id = doc_id;
    p->freq = 1;
    p->next = NULL;

    if (e->tail == NULL)//connect
        e->head = p;        
    else
        e->tail->next = p;// Attach the new node at the end of the list
    e->tail = p;
    return 0;
}

/*search, printout, release,  */

const TermEntry *index_lookup(const Index *idx, const char *term)//Finds a word in the index.
{
    if (idx == NULL || term == NULL)
        return NULL;
    for (int i = 0; i < idx->nterms; i++) {
        if (strcmp(idx->terms[i].term, term) == 0)
            return &idx->terms[i];//address of the matching entry
    }
    return NULL;//  not found 
}

void index_print(const Index *idx)//Prints every word followed by its postings
{
    printf("unique terms: %d\n", idx->nterms);
    for (int i = 0; i < idx->nterms; i++) {
        printf("%-12s", idx->terms[i].term);
        for (Posting *p = idx->terms[i].head; p != NULL; p = p->next)//start at head, follow next, until reach null
            printf(" -> (doc %d, freq %d)", p->doc_id, p->freq);
        printf("\n");
    }
}

void index_free(Index *idx)//Release memory
{
    if (idx == NULL)
        return;
    for (int i = 0; i < idx->nterms; i++) {
        Posting *p = idx->terms[i].head;
        while (p != NULL) {
            Posting *next = p->next;   //save the next node before freeing p
            free(p);
            p = next;
        }
    }
    free(idx);
}

/*Read words from a file*/
int load_file(Index *idx, const char *path, int doc_id)
{
    FILE *fp = fopen(path, "r");
    if (fp == NULL)
   {
        printf("[error] cannot open file: %s\n", path);
        return -1;
    }
    char line[1024];
    char token[MAX_TERM_LEN + 1];
    int len = 0;//number of letters currently in token
    int count = 0;//number of words  added

    while (fgets(line, sizeof(line), fp) != NULL) {//reads the next line
        for (int i = 0;     ; i++) {
            unsigned char c = (unsigned char)line[i];//transfer to safer type
            if (isalpha(c)) 
            {
                if (len < MAX_TERM_LEN)           
                    token[len++] = (char)tolower(c);//lowercase it and append it to the current word
            } 
                
            else {                    //Not a letter
                if (len > 0) {
                    token[len] = '\0';
                        if (index_add_term(idx, token, doc_id) == 0)
                                count++;
                        len = 0;
                }
                if (c == '\0')//until to the end point
                    break;
            }
        }
    }
    fclose(fp);

    if (count == 0)
        printf("[warning] %s has no words\n", path);
    return count;
}


int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("usage: %s file1.txt [file2.txt ...]\n", argv[0]);
        return 1;
    }

    Index *idx = calloc(1, sizeof(Index));
    if (idx == NULL) {
        printf("[error] out of memory\n");
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        int n = load_file(idx, argv[i], i - 1);
        if (n >= 0)
            printf("doc %d (%s): %d words\n", i - 1, argv[i], n);
    }

    printf("\n");
    index_print(idx);

    const TermEntry *e = index_lookup(idx, "energy");
    printf("\nlookup \"energy\": %s\n", e ? "found" : "not found");
    e = index_lookup(idx, "quantumbanana");
    printf("lookup \"quantumbanana\": %s\n", e ? "found" : "not found");

    index_free(idx);
    return 0;
}
