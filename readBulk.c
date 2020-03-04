#include "readBulk.h"

size_t getBulk(FILE * fptr, char  bulk[BULKSIZE][MAXMEM]){
    size_t i;
    for (i = 0; i < BULKSIZE; ++i) {
        if (!fgets(bulk[i], MAXMEM, fptr)){
            break;
        }
    }
    return i;
}
size_t getHashBulk(FILE *fptr, struct s_rainbowvalue256 rs[BULKSIZE]){
    return fread(rs, sizeof(struct s_rainbowvalue256),BULKSIZE,fptr );
}