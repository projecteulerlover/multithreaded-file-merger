#include <iostream>

#include <fstream>
#include "file_merger.h"

void MakeLargeTestCase() {
  std::cout << "creating output files " << std::endl;
  std::vector<std::ofstream> outs(10);
  for (int i = 0; i < 10; ++i) {
    std::string file_name = "../tests/data/large_test/";
    file_name.push_back('a' + i);
    outs[i].open(file_name);
  }
  std::cout << "all outs created " << std::endl;
  std::ifstream in;
  in.open("../tests/data/large_test/dictionary");
  std::string token;
  while (getline(in, token, '\n')) {
    int index = rand() % 10;
    outs[index] << token << '\n';
  }
  for (int i = 0; i < 10; ++i) {
    outs[i].close();
  }
  in.close();
}

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