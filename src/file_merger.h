#ifndef SRC_FILE_MERGER_H
#define SRC_FILE_MERGER_H

#include <string>
#include <string_view>

class FileMerger {
 public:
  FileMerger(std::string_view input_folder_path,
             std::string_view output_file_path)
      : input_folder_path_(input_folder_path),
        output_file_path_(output_file_path) {
    temp_storage_ = input_folder_path_ + "//temp//";
  };

  void MergeSortFiles();

 private:
  std::string MergeSortTwoFiles(const std::string& file_path_1,
                                const std::string& file_path_2,
                                const std::string& out_file_path);

  const std::string input_folder_path_;
  const std::string output_file_path_;
  std::string temp_storage_;
};

#endif  // SRC_FILE_MERGER_H