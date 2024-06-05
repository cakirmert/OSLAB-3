#!/bin/bash

# Compile the message queue programs
gcc -o sender task3.1/sender.c
gcc -o receiver task3.1/receiver.c

# Compile the shared memory programs
gcc -o writer task3.2/writer.c
gcc -o reader task3.2/reader.c
gcc -o writer_measure task3.2/writer_measure.c

# Function to clear the message queue
clear_message_queue() {
    local queue_key=12345
    local msg_id

    msg_id=$(ipcs -q | awk -v key=$queue_key '$1 ~ key {print $2}')
    if [ -n "$msg_id" ]; then
        ipcrm -q $msg_id
    fi
}

# Run parameter variations for message queues
run_message_queue_tests() {
    # Parameters to test
    message_sizes=(10 50 256)
    num_messages=1
    delay=1

    for size in "${message_sizes[@]}"; do
        clear_message_queue  # Clear the message queue before each test

        echo "Running receiver for message size: $size"
        ./receiver $num_messages $delay &
        receiver_pid=$!
        sleep 2  # Ensure the receiver is ready

        echo "Running sender with message size: $size"
        ./sender $size $num_messages $delay

        wait $receiver_pid
    done
}

# Run the message queue tests
run_message_queue_tests

# Run parameter variations for shared memory
run_shared_memory_tests() {
    # Data sizes to test
    data_sizes=(256 512 1024)

    for size in "${data_sizes[@]}"; do
        echo "Running writer_measure with data size: $size"
        ./reader &
        reader_pid=$!
        sleep 2  # Ensure the reader is ready
        ./writer_measure $size
        wait $reader_pid
    done
}

# Run the shared memory tests
run_shared_memory_tests

# Indicate that all tests are complete
echo "All tests complete."
