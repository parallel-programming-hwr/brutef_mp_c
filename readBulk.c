#include "readBulk.h"

size_t getBulk(FILE * fptr, char  bulk[BULKSIZE][MAXPWSIZE]){
    size_t i;
    for (i = 0; i < BULKSIZE; ++i) {
        if (!fgets(bulk[i], MAXPWSIZE, fptr)){
            break;
        }
    }
    return i;
}
size_t getHashBulk(FILE *fptr, struct s_rainbowvalue256 rs[BULKSIZE]){
    return fread(rs, sizeof(struct s_rainbowvalue256),BULKSIZE,fptr );
}

size_t write_rainbow_bulk(FILE *fptr ,struct s_rainbowvalue256 rs[BULKSIZE], size_t len ){
    for (size_t i = 0 ; i<len ; i++){
        fprintf(fptr,"%s,",rs[i].pw);
        for (int j = 0; j<32 ; j++){
            fprintf(fptr,"%02x",rs[i].hash[j]);
        }
        fprintf(fptr,"\n");
    }
    return len;
}