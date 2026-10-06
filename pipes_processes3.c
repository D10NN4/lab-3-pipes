#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

/**
 * Executes the command "cat scores | grep Lakers".  In this quick-and-dirty
 * implementation the parent doesn't wait for the child to finish and
 * so the command prompt may reappear before the child terminates.
 *
 */

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <search_term>\n", argv[0]);
        return 1;
    }

    int pipefd1[2]; // Pipe between cat and grep
    int pipefd2[2]; // Pipe between grep and sort

    pid_t pid1, pid2;

    char *cat_args[] = {"cat", "scores", NULL};
    char *grep_args[] = {"grep", argv[1], NULL};
    char *sort_args[] = {"sort", NULL};

    // Create pipe 1
    if (pipe(pipefd1) == -1) {
        perror("pipe1 failed");
        return 1;
    }

  // Create pipe 2
    if (pipe(pipefd2) == -1) {
        perror("pipe2 failed");
        return 1;
    }

    // Fork first child process
    pid1 = fork();

    if (pid1 < 0) {
        perror("fork1 failed");
        return 1;
    }

    if (pid1 == 0) {
        // Child process: handle grep and fork grandchild for sort
        pid2 = fork();

        if (pid2 < 0) {
            perror("fork2 failed");
            return 1;
        }

        if (pid2 == 0) {
            // P3: Grandchild process executing "sort"
            dup2(pipefd2[0], 0);

            // Close all pipe file descriptors
            close(pipefd1[0]);
            close(pipefd1[1]);
            close(pipefd2[0]);
            close(pipefd2[1]);

            execvp("sort", sort_args);
            perror("execvp sort failed");
            exit(1);
        } else {
            // P2: Child process executing "grep <arg>"
            dup2(pipefd1[0], 0);
            dup2(pipefd2[1], 1);

            // Close all pipe file descriptors
            close(pipefd1[0]);
            close(pipefd1[1]);
            close(pipefd2[0]);
            close(pipefd2[1]);

            execvp("grep", grep_args);
            perror("execvp grep failed");
            exit(1);
        }
    } else {
        // Parent process P1 executing "cat scores"
        dup2(pipefd1[1], 1);

        // Close all pipe file descriptors in parent
        close(pipefd1[0]);
        close(pipefd1[1]);
        close(pipefd2[0]);
        close(pipefd2[1]);

        execvp("cat", cat_args);
        perror("execvp cat failed");
        exit(1);
    }

    return 0;
}