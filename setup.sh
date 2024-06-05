#!/bin/bash

# Compile the message queue programs
gcc -o sender sender.c
gcc -o receiver receiver.c

# Function to clear the message queue
clear_message_queue() {
    local queue_key=12345
    ipcrm -Q $queue_key 2>/dev/null
}

# Run message queue tests
run_message_queue_tests() {
    # Parameters to test
    message_sizes=(10 50 256)
    num_messages=10
    delay=1

    for size in "${message_sizes[@]}"; do
        clear_message_queue  # Clear the message queue before each test

        echo "Running receiver for message size: $size"
        ./receiver $num_messages &
        receiver_pid=$!
        sleep 2  # Give time for the receiver to start

        echo "Running sender with message size: $size"
        ./sender $size $num_messages

        wait $receiver_pid
    done
}

# Execute the tests
run_message_queue_tests

echo "All tests complete."
