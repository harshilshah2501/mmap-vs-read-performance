<<<<<<< HEAD
# mmap-vs-read-performance
This PoC explores a performance comparison between these two methods by analyzing a scenario where a large file is accessed randomly.
=======

# mmap vs read Performance Comparison

This project demonstrates a performance comparison between memory-mapped I/O (`mmap()`) and the `read()` system call in C programming. The focus is on scenarios where random access to large files is required, showcasing how `mmap()` can outperform `read()` under specific conditions.

## Objective
To test and analyze the performance of `mmap()` and `read()` by:
- Measuring execution time, context switches, and page faults.
- Understanding the scenarios where `mmap()` provides significant advantages over `read()`.

## Prerequisites
- GCC or compatible C compiler
- Linux system with `perf` installed
- A 1 GB or larger file for testing

## Setup Instructions

### 1. Generate a Test File
Generate a large file (e.g., 1 GB) to test the performance:
```bash
dd if=/dev/urandom of=large_file.txt bs=1M count=1024
```

### 2. Compile the Programs
Compile the `mmap_test.c` and `read_test.c` files:
```bash
gcc -o mmap_test mmap_test.c
gcc -o read_test read_test.c
```

### 3. Run the Tests
Execute both programs with `perf` to gather performance data:
```bash
sudo perf stat -e context-switches,page-faults,cpu-migrations,cycles,instructions ./mmap_test large_file.txt
sudo perf stat -e context-switches,page-faults,cpu-migrations,cycles,instructions ./read_test large_file.txt
```

Alternatively, use the provided `benchmark.sh` script to automate the process:
```bash
chmod +x benchmark.sh
./benchmark.sh
```

### 4. Analyze the Results
Review the formatted output, which includes:
- Execution time (elapsed time for program completion)
- Context switches (how often the CPU switches between tasks)
- Page faults (memory access issues handled by the kernel)
- Kernel vs. user time (time spent in kernel-mode vs user-mode)

## Example Results
Below is an example of the formatted output from running the tests:

```
----------------------------------------------------------
| Metric               | mmap_test   | read_test  | Observations                           |
----------------------------------------------------------
| Execution Time       | 0.812s      | 1.531s     | mmap is faster for random access       |
| Context Switches     | 4           | 7          | mmap avoids repeated system calls      |
| Page Faults          | 16426       | 42         | mmap triggers on-demand paging         |
| Kernel Time          | 0.064s      | 0.675s     | read spends more time in kernel        |
----------------------------------------------------------
```

## Expected Outcome
- `mmap()` is expected to outperform `read()` for random access due to:
  - **Reduced System Calls**: `mmap()` maps the file into memory once, allowing subsequent access directly in user space.
  - **Efficient Random Access**: Accessing random locations in memory is faster than repeatedly seeking and reading from disk.

## Cleanup
The `benchmark.sh` script cleans up all temporary files after execution, including:
- The compiled binaries (`mmap_test`, `read_test`).
- The large test file (`large_file.txt`).

If running manually, you can clean up with:
```bash
rm mmap_test read_test large_file.txt
```

## License
This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
>>>>>>> 0e17d2a (Benchmarking tool for mmap vs read performance comparison with source files.)
