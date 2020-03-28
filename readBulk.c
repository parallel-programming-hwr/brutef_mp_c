#include "readBulk.h"

size_t getBulk(FILE * fptr, char  bulk[BULKSIZE][MAX_PW_LEN]){
    size_t i;
    for (i = 0; i < BULKSIZE; ++i) {
        if (!fgets(bulk[i], MAX_PW_LEN, fptr)){
            break;
        }
    }
    return i;
}
size_t getHashBulk(FILE *fptr, struct s_rainbowvalue256 rs[BULKSIZE]){
    return fread(rs, sizeof(struct s_rainbowvalue256),BULKSIZE,fptr );
}
size_t writeHashHumanReadable(FILE *fptr, struct s_rainbowvalue256 rs[BULKSIZE]){
    return 0;
}
size_t writeHashBase64(FILE *fptr,size_t num ,struct s_rainbowvalue256 rs[BULKSIZE]){
    return 0;
}