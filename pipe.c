#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main() {
    int pipe1[2];
    int pipe2[2];
    char buffer[256];
    const char* message1 = "Hello from the parent to child1!";
    const char* message2 = "Hello from the parent to child2!";
    const char* message3 = "Hello from child1 to child3!";

    // Create the first pipe (between parent and child1)
    if (pipe(pipe1) == -1) {
        perror("pipe1");
        return 1;
    }

    // Create the second pipe (between parent and child2)
    if (pipe(pipe2) == -1) {
        perror("pipe2");
        return 1;
    }

    // Fork child1 process
    pid_t child1_pid = fork();
    if (child1_pid == -1) {
        perror("fork (child1)");
        return 1;
    }

    if (child1_pid == 0) {
        // Child1 process
        close(pipe1[1]);  // Close the write end of the first pipe

        // Read from the first pipe (data from the parent)
        read(pipe1[0], buffer, sizeof(buffer));
        printf("Child1 received: %s\n", buffer);

        close(pipe1[0]);  // Close the read end of the first pipe

        // Child1 writes to the third pipe (to child3)
        close(pipe2[0]);  // Close the read end of the second pipe
        write(pipe2[1], message3, strlen(message3) + 1);
        close(pipe2[1]);  // Close the write end of the second pipe

        return 0;
    }

    // Fork child2 process
    pid_t child2_pid = fork();
    if (child2_pid == -1) {
        perror("fork (child2)");
        return 1;
    }

    if (child2_pid == 0) {
        // Child2 process
        close(pipe2[1]);  // Close the write end of the second pipe

        // Read from the second pipe (data from the parent)
        read(pipe2[0], buffer, sizeof(buffer));
        printf("Child2 received: %s\n", buffer);

        close(pipe2[0]);  // Close the read end of the second pipe

        return 0;
    }

    // Parent process
    close(pipe1[0]);  // Close the read end of the first pipe
    close(pipe2[0]);  // Close the read end of the second pipe

    // Write to the first pipe (to child1)
    write(pipe1[1], message1, strlen(message1) + 1);
    close(pipe1[1]);  // Close the write end of the first pipe

    // Write to the second pipe (to child2)
    write(pipe2[1], message2, strlen(message2) + 1);
    close(pipe2[1]);  // Close the write end of the second pipe

    // Wait for child processes to complete
    waitpid(child1_pid, NULL, 0);
    waitpid(child2_pid, NULL, 0);

    return 0;
}
