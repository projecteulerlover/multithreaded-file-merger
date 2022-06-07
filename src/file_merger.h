#ifndef SRC_FILE_MERGER_H
#define SRC_FILE_MERGER_H

#include <algorithm>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

class FileMerger {
 public:
  FileMerger(std::string_view input_folder_path,
             std::string_view output_file_path, bool delete_temporary = false)
      : input_folder_path_(input_folder_path),
        output_file_path_(output_file_path),
        delete_temporary_(delete_temporary) {
    temp_storage_ = input_folder_path_ + "/temp/";
    max_threads_ = std::thread::hardware_concurrency();
  };

  // Uniquely merge sorts all lines in all files in the given
  // `input_folder_path_` and writes to `output_file_path_`. Initializes a
  // thread pool to repeatedly pull two files from the `files_to_merge_` work
  // queue and merges them.
  void MergeSortFiles();

 private:
  // Worker thread.
  // 1. Under a lock,
  //      - increments `working_threads`
  //      - pull two file names from `files_to_merge` (or sleep if unable)
  //      - constructs a temp file to write to,
  // 2. Releases lock and merges the files and write to temp file.
  // 3. Reaquire lock to push new file back into work queue. Decrements
  // `working_threads`.
  // 4. Signals other worker threads.
  void ThreadPoolWorker(const int thread_number);

  // Business logic to merge unique entries from `file_path_1` and
  // `file_path_2`, both of which have lines lexicographically ordered.
  std::string MergeSortTwoFiles(const std::string &file_path_1,
                                const std::string &file_path_2,
                                const std::string &out_file_path);

  // Helper method to delete temporary files after they have been merged.
  void DeleteTemporaryFile(const std::string &file_path);

  // Multithreading member variables.
  int max_threads_;
  int working_threads_ = 0;
  std::mutex mutex_;
  std::condition_variable cv_;
  bool completed_ = false;
  std::vector<std::thread> thread_pool_;

  // Folder containing original data.
  const std::string input_folder_path_;
  // File to write merged data.
  const std::string output_file_path_;
  // Folder to store partially merged data.
  std::string temp_storage_;
  // Whether to delete temporary files.
  bool delete_temporary_;
  // Work queue containing files to be merged. Partially merged temporaries are
  // also pushed here.
  std::vector<std::string> files_to_merge_;
  // Index used as temporary file names for merged intermediaries.
  int temp_index_ = 0;
};

#endif  // SRC_FILE_MERGER_H