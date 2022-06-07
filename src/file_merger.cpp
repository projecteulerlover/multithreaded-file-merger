#include "file_merger.h"

#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <list>
#include <memory>
#include <string>

namespace {
constexpr int kBufferSize = 1024;
}

void FileMerger::MergeSortFiles() {
  auto directory_itr = std::filesystem::directory_iterator(input_folder_path_);
  std::list<std::string> file_paths_to_combine;
  for (const auto& file : directory_itr) {
    if (!file.is_directory() && file.path().string() != output_file_path_) {
      file_paths_to_combine.push_back(file.path().string());
    }
  }
  int temp_index = 0;
  while (file_paths_to_combine.size() > 1) {
    std::string in_file_path_1 = file_paths_to_combine.front();
    file_paths_to_combine.pop_front();
    std::string in_file_path_2 = file_paths_to_combine.front();
    file_paths_to_combine.pop_front();
    std::string out_file_path = file_paths_to_combine.empty()
                                    ? output_file_path_
                                    : temp_storage_ +
                                          std::to_string(temp_index++) + ".txt";
    std::cout << "Merge sorting files " << in_file_path_1 << " and "
              << in_file_path_2 << " into " << out_file_path << "\n";
    MergeSortTwoFiles(in_file_path_1, in_file_path_2, out_file_path);
    file_paths_to_combine.push_back(out_file_path);
  }
  std::filesystem::remove_all("myDirectory");
}

std::string FileMerger::MergeSortTwoFiles(const std::string& in_file_path_1,
                                          const std::string& in_file_path_2,
                                          const std::string& out_file_path) {
  std::ifstream in_1, in_2;
  in_1.open(in_file_path_1);
  in_2.open(in_file_path_2);

  std::ofstream out_file;
  out_file.open(out_file_path);
  std::string token_1, token_2, last_write;
  std::getline(in_1, token_1, '\n');
  std::getline(in_2, token_2, '\n');
  while (in_1 && in_2) {
    if (token_1 == "") {
      std::getline(in_1, token_1, '\n');
      continue;
    }
    if (token_2 == "") {
      std::getline(in_2, token_2, '\n');
      continue;
    }
    int cmp = strcmp(token_1.c_str(), token_2.c_str());
    if (cmp <= 0) {
      if (token_1 != last_write) {
        out_file << token_1 << "\n";
        last_write = std::move(token_1);
      }
      std::getline(in_1, token_1, '\n');
      if (cmp == 0) {
        std::getline(in_2, token_2, '\n');
      }
    } else {
      if (token_2 != last_write) {
        out_file << token_2 << "\n";
        last_write = std::move(token_2);
      }
      std::getline(in_2, token_2, '\n');
    }
  }
  while (std::getline(in_1, token_1, '\n')) {
    out_file << token_1 << '\n';
  }
  while (std::getline(in_2, token_2, '\n')) {
    out_file << token_2 << '\n';
  }
  in_1.close();
  in_2.close();
  out_file.close();
  return out_file_path;
}