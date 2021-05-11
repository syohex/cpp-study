#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <unistd.h>

namespace {

void Usage() {
    std::cout << "Usage: tiny-unzip file.zip\n\n"
              << "  -l list files\n"
              << "  -d directory to extract into\n"
              << std::endl;
}

} // namespace

int main(int argc, char *argv[]) {
    if (argc == 1) {
        Usage();
        std::exit(0);
    }

    int opt;
    bool list_files = false;
    std::filesystem::path output_dir;
    while ((opt = getopt(argc, argv, "d:l")) != -1) {
        switch (opt) {
        case 'd':
            output_dir = optarg;
            break;
        case 'l':
            list_files = true;
            break;
        default: /* '?' */
            Usage();
            std::exit(EXIT_FAILURE);
        }
    }

    if (optind >= argc) {
        Usage();
        exit(EXIT_FAILURE);
    }

    return 0;
}
