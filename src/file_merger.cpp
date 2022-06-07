#include "file_merger.h"

#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <algorithm>
#include <random>
#include <mutex>

void FileMerger::MergeSortFiles() {
  auto directory_itr = std::filesystem::directory_iterator(input_folder_path_);
  std::vector<std::string> files_to_merge_;
  for (const auto& file : directory_itr) {
    if (!file.is_directory() && file.path().string() != output_file_path_) {
      files_to_merge_.push_back(file.path().string());
    }
  }
  for(int i = 0; i < max_threads_; ++i) {
      thread_pool_.emplace_back(&FileMerger::ThreadPoolWorker, this, &MergeSortFiles);
  }
  for(auto& t : thread_pool_){
      t.join();
  }
  std::filesystem::remove_all(temp_storage_);
}

void FileMerger::ThreadPoolWorker() {
  while (true) {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [&]() {
      return files_to_merge_.size() > 1 || completed_;
    });
    if (completed_) {
      return;
    }
    ++working_threads_;
    // At least two files to combine.
    std::string in_file_path_1 = files_to_merge_.back();
    files_to_merge_.pop_back();
    std::string in_file_path_2 = files_to_merge_.back();
    files_to_merge_.pop_back();
    std::string out_file_path =
        files_to_merge_.empty()
            ? output_file_path_
            : temp_storage_ + std::to_string(temp_index_++) + ".txt";
    std::cout << "Merge sorting files " << in_file_path_1 << " and "
              << in_file_path_2 << " into " << out_file_path << "\n";
    lock.unlock();
    MergeSortTwoFiles(in_file_path_1, in_file_path_2, out_file_path);
    DeleteTemporaryFile(in_file_path_1);
    DeleteTemporaryFile(in_file_path_2);
    files_to_merge_.push_back(out_file_path);
  }
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

void FileMerger::DeleteTemporaryFile(const std::string& file_path) {
  if (delete_temporary_ &&
      file_path.compare(0, temp_storage_.size(), temp_storage_) == 0) {
    std::filesystem::remove(file_path);
  }
}