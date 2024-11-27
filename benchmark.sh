#!/bin/bash

# Script settings
TEST_FILE="large_file.txt"
MMAP_PROG="mmap_test"
READ_PROG="read_test"
FILE_SIZE_MB=1024

# Step 1: Compile the programs
echo "Compiling programs..."
gcc -o "$MMAP_PROG" mmap_test.c || { echo "Failed to compile mmap_test.c"; exit 1; }
gcc -o "$READ_PROG" read_test.c || { echo "Failed to compile read_test.c"; exit 1; }

# Step 2: Create a large test file
echo "Generating $FILE_SIZE_MB MB test file..."
dd if=/dev/urandom of="$TEST_FILE" bs=1M count="$FILE_SIZE_MB" status=progress

# Step 3: Run performance tests
echo "Running mmap_test..."
mmap_results=$(sudo perf stat -e context-switches,page-faults,cpu-migrations,cycles,instructions ./"$MMAP_PROG" "$TEST_FILE" 2>&1)
echo "Running read_test..."
read_results=$(sudo perf stat -e context-switches,page-faults,cpu-migrations,cycles,instructions ./"$READ_PROG" "$TEST_FILE" 2>&1)

# Step 4: Extract metrics
extract_metric() {
    local results=$1
    local metric=$2
    echo "$results" | awk -v m="$metric" '$0 ~ m {print $1}'
}

mmap_time=$(echo "$mmap_results" | awk '/seconds time elapsed/ {print $1}')
mmap_context_switches=$(extract_metric "$mmap_results" "context-switches")
mmap_page_faults=$(extract_metric "$mmap_results" "page-faults")
mmap_kernel_time=$(echo "$mmap_results" | awk '/seconds sys/ {print $1}')

read_time=$(echo "$read_results" | awk '/seconds time elapsed/ {print $1}')
read_context_switches=$(extract_metric "$read_results" "context-switches")
read_page_faults=$(extract_metric "$read_results" "page-faults")
read_kernel_time=$(echo "$read_results" | awk '/seconds sys/ {print $1}')

# Step 5: Format results
echo "----------------------------------------------------------"
echo "| Metric               | mmap_test   | read_test  | Observations                           |"
echo "----------------------------------------------------------"
printf "| Execution Time       | %-11s | %-10s | mmap is faster for random access       |\n" "$mmap_time" "$read_time"
printf "| Context Switches     | %-11s | %-10s | mmap avoids repeated system calls      |\n" "$mmap_context_switches" "$read_context_switches"
printf "| Page Faults          | %-11s | %-10s | mmap triggers on-demand paging         |\n" "$mmap_page_faults" "$read_page_faults"
printf "| Kernel Time          | %-11s | %-10s | read spends more time in kernel        |\n" "$mmap_kernel_time" "$read_kernel_time"
echo "----------------------------------------------------------"

# Step 6: Cleanup
echo "Cleaning up..."
rm -f "$MMAP_PROG" "$READ_PROG" "$TEST_FILE"
echo "Done."