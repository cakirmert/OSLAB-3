#!/bin/bash

# Script must be executable first, with chmod +x setup.sh

# Clone the repository
git clone https://github.com/cakirmert/OSLAB-3.git

# Navigate to the repository directory
cd OSLAB-3

# Compile the server program
gcc -o server task3.0\(2.3\)/server.c

# Compile the message queue programs
gcc -o sender task3.1/sender.c
gcc -o receiver task3.1/receiver.c

# Compile the shared memory programs
gcc -o writer task3.2/writer.c
gcc -o reader task3.2/reader.c
gcc -o writer_measure task3.2/writer_measure.c

# Indicate that the setup is complete
echo "Setup complete. All programs have been compiled."
