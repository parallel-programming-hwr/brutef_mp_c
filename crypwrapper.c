#include "cryptwrapper.h"

void mycryptwrapper_init(){

    /* Version check should be the very first call because it
    makes sure that important subsystems are initialized.
    */
    if (!gcry_check_version (GCRYPT_VERSION))
    {
        fprintf (stderr, "libgcrypt is too old (need %s, have %s)\n",
                 GCRYPT_VERSION, gcry_check_version (NULL));
        exit (2);
    }
  //  printf("gcrypt version = %s\n",GCRYPT_VERSION);


    /* Disable secure memory.  */
    gcry_control (GCRYCTL_DISABLE_SECMEM, 0);

    /* ... If required, other initialization goes here.  */

    /* Tell Libgcrypt that initialization has completed. */
    gcry_control (GCRYCTL_INITIALIZATION_FINISHED, 0);

    HASH_SIZE = gcry_md_get_algo_dlen(algo);

}

void  mycryptwrapper_print(const void * buf, size_t len){
    for (size_t i = 0; i< len; i++){
        printf("%02x ", ((uint8_t*)buf)[i]);
    }
    printf("\n");
}

int check_sha256_tag(uint8_t * ptr, size_t file_len){
    uint8_t * calc_hash = malloc(HASH_SIZE);
    gcry_md_hash_buffer(algo,calc_hash,ptr,file_len-HASH_SIZE);
    for (int i=0 ; i< HASH_SIZE; i++){
        if (calc_hash[i] != ptr[i+file_len-HASH_SIZE]){
            free(calc_hash);
            return 0;
        }
    }
    return 1;
}
