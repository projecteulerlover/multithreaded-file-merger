# Merge Sort Files Tool

An implementation of a file merging tool which when given a directory containing
new line character delimited text files, aggregates all tokens into a single new
line character delimited text file. Duplicate tokens across all files should
appear a single time, and empty tokens are ignored.

Deals with the following scaled up situation
1. many large files, which cannot be held in memory

2. the aggregate file could be held in disk.

## Usaged
1. Compile with
```
g++ file_merger_main.cpp -o file_merger_main.out file_merger.cpp file_merger.h -std=c++17 -Wall -Wextra -g -lstdc++fs -lpthread
```

2. Run with .\output\main {fully qualified input files folder} {fully qualified
output file}, e.g.
```
./file_merger_main.out /home/kan/repos/multithreaded-file-merger/tests/data/large_test /home/kan/repos/multithreaded-file-merger/tests/data/large_test_merged_files.txt
```

3. {fully qualified output file} contains the combined unique merge sorted
tokens.

## Implementation
In short, this is a multithreaded merge sort implementation.
1. Iterate over all files in the {fully qualified input files folder} and insert
file names into a linked list.

2. Repeatedly pop the first two file names, and perform a "merge two sorted
lists" step by using string streams to read buffers from the file.

3. If after popping the linked list is not yet empty, the merged file is stored
in a temporary location ({fully qualified input files folder} + "\temp\" +
{incrementing index}). Otherwise, we are in the final merge, and the results are
instead written to {fully qualified output file}.

## Assumptions
1. No other tool is using the same disk location (input, output, and temp files
are not locked while aggregating).

2. Does not attempt to do error handling for misreads/miswrites.

3. The disk can hold all the input files and the merged file at the same time.
At most 2x the size of the input files are stored simultaneously.

## Tests

Three test sets have been included.

The directories `tests/data/test_0` and `tests/data/test_1` contain tests for 
the tool's business logic.

The directory `tests/data/large_test` contains 10 test folders with 10657 words
total.