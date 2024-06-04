#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define LOG_FILE "logfile.txt"
#define BUFFER_SIZE 1024

int main() {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    // Open log file for appending
    int log_fd = open(LOG_FILE, O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (log_fd < 0) {
        perror("Failed to open log file");
        return 1;
    }

    // Read data from stdin (provided by inetd/xinetd)
    while ((bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0) {
        // Write data to log file
        if (write(log_fd, buffer, bytes_read) != bytes_read) {
            perror("Failed to write to log file");
            close(log_fd);
            return 1;
        }
    }

    if (bytes_read < 0) {
        perror("Failed to read from socket");
    }

    close(log_fd);  // Close the log file
    return 0;
}
