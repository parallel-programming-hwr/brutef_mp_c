#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "rainbowvalue.h"

#ifndef readBulk
#define readBulk

#define BULKSIZE 2000
#define MAXMEM 256
/**
 * reads bulk from file, file pointer is used as handle so if you read several bulk, dont use file pointer between
 * if BULKSIZE or EOF is reached, 2000 or number of lines read is returned. Memory must be allocated by caller
 * @param fptr file pointer
 * @bulk allocated memory fot bulk
 * @return
 */
size_t getBulk(FILE * fptr, char  bulk[BULKSIZE][MAXMEM]);
/**
 * reads bulk of s_rainbowvalues and stores them in rs
 * @param fptr
 * @param rs
 * @return values read
 */
size_t getHashBulk(FILE *fptr, struct s_rainbowvalue256 rs[BULKSIZE]);
#endif