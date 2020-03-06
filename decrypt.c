#include <stdio.h>
#include "gcrypt.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <strings.h>
#include <omp.h>
#include <time.h>
#include <inttypes.h>
#include "cryptwrapper.h"
#include "rainbowvalue.h"
#include "readBulk.h"


int main(int argc, char const *argv[]) {
    if (argc < 3) {
        printf("please select rainbow table and encrypted file\n");
        return 1;
    }
    FILE *fptr_rainbow = fopen(argv[1], "rb");
    FILE *fptr_encry = fopen(argv[2], "rb");

    if (fptr_encry == NULL) {
        printf("could not open encrypted file %s\n", argv[1]);
        return 1;
    }
    if (fptr_rainbow == NULL) {
        printf("could not open rainbow table  file %s\n", argv[2]);
        return 1;
    }

    mycryptwrapper_init();

    // to calculate hash without trailing 256 bits of decrypted file
    size_t mens = gcry_md_get_algo_dlen(algo);
    void *digest = malloc(mens);

    // initialization vector for decryption and encryption
    size_t len = 8;


    size_t file_len;
    if (fseek(fptr_encry, 0, SEEK_END)) {
        perror("could not ssek end of file\n");
        return 1;
    }
    file_len = ftell(fptr_encry);
    if (file_len == 0) {
        perror("file length is 0\n");
        return 1;
    }
    rewind(fptr_encry); // go to beginning of file
    uint8_t *buf = malloc(file_len);
    if (fread(buf, sizeof(uint8_t), file_len, fptr_encry) != file_len) {
        perror("could not read complete file\n");
        return 1;
    }
    //encrypted file is stored in buf and can now be decrypted




    struct s_rainbowvalue256 *rs = malloc(sizeof(struct s_rainbowvalue256) * BULKSIZE);
    size_t num_rainbow_values;
    // read a block of rainbow values

    clock_t old_clock;
    int success = 0;
    while ((num_rainbow_values = fread(rs, sizeof(struct s_rainbowvalue256), BULKSIZE, fptr_rainbow)) != 0) {
        if (success == 1) { break; }
        printf("read %d rainbow values\n", (int) num_rainbow_values);
        // iterate through rainbow values and decrypt

        old_clock=clock();
        #pragma omp parallel for
        for (size_t i = 0; i < num_rainbow_values; i++) {
            uint8_t *decrypted_buf = malloc(file_len);//allocate mem for decrypted buffer
            gcry_cipher_hd_t dhd;
            if (gcry_cipher_open(&dhd, cipher, GCRY_CIPHER_MODE_CFB, 0)) { perror("could not open cypher\n"); }
            if (gcry_cipher_setkey(dhd, rs[i].hash, 8)) { perror("could not set key\n"); };
            void *iv = malloc(len);
            memset(iv, 0, len);
            if (gcry_cipher_setiv(dhd, iv, len)) { perror("could not set init vector\n"); }
            if (gcry_cipher_decrypt(dhd, decrypted_buf, file_len, buf, file_len)) { perror("could not decrypt\n"); }
            //mycryptwrapper_print(decrypted_buf,file_len);
            //printf("pw: %s\nfile:%s\n",rs[i].pw,decrypted_buf);
            if (check_sha256_tag(decrypted_buf, file_len)) {
                printf("pw: %s\n", rs[i].pw);

                char *enc_fname = malloc(strlen(argv[2]) + 5);
                strcpy(enc_fname, argv[2]);
                strcat(enc_fname, ".decr");
                FILE *encrypted_fptr = fopen(enc_fname, "wb");
                if (fwrite(decrypted_buf, 1, file_len - 32, encrypted_fptr) != file_len - 32) {
                    perror("couln not write all data to decrypted file");
                    //return 1;
                #pragma omp exitregion
                };

                printf("successfully saved decrypted data in %s\n", enc_fname);
                //return 0;
                success = 1;
            #pragma omp exitregion
            }
            free(iv);
            free(decrypted_buf);
            gcry_cipher_close(dhd);
            if (success == 1) {
            #pragma omp exitregion
            }
        }// end for parallel

        float sec = (float)((clock()-old_clock))/(float)CLOCKS_PER_SEC;
        printf("Clocks per sec: %ld\ncalc/sec: %f\n",CLOCKS_PER_SEC, (float)BULKSIZE/sec);
        if (success == 1) {
            return 0;
        }


    }// end while

    /*while (fread(&r, sizeof(struct s_rainbowvalue256), 1,fptr)) {//reading hash values from rainbowtable
        gcry_cipher_hd_t dhd;
        gcry_error_t err= gcry_cipher_open(&dhd,cipher,GCRY_CIPHER_MODE_CFB,0);
        if (err){
            printf("could not open handle\n");
        }
        err = gcry_cipher_setkey(dhd,r.hash,8);
        if (err) {
            printf("could not set key \n");
        }
        memset(iv,0,len);
        err = gcry_cipher_setiv(dhd, iv , len);
        if (err){
            printf("could not init init vector");
        }


        memset(out,0,256);
        err = gcry_cipher_decrypt(dhd,out,256,encrypted_secret,strlen(encrypted_secret));
        if (err){
            printf("could not decrypt\n");
            }

        if (strcmp(out,dummydata)==0){
            printf("pw: %sfor data: %s\npwhash: ", r.pw, (char*)out);
            mycryptwrapper_print(r.hash, strlen(r.hash));
            gcry_cipher_close(dhd);//close cipher
            return 0;
        }
        gcry_cipher_close(dhd);//close cipher
    }
*/
    if (success == 0) {
        printf("\nnothing found\n");

    }

    //printdata(digest,mens);



    return 0;
}