#include <stdio.h>
#include "gcrypt.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <strings.h>
#include <inttypes.h>

#include "cryptwrapper.h"

int main(int argc, char const *argv[]) {
    if (argc < 3){
        printf("please select file and pw\n");
        return 1;
    }
    FILE * fptr = fopen(argv[1],"rb");
    if (fptr==NULL){
        printf("could not open file %s\n",argv[1]);
        return 1;
    }
    char * new_file_name = malloc(strlen(argv[1]+5));
    strcpy(new_file_name,argv[1]);
    strcat(new_file_name,".encry");
    FILE * tfptr = fopen(new_file_name,"wb");
    if (fseek(fptr,0,SEEK_END)){
        printf("could not ssek end\n");
        return 1;
    };
    int file_len = ftell(fptr);
    rewind(fptr);// sets pointer to beginning of file
    //printf("file is %d long\n", file_len);
    char * buf_out = malloc(file_len + 32); //  for sha256 check sum 32*8=256 lol
    if (buf_out == NULL){
        printf("could not allocate memory, maybe file is too large\n");
        return 1;
    }

    if (file_len!=fread(buf_out,sizeof(char),file_len,fptr)){
        printf("could not read all data from file\nfile_len=%d",file_len);
        return 1;
    }

    //calculating sha256 hash to append to file
    uint8_t * pw_hash = malloc(gcry_md_get_algo_dlen(algo));
    uint8_t * file_hash = malloc(gcry_md_get_algo_dlen(algo));


    mycryptwrapper_init();
    gcry_md_hash_buffer(algo,pw_hash,argv[2],strlen(argv[2]));
    gcry_md_hash_buffer(algo,file_hash,buf_out,file_len);


    //appending sha256 to end of buffer
    for (size_t i = 0; i<+32; i++){
        buf_out[i+file_len] = file_hash[i];
    }

    gcry_cipher_hd_t hd;
    if (gcry_cipher_open(&hd,cipher,GCRY_CIPHER_MODE_CFB,0)){
        perror("could not open handle for cipher\n");
        return 1;
    }
    if(gcry_cipher_setkey(hd,pw_hash,8)){
        printf("could not set key for encryption");
        return 1;
    }
    size_t len = 8;
    void * iv = malloc(len);
    if (gcry_cipher_setiv(hd, iv , len)){
        printf("could not init init vector");
        return 1;
    }
    if (gcry_cipher_encrypt(hd,buf_out,file_len+32,NULL,0)){
        perror("could not encrypt data\n");
        return 1;
    }
    if (fwrite(buf_out, sizeof(char),file_len+32,tfptr)!= file_len+32){
        perror("could not write all data\n");
        return 1;
    }

    gcry_cipher_close(hd);

    fclose(fptr);
    fclose(tfptr);
    printf("successfully encrypted file %s with sha256 check sum and stored in file %s\n",argv[1],new_file_name);
    return 0;
}