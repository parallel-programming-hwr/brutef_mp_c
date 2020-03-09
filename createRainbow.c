#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include "rainbowvalue.h"
#include "cryptwrapper.h"
#include "readBulk.h"

#include <argp.h>

const char *argp_program_version =
        "create Rainbow 0.1";
/* Program documentation. */
static char doc[] =
        "I wonder for what this is";

static struct argp_option options[] = {
        {"verbose",  'v', 0,      0,  "Produce verbose output" },
        {"quiet",    'q', 0,      0,  "Don't produce any output" },
        {"base64",    'b', 0,      0,  "Saves data in base 64" },
        {"humanreadable",    'H', 0,      0,  "Output is human readable (slower)" },
        {"output",   'o', "FILE", 0, "Output to FILE instead of <name of input file>.sha256" },
        { 0 }
};
struct arguments
{
    char *input_file;        /* arg1 & arg2 */
    int silent, verbose, humanreadable, base64;
    int use_output_file, use_input_file;
    char *output_file;
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
        case 'H':
            arguments->humanreadable = 1;
            break;
        case 'b':
            arguments->base64 = 1;
            break;
        case 'o':
            arguments->output_file = arg;
            arguments->use_output_file = 1;
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

char bulk_buf[BULKSIZE][MAX_PW_LEN];
struct s_rainbowvalue256 bulk_buf_out[BULKSIZE];

int main(int argc, char **argv) {
    //parsing args----------------------------------------------------------------
    struct arguments arguments;

    /* Default values. */
    arguments.silent = 0;
    arguments.verbose = 0;
    arguments.humanreadable = 0;
    arguments.base64 = 0;
    arguments.output_file = "-";
    arguments.input_file = "-";

    argp_parse (&argp, argc, argv, 0, 0, &arguments);
    //parsing args end---------------------------------------------------------------

    FILE *fptr;
    fptr = fopen(arguments.input_file,"r");
    if(fptr == NULL)
    {
        printf("Could not open file %s\n", arguments.input_file);
        return 1;
    }
    FILE *tfptr;
    if (arguments.use_output_file){
        tfptr = fopen(arguments.output_file,"wb");
        if(tfptr == NULL)
        {
            printf("Could not create file %s\n", arguments.output_file);
            return 1;
        }
    }
    else {
        char str[strlen(argv[1])+7];
        strcpy(str,argv[1]);
        strcat(str,".sha256");
        tfptr = fopen(str,"wb");
        if(tfptr == NULL)
        {
            printf("Could not create file %s\n", str);
            return 1;
        }
    }


    mycryptwrapper_init();
    char line[256];

    gcry_error_t err;
    size_t mens = gcry_md_get_algo_dlen(algo);
    void * digest=malloc(mens);

    size_t entries;
    while ((entries = getBulk(fptr,bulk_buf))){
        #pragma omp parallel for
        for (size_t i = 0 ; i < entries;++i){
            struct s_rainbowvalue256 r;
            gcry_md_hash_buffer(algo,r.hash,bulk_buf[i],strlen(bulk_buf[i])-1);// -1 so trailing \n is not used for calculation
            strcpy(r.pw , bulk_buf[i]);
            bulk_buf_out[i]= r;
        }
        if (arguments.humanreadable){

        }
        else if(arguments.base64){

        }
        else {
            fwrite(bulk_buf_out, sizeof(struct s_rainbowvalue256),entries,tfptr);
        }

    }
    fclose(tfptr);
    fclose(fptr);
    return 0;
}