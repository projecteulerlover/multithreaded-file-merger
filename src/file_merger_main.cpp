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

/*
# Merge Sort Files Tool

An implementation of a file merging tool which when given a directory containing
new line character delimited text files, aggregates all tokens into a single new
line character delimited text file. Duplicate tokens across all files should
appear a single time, and empty tokens are ignored.

## Usage
1. Compile with
2. Run with .\output\main {fully qualified input files folder} {fully qualified
output file}
3. {fully qualified output file} contains the combined unique merge sorted
tokens.

## Implementation

*/