
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "geno.h"

static const char usage_documentation[] =
    "Transpose a genotype between a packed ancestry map format "
    "and a transpose packed format or vice versa.";

struct arguments {
    char *input_file;
    char *output_file;
    int low_mem;
};

static const struct option options[] = {
    {"low-mem", no_argument, NULL, 'm'},
    {NULL, 0, NULL, 0}
};

static void usage(const char *program)
{
    fprintf(stderr,
        "Usage: %s [-m | --low-mem] %s\n"
        "\n"
        "%s\n"
        "\n"
        "Options:\n"
        "  -m, --low-mem   Reduce memory usage (slower)\n",
        program,
        "input_genotype_file output_genotype_file",
        usage_documentation);
}

int main(int argc, char **argv)
{
    struct arguments arguments;
    int opt;

    arguments.low_mem = 0;
    arguments.input_file = NULL;
    arguments.output_file = NULL;

    while ((opt = getopt_long(argc, argv, "m", options, NULL)) != -1) {
        switch (opt) {
        case 'm':
            arguments.low_mem = 1;
            break;

        default:
            usage(argv[0]);
            return 2;
        }
    }

    if (argc - optind != 2) {
        usage(argv[0]);
        return 2;
    }

    arguments.input_file = argv[optind];
    arguments.output_file = argv[optind + 1];

    genotype_matrix geno;

    printf("reading %s\n", arguments.input_file);

    genotype_matrix_initialize(&geno);

    int result;

    if (arguments.low_mem) {
        result = genotype_matrix_read_file(
            &geno, arguments.input_file, 0, NULL, NULL);
    } else {
        result = genotype_matrix_read_file_full(
            &geno, arguments.input_file, 0, NULL, NULL);
    }

    if (result < 0) {
        printf("Error %d", result);
        return result;
    }

    genotype_matrix_write_packed_file(
        &geno, arguments.output_file, !geno.transpose);

    genotype_matrix_destructor(&geno);

    return 0;
}
