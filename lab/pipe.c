#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    int pipefd[2];
    pid_t cat_pid, grep_pid;

    // makes pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // for and exe cat
    cat_pid = fork();
    if (cat_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (cat_pid == 0) {  // child proc for cat
        // close pipe end
        close(pipefd[0]);

        // dupes pipe
        dup2(pipefd[1], STDOUT_FILENO);

        // gets rid of extra info
        close(pipefd[1]);

        // exe cat bar txt
        char* cat_args[] = {"cat", "bar.txt", NULL};
        execvp("cat", cat_args);

        perror("execvp");  // execvp only returns if an error occurs
        exit(EXIT_FAILURE);
    } else {  // Parent process
        // Forks and completes foo
        grep_pid = fork();
        if (grep_pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (grep_pid == 0) {  // child proc for grep_pid
            // closes the pipe
            close(pipefd[1]);

            // dupes rear part of pipe
            dup2(pipefd[0], STDIN_FILENO);

            // gets rid of extra info 
            close(pipefd[0]);

            // completes the "grep foo"
            char* grep_args[] = {"grep", "foo", NULL};
            execvp("grep", grep_args);

            perror("execvp");  // execvp only returns if an error occurs
            exit(EXIT_FAILURE);
        } else {  // Parent process
            // close both sides of pipe
            close(pipefd[0]);
            close(pipefd[1]);

            // wait for both to finish
            waitpid(cat_pid, NULL, 0);
            waitpid(grep_pid, NULL, 0);
        }
    }

    return 0;
}
