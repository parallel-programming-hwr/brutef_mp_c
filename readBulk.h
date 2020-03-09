#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "rainbowvalue.h"

#ifndef readBulk
#define readBulk

#define BULKSIZE 20000

/**
 * reads bulk from file, file pointer is used as handle so if you read several bulk, dont use file pointer between
 * if BULKSIZE or EOF is reached, 2000 or number of lines read is returned. Memory must be allocated by caller
 * @param fptr file pointer
 * @bulk allocated memory fot bulk
 * @return
 */
size_t getBulk(FILE * fptr, char  bulk[BULKSIZE][MAX_PW_LEN]);
/**
 * reads bulk of s_rainbowvalues and stores them in rs
 * @param fptr
 * @param rs
 * @return values read
 */
size_t getHashBulk(FILE *fptr, struct s_rainbowvalue256 rs[BULKSIZE]);
/**
 * writes to tsv file
 * @param fptr file pointer to which values are written
 * @param rs buffer of values to be written
 * @return values written
 */
size_t writeHashHumanReadable(FILE *fptr, struct s_rainbowvalue256 rs[BULKSIZE]);
/**
 * writes hash to tsv in base 64
 * @param fptr
 * @param rs
 * @return
 */
size_t writeHashBase64(FILE *fptr, struct s_rainbowvalue256 rs[BULKSIZE]);

#endif