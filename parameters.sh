#!/bin/bash



# Compile the shared memory programs
gcc -o writer task3.2/writer.c
gcc -o reader task3.2/reader.c
gcc -o writer_measure task3.2/writer_measure.c

# Run parameter variations for shared memory
run_shared_memory_tests() {
    # Data sizes to test
    data_sizes=(256 512 1024)

    for size in "${data_sizes[@]}"; do
        echo "Running writer_measure with data size: $size"
        ./reader &
        reader_pid=$!
        sleep 2  # Give time for the reader to start
        ./writer_measure $size
        wait $reader_pid
    done
}

# Run the shared memory tests
run_shared_memory_tests

# Indicate that all tests are complete
echo "All tests complete."
