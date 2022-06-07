#include "file_merger.h"

#include <algorithm>
#include <cstring>
#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <random>
#include <string>
#include <thread>

namespace {
void Writer(std::ofstream& out_file, std::string& line,
            std::string& last_line) {
  if (!line.empty() && line != last_line) {
    out_file << line << "\n";
    last_line = std::move(line);
  }
}
}  // namespace

void FileMerger::ThreadPoolWorker(const int thread_number) {
  while (true) {
    std::unique_lock<std::mutex> u_lock(mutex_);
    cv_.wait(u_lock,
             [&]() { return files_to_merge_.size() > 1 || completed_; });
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
        files_to_merge_.empty() && working_threads_ == 1
            ? output_file_path_
            : temp_storage_ + std::to_string(temp_index_++) + ".txt";
    std::cout << "Thread #" << thread_number << " merging sorted files "
              << in_file_path_1 << " and " << in_file_path_2 << " into "
              << out_file_path << "\n"
              << std::endl;
    u_lock.unlock();
    MergeSortTwoFiles(in_file_path_1, in_file_path_2, out_file_path);

    u_lock.lock();

    files_to_merge_.push_back(out_file_path);
    --working_threads_;
    if (working_threads_ == 0 && files_to_merge_.size() <= 1) {
      completed_ = true;
    }
    cv_.notify_all();
  }
}

void FileMerger::MergeSortFiles() {
  auto directory_itr =
      std::experimental::filesystem::directory_iterator(input_folder_path_);
  for (const auto& file : directory_itr) {
    if (!is_directory(file) && file.path().string() != output_file_path_) {
      files_to_merge_.push_back(file.path().string());
    }
  }
  if (files_to_merge_.size() == 0) {
    std::cout << "No input files in given directory " << input_folder_path_ <<  std::endl;
    return;
  }
  std::experimental::filesystem::create_directory(temp_storage_);
  for (int i = 0; i < max_threads_; ++i) {
    thread_pool_.emplace_back(&FileMerger::ThreadPoolWorker, this, i);
  }
  for (auto& t : thread_pool_) {
    t.join();
  }
  if (delete_temporary_) {
    std::experimental::filesystem::remove_all(temp_storage_);
  }
  std::cout << "Merging complete." << std::endl;
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
  while (!in_1.eof() && !in_2.eof()) {
    if (token_1.empty()) {
      std::getline(in_1, token_1, '\n');
      continue;
    }
    if (token_2.empty()) {
      std::getline(in_2, token_2, '\n');
      continue;
    }
    int cmp = token_1.compare(token_2);
    if (cmp <= 0) {
      Writer(out_file, token_1, last_write);
      std::getline(in_1, token_1, '\n');
      if (cmp == 0) {
        std::getline(in_2, token_2, '\n');
      }
    } else {
      Writer(out_file, token_2, last_write);
      std::getline(in_2, token_2, '\n');
    }
  }
  do {
    Writer(out_file, token_1, last_write);
  } while (getline(in_1, token_1, '\n'));
  do {
    Writer(out_file, token_2, last_write);
  } while (getline(in_2, token_2, '\n'));

  in_1.close();
  in_2.close();
  out_file.close();
  return out_file_path;
}

void FileMerger::DeleteTemporaryFile(const std::string& file_path) {
  if (delete_temporary_ &&
      file_path.compare(0, temp_storage_.size(), temp_storage_) == 0) {
    std::experimental::filesystem::remove(file_path);
  }
}