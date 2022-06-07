#ifndef SRC_FILE_MERGER_H
#define SRC_FILE_MERGER_H

#include <algorithm>
#include <mutex>
#include <string>
#include <string_view>
#include <thread>

class FileMerger {
 public:
  FileMerger(std::string_view input_folder_path,
             std::string_view output_file_path, bool delete_temporary = false)
      : input_folder_path_(input_folder_path),
        output_file_path_(output_file_path),
        delete_temporary_(delete_temporary) {
    temp_storage_ = input_folder_path_ + "//temp//";
    max_threads_ = std::min(1, int(std::thread::hardware_concurrency()));
  };

  void MergeSortFiles();

 private:
  void ThreadPoolWorker();

  std::string MergeSortTwoFiles(const std::string& file_path_1,
                                const std::string& file_path_2,
                                const std::string& out_file_path);

  void DeleteTemporaryFile(const std::string& file_path);

  const std::string input_folder_path_;
  const std::string output_file_path_;
  std::string temp_storage_;
  bool delete_temporary_;
  std::vector<std::string> files_to_merge_;
  // Index used as temporary file names for merged intermediaries.
  int temp_index_ = 0;

  // Multithreading member variables.
  int max_threads_;
  int working_threads_ = 0;
  std::mutex mutex_;
  std::condition_variable cv_;
  bool completed_ = false;
  std::vector<std::thread> thread_pool_;
};

#endif  // SRC_FILE_MERGER_H