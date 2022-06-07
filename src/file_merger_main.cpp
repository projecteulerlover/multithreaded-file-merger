#include <iostream>

#include "file_merger.h"

int main(int argc, char** argv) {
  if (argc != 3) {
    std::cout << "Three arguments required: 1. binary path 2. input file "
                 "directory 3. output file name.";
    return EXIT_FAILURE;
  }
  FileMerger merger(argv[1], argv[2]);
  merger.MergeSortFiles();
  return EXIT_SUCCESS;
}