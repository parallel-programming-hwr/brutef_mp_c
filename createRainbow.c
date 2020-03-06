#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include "rainbowvalue.h"
#include "cryptwrapper.h"
#include "readBulk.h"

char bulk_buf[BULKSIZE][MAXPWSIZE];
struct s_rainbowvalue256 bulk_buf_out[BULKSIZE];


int main(int argc, char const *argv[]) {
    if (argc != 2){
        printf("one argument expected ,argc =%i\n",argc);
        return 1;
    }
    FILE *fptr;
    fptr = fopen(argv[1],"r");
    if(fptr == NULL)
    {
        printf("Could not open file %s\n", argv[1]);
        return 1;
    }
    FILE *tfptr;
    char str[strlen(argv[1])+7];
    strcpy(str,argv[1]);
    strcat(str,".sha256");
    tfptr = fopen(str,"w");
    if(tfptr == NULL)
    {
        printf("Could not create file %s\n", str);
        return 1;
    }
    mycryptwrapper_init();
    char line[256];

    gcry_error_t err;
    size_t mens = gcry_md_get_algo_dlen(algo);
    void * digest=malloc(mens);

    size_t entries;
    while ((entries = getBulk(fptr,bulk_buf))){
        #pragma omp parallel for
        for (size_t i = 0 ; i < entries;++i){
            struct s_rainbowvalue256 r;
            gcry_md_hash_buffer(algo,r.hash,bulk_buf[i],strlen(bulk_buf[i])-1);// -1 so trailing \n is not used for calculation
            int pw_len= strlen(bulk_buf[i]);
            r.pw = malloc(pw_len);
            strncpy(r.pw ,bulk_buf[i],pw_len-1 );
            r.pw[pw_len-1]='\0';
            bulk_buf_out[i]= r;
        }
        //fwrite(bulk_buf_out, sizeof(struct s_rainbowvalue256),entries,tfptr);

        if (entries!=write_rainbow_bulk(tfptr,bulk_buf_out,entries)){
            perror("could not write all data\n");
        }
        //free memory
        for (size_t i = 0 ; i < entries;++i){
            free(bulk_buf_out[i].pw);
        }
    }
    fclose(tfptr);
    fclose(fptr);
    return 0;
}