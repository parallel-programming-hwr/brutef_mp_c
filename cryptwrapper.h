#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gcrypt.h"
#include <inttypes.h>



#ifndef mycryptwrapper_
#define mycryptwrapper_


#define NEED_LIBGCRYPT_VERSION "1.8.5" //should be 1.8.5 but my libgcrypt as mismatched versions :(
#define algo GCRY_MD_SHA256
#define cipher GCRY_CIPHER_DES

int HASH_SIZE;

/**
 * initializes gcrypt
 */
void mycryptwrapper_init();
/**
 * prints binary data into HEX format. Nice for debugging hash calculations
 */
void mycryptwrapper_print();
/**
 * checks if sha256 at the end of file matches the front
 * @param fptr file pointer must be opned in rb mode
 * @return 0 if true, else 1 , and 2 for errors
 */
int check_sha256_tag(uint8_t * ptr, size_t file_len);




#endif