#include <iostream>

#include "cmdline.hpp"

int main(int argc, char *argv[]) {
    CmdLine cmdLine;
    if (!cmdLine.parse(argc, argv))
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}