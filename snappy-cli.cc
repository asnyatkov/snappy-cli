include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

#include "snappy.h"

inline bool ends_with(std::string const & value, std::string const & ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

int main(int argc, char** argv)
{
    if (argc < 3) {
        printf("Usage: snappy-cli <input file> <output file>\n");
        return 0;
    }

    struct stat st;
    if (stat(argv[1], &st) != 0) {
        perror("stat");
        return 1;
    }
    char* in = (char*)malloc(st.st_size);
    if (in == NULL) {
        perror("malloc");
        return 1;
    }
    FILE* f1 = fopen(argv[1], "r");
    if (f1 == NULL) {
        perror("fopen");
        return 1;
    }
    fread(in, st.st_size, sizeof(char), f1);
    fclose(f1);

    std::string output;
    size_t compressed_len = 0;

    if (ends_with(argv[1], ".snappy")) {
        if(!snappy::Uncompress(in, st.st_size, &output)) {
            printf("Cannot uncompress!\n");
            return 1;
        }
        printf("Uncompressed %s size %d to %s size %d\n", argv[1], int(st.st_size), argv[2], int(output.size()));
    } else {
        size_t compressed_len = snappy::Compress(in, st.st_size, &output);
        printf("Compressed %s size %d to %s size %d\n", argv[1], int(st.st_size), argv[2], int(compressed_len));
    }

    FILE* f = fopen(argv[2], "w");
    fwrite(output.data(), sizeof(char), output.size(), f);
    fclose(f);
    return 0;
}
