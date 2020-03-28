#include <stdio.h>
#include "gcrypt.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <strings.h>
#include <inttypes.h>

#include "cryptwrapper.h"

#include <argp.h>

const char *argp_program_version =
        "create Rainbow 0.1";
/* Program documentation. */
static char doc[] =
        "I wonder for what this is";

static struct argp_option options[] = {
        {"verbose",  'v', 0,      0,  "Produce verbose output" },
        {"quiet",    'q', 0,      0,  "Don't produce any output" },
        {"output",   'o', "FILE", 0, "Output to FILE instead of <name of input file>.encry" },
        {"authentication_mode",   'm', "MODE", 0, "default MtE (only one implemented)" },
        {"password",   'p', "clear_pw", 0, "password used for encryption" },
        { 0 }
};
struct arguments
{
    char *authentication_mode;
    char *output_file;
    char *input_file;
    char *clear_pw;
    int silent, verbose;
    int use_output_file, use_clear_pw;
};
/* Parse a single option. */
static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
    /* Get the input argument from argp_parse, which we
       know is a pointer to our arguments structure. */
    struct arguments *arguments = state->input;

    switch (key)
    {
        case 'q': case 's':
            arguments->silent = 1;
            break;
        case 'v':
            arguments->verbose = 1;
            break;
        case 'o':
            arguments->output_file = arg;
            arguments->use_output_file = 1;
            break;
        case 'p':
            arguments->use_clear_pw = 1;
            arguments->clear_pw = arg;
            break;
        case ARGP_KEY_ARG:
            if (state->arg_num >= 1)
                /* Too many arguments. */
                argp_usage (state);

            arguments->input_file = arg;

            break;

        case ARGP_KEY_END:
            if (state->arg_num < 1)
                /* Not enough arguments. */
                argp_usage (state);
            break;


        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

/* Our argp parser. */
static struct argp argp = { options, parse_opt, 0 , doc };
//parsing args set up end---------------------------------------------------------------


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
    if (buf_out==NULL){
        perror("could not allocate memory\n");
    }
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
    printf("filehash:\n");
    mycryptwrapper_print(file_hash,32);

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
    memset(iv,0,len);
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
    free(iv);
    gcry_cipher_close(hd);

    fclose(fptr);
    fclose(tfptr);
    printf("successfully encrypted file %s with sha256 check sum and stored in file %s\n",argv[1],new_file_name);
    return 0;
}