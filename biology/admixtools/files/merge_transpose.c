
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "geno.h"

#define WVERSION "100"

static const char usage_documentation[] =
    "Merge transpose bit-packed genotype files. SNPs for one sample "
    "are stored continuously in one row, instead of samples for one "
    "SNP being stored in one row as in the packed ancestry format.";

static const struct option options[] = {
    {"input", required_argument, NULL, 'l'},
    {"output", required_argument, NULL, 'o'},
    {"ind_hash", required_argument, NULL, 'i'},
    {"snp_hash", required_argument, NULL, 's'},
    {"help", no_argument, NULL, 'h'},
    {NULL, 0, NULL, 0}
};

struct arguments {
    char *input_file_list;
    char *output_file;
    unsigned int individual_hash;
    unsigned int snp_hash;
};

static void usage(const char *program)
{
    fprintf(stderr,
        "Usage: %s -l FILE -o FILE [OPTIONS]\n\n"
        "%s\n\n"
        "Options:\n"
        "  -l, --input FILE       List of genotype files or stems\n"
        "  -o, --output FILE      Output genotype file\n"
        "  -i, --ind_hash HEX     Individual file hash\n"
        "  -s, --snp_hash HEX     SNP file hash\n"
        "  -h, --help             Show this help\n",
        program, usage_documentation);
}

static int parse_hash(const char *arg, unsigned int *value)
{
    char *end;
    unsigned long parsed;

    parsed = strtoul(arg, &end, 16);

    if (*arg == '\0' || *end != '\0' || parsed > UINT_MAX)
        return -1;

    *value = (unsigned int)parsed;
    return 0;
}

bool file_exists(const char filename[])
{
    FILE *fp = fopen(filename, "r");
    bool exists = (fp != NULL);

    if (fp)
        fclose(fp);

    return exists;
}

/* Return number of samples added to file */
int add_geno(const char filename[], FILE *merged,
             genotype_matrix *combined_geno)
{
    genotype_matrix to_add;
    int num_samples = 0;

    size_t expanded_filename_size = strlen(filename) + 6;
    char *file_to_open = malloc(expanded_filename_size);

    if (!file_to_open) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    strcpy(file_to_open, filename);

    size_t filename_length = strlen(file_to_open);

    if (filename_length > 0 &&
        file_to_open[filename_length - 1] == '\n') {
        file_to_open[filename_length - 1] = '\0';
    }

    if (!file_exists(file_to_open)) {
        strcat(file_to_open, ".geno");
    }

    printf("reading %s\n", file_to_open);

    genotype_matrix_initialize(&to_add);

    int result = genotype_matrix_read_file(
        &to_add, file_to_open, 0, NULL, NULL);

    if (result < 0 || !to_add.transpose) {
        fprintf(stderr, "Failed to add genotype file %s\n",
                file_to_open);
        free(file_to_open);
        exit(EXIT_FAILURE);
    }

    if (combined_geno->num_columns <= 0) {
        combined_geno->num_columns = to_add.num_columns;
    } else if (combined_geno->num_columns != to_add.num_columns) {
        fprintf(stderr, "SNP mismatch %zu %zu %s\n",
                combined_geno->num_columns,
                to_add.num_columns,
                file_to_open);

        genotype_matrix_destructor(&to_add);
        free(file_to_open);
        exit(EXIT_FAILURE);
    }

    num_samples = to_add.num_rows;

    fwrite(to_add.genotypes, sizeof(unsigned char),
           genotype_matrix_byte_size(&to_add), merged);

    genotype_matrix_destructor(&to_add);
    free(file_to_open);

    return num_samples;
}

int main(int argc, char **argv)
{
    struct arguments arguments;

    arguments.input_file_list = NULL;
    arguments.output_file = NULL;
    arguments.individual_hash = 0;
    arguments.snp_hash = 0;

    int opt;

    while ((opt = getopt_long(argc, argv, "l:o:i:s:h",
                              options, NULL)) != -1) {
        switch (opt) {
        case 'l':
            arguments.input_file_list = optarg;
            break;

        case 'o':
            arguments.output_file = optarg;
            break;

        case 'i':
            if (parse_hash(optarg, &arguments.individual_hash) != 0) {
                fprintf(stderr, "Invalid individual hash: %s\n", optarg);
                return EXIT_FAILURE;
            }
            break;

        case 's':
            if (parse_hash(optarg, &arguments.snp_hash) != 0) {
                fprintf(stderr, "Invalid SNP hash: %s\n", optarg);
                return EXIT_FAILURE;
            }
            break;

        case 'h':
            usage(argv[0]);
            return EXIT_SUCCESS;

        default:
            usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    if (arguments.input_file_list == NULL ||
        arguments.output_file == NULL ||
        optind != argc) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    genotype_matrix combined;
    genotype_matrix_initialize(&combined);

    combined.transpose = true;

    if (arguments.individual_hash)
        combined.individual_file_hash = arguments.individual_hash;

    if (arguments.snp_hash)
        combined.snp_file_hash = arguments.snp_hash;

    FILE *merged = fopen(arguments.output_file, "w+");

    if (!merged) {
        fprintf(stderr, "failed to open %s\n",
                arguments.output_file);
        return EXIT_FAILURE;
    }

    char header_buffer[GENO_HEADER_SIZE + 1];

    memset(header_buffer, 0, sizeof(header_buffer));

    genotype_matrix_header(&combined, true, header_buffer);

    fwrite(header_buffer, sizeof(char), GENO_HEADER_SIZE, merged);

    FILE *list_file = fopen(arguments.input_file_list, "r");

    if (!list_file) {
        fprintf(stderr, "failed to open %s\n",
                arguments.input_file_list);
        fclose(merged);
        return EXIT_FAILURE;
    }

    size_t len = 2048;
    char *filename = malloc(len);

    if (!filename) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(list_file);
        fclose(merged);
        return EXIT_FAILURE;
    }

    ssize_t read;

    while ((read = getline(&filename, &len, list_file)) != -1) {
        int samples_added = add_geno(filename, merged, &combined);
        combined.num_rows += samples_added;
    }

    free(filename);

    fseek(merged, 0, SEEK_SET);

    genotype_matrix_header(&combined, true, header_buffer);

    fwrite(header_buffer, sizeof(char), GENO_HEADER_SIZE, merged);

    fclose(list_file);
    fclose(merged);

    return EXIT_SUCCESS;
}
