#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define SIZE 256

// New type for shell local variables represented as a key and value.
typedef struct {
    char *key;
    char *value;
} shellLocalVar;

// New type for command, arguments and stdin, out, err.

typedef struct {
    char *command;
    char *argArr[SIZE];
    char *stdoutFile;
    char *stdinFile;
    char *stderrFile;
} ParsedCommand;

extern char **environ;  // Global environment variables

// Function declarations.
void tokenize(shellLocalVar *localVars[], char buff[], ParsedCommand *cmd);
void echo(char *args[]);
void pwd(char currentDir[]);
int addLocalVar(shellLocalVar *localVars[], shellLocalVar *newVar);
int searchLocalVar(shellLocalVar *arr[], char *key);
char *getEnvVarValue(const char *key);
void freeParsedCommand(ParsedCommand *cmd);
int isBuiltIn(char *cmd);
void restoreOriginalFDs(int *originalFDs);
void saveOriginalFDs(int *originalFDs);

// Main function.
int main(int argc, char **argv) {
    char buff[SIZE];
    char currentDir[SIZE];
    int status;
    // char *argArr[100];
    shellLocalVar *localVars[100] = {NULL};
    shellLocalVar currVar;
    ParsedCommand cmd;

    // Main loop.
    while (1) {
        pwd(currentDir);
        printf("\033[1;31m%s@Micro_shell:\033[0m \033[36m%s\033[0m $ ", getEnvVarValue("USER"), currentDir);
        fgets(buff, SIZE, stdin);
        //              buff[strlen(buff)-1] = 0; // replace the '\n' character with null terminator.

        if (strlen(buff) == 0) continue;

        tokenize(localVars, buff, &cmd);  // tokanize user input line.
        // Check for Local Variable Assignment.
        char *equalSign = strchr(cmd.command, '=');

        if (isBuiltIn(cmd.command)) {
            // Apply redirection for built-ins (in parent)
            int originalFDs[3];
            int redirected = 0;

            if (cmd.stdinFile || cmd.stdoutFile || cmd.stderrFile) {
                redirected = 1;
                saveOriginalFDs(originalFDs);
                // Handle stdout redirection
                if (cmd.stdoutFile) {
                    int fd = open(cmd.stdoutFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (fd < 0) {
                        perror("open stdout");
                        // exit (EXIT_FAILURE);
                    }
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                }

                // Handle stderr redirection
                if (cmd.stderrFile) {
                    int fd = open(cmd.stderrFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (fd < 0) {
                        perror("open stderr");
                        // exit (EXIT_FAILURE);
                    }
                    dup2(fd, STDERR_FILENO);
                    close(fd);
                }
            }
            // Handle stdin redirection
            if (cmd.stdinFile) {
                int fd = open(cmd.stdinFile, O_RDONLY);
                if (fd < 0) {
                    perror("open stdin");
                    // exit (EXIT_FAILURE);
                }
                dup2(fd, STDIN_FILENO);
                close(fd);
            }

            // Built-in commands and new process creation (linux native commands).
            if (strcmp(cmd.command, "echo") == 0) {
                echo(cmd.argArr);
            } else if (strcmp(cmd.command, "pwd") == 0) {
                printf("%s\n", currentDir);
            } else if (strcmp(cmd.command, "cd") == 0) {
                // Note: chdir() function only accepts path starting with '/'.
                if (chdir(cmd.argArr[1]) < 0) {
                    perror("Invalid Path!!\n");
                }
            } else if (strcmp(cmd.command, "exit") == 0) {
                exit(0);
            } else if (strcmp(cmd.command, "export") == 0)  // Add a Global Variable.
            {
                char *equalSign = strchr(cmd.argArr[1], '=');
                if (equalSign == NULL) {
                    printf("ERROR: can't create empty variable\n");
                    printf("usage: export varName=\"value\"\n");
                } else {
                    const char *value = equalSign + 1;
                    int keyLen = equalSign - cmd.argArr[1];
                    char *name = (char *) malloc(sizeof(char) * (keyLen) + 1);
                    name = strncpy(name, cmd.argArr[1], keyLen);
                    name[keyLen] = '\0';
                    setenv(name, value, 1);
                }
            } else if (strcmp(cmd.command, "unset") == 0)  // Delete a Global Variable.
            {
                unsetenv(cmd.argArr[1]);
            }
            // Restore original file descriptors
            if (redirected) {
                restoreOriginalFDs(originalFDs);
            }
        } else if ((cmd.argArr[1] == NULL) && (equalSign != NULL))  // Assign a Local Variable.
        {
            // Local Variables
            int i = 0;
            int keyLen = equalSign - cmd.command;
            currVar.key = (char *) malloc(sizeof(char) * (keyLen + 1));
            strncpy(currVar.key, cmd.command, keyLen);
            currVar.key[keyLen] = '\0';

            int valueLen = strlen(equalSign + 1);
            currVar.value = (char *) malloc(sizeof(char) * (valueLen + 1));
            strcpy(currVar.value, equalSign + 1);

            if (addLocalVar(localVars, &currVar) != 0) {
                printf("Local Var Not Added!!\n");
            }

        } else {
            pid_t child_pid = fork();
            if (child_pid == 0) {
                // char *argv[] = { NULL };
                // char *envp[] = { NULL };

                // Handle I/O Redirection For Child Process.
                // 1. Stdout File Redirection.
                if (cmd.stdoutFile) {
                    int fd = open(cmd.stdoutFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (fd < 0) {
                        perror("open stdout");
                        exit(EXIT_FAILURE);
                    }
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                }
                // 2. Stderr File Redirection.
                if (cmd.stderrFile) {
                    int fd = open(cmd.stderrFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (fd < 0) {
                        perror("open stderr");
                        exit(EXIT_FAILURE);
                    }
                    dup2(fd, STDERR_FILENO);
                    close(fd);
                }
                // 3. Stdin File Redirection.
                if (cmd.stdinFile) {
                    int fd = open(cmd.stdinFile, O_RDONLY);
                    if (fd < 0) {
                        perror("open stdin");
                        exit(EXIT_FAILURE);
                    }
                    dup2(fd, STDIN_FILENO);
                    close(fd);
                }

                execvp(cmd.command, cmd.argArr);  // Supports program execution directly (check `man exec`).

                printf("ERROR: %s Execution Faild!!\n", cmd.command);
                exit(-1);
            } else if (child_pid > 0) {
                wait(&status);
                // printf
                ("PARENT: I'm %d, and my child returned with a status: %d\n", getpid(), WEXITSTATUS(status));
            } else {
                printf("ERROR: can't create process\n");
                exit(-1);
            }
        }
        freeParsedCommand(&cmd);
    }
    // Memory de-allocation.
    int i = 0;
    free(currVar.key);
    free(currVar.value);

    return 0;
}

// Helper Functions.

// Save Original Default File Descriptors Helper Function.
void saveOriginalFDs(int *originalFDs) {
    originalFDs[0] = dup(STDIN_FILENO);
    originalFDs[1] = dup(STDOUT_FILENO);
    originalFDs[2] = dup(STDERR_FILENO);
}

// Restore Original Default File Descriptors Helper Function.
void restoreOriginalFDs(int *originalFDs) {
    dup2(originalFDs[0], STDIN_FILENO);
    dup2(originalFDs[1], STDOUT_FILENO);
    dup2(originalFDs[2], STDERR_FILENO);
    close(originalFDs[0]);
    close(originalFDs[1]);
    close(originalFDs[2]);
}

// Check if command is built-in Helper Function.
int isBuiltIn(char *cmd) {
    return strcmp(cmd, "echo") == 0 || strcmp(cmd, "pwd") == 0 || strcmp(cmd, "cd") == 0 || strcmp(cmd, "exit") == 0 ||
           strcmp(cmd, "export") == 0 || strcmp(cmd, "unset") == 0;
}

// Free memory allocated for parsed command.
void freeParsedCommand(ParsedCommand *cmd) {
    if (cmd->command) free(cmd->command);
    for (int i = 0; cmd->argArr[i]; i++) free(cmd->argArr[i]);
    if (cmd->stdinFile) free(cmd->stdinFile);
    if (cmd->stdoutFile) free(cmd->stdoutFile);
    if (cmd->stderrFile) free(cmd->stderrFile);
    memset(cmd, 0, sizeof(ParsedCommand));  // reset all pointers
}

// Helper Functions for local Varibles manipulation
// Add new local Variable Function Definition.

int addLocalVar(shellLocalVar *localVars[], shellLocalVar *newVar) {
    int i = 0;
    int index = searchLocalVar(localVars, newVar->key);
    if (index == -1) {
        while (localVars[i] != NULL && i < 100) {
            i++;
        }
        if (i >= 100) {
            return -1;
        }

        if ((localVars[i] = (shellLocalVar *) malloc(sizeof(shellLocalVar))) == NULL) {
            return -1;
        }

        localVars[i]->key = strdup(newVar->key);
        localVars[i]->value = strdup(newVar->value);

        if (localVars[i]->key == NULL || localVars[i]->value == NULL) {
            free(localVars[i]->key);
            free(localVars[i]->value);
            free(localVars[i]);
            return -1;
        }

        if (i + 1 < 100) {
            localVars[i + 1] = NULL;
        }

        return 0;
    } else {
        free(localVars[index]->value);
        localVars[index]->value = strdup(newVar->value);
        if (localVars[index]->value == NULL) {
            return -1;
        }
        return 0;
    }

    return -1;  // shouldn't reach here.
}

int searchLocalVar(shellLocalVar *arr[], char *key) {
    int i = 0;
    while (arr[i] != NULL && i < 100) {
        if (strcmp(arr[i]->key, key) == 0) {
            return i;
        }
        i++;
    }

    return -1;
}

// Function to search for a variable in environment variables
char *getEnvVarValue(const char *key) {
    if (key == NULL) return NULL;

    for (char **env = environ; *env != NULL; env++) {
        char *equal_sign = strchr(*env, '=');
        if (equal_sign != NULL) {
            size_t key_len = equal_sign - *env;
            if (strncmp(*env, key, key_len) == 0 && key[key_len] == '\0') {
                return equal_sign + 1;
            }
        }
    }
    return NULL;
}

// Command line parsing function definition.
void tokenize(shellLocalVar *localVars[], char buff[], ParsedCommand *cmd) {
    int i = 0, j = 0, count = 0;
    cmd->command = NULL;
    cmd->stdoutFile = NULL;
    cmd->stdinFile = NULL;
    cmd->stderrFile = NULL;

    // Remove trailing newline from fgets()
    size_t len = strlen(buff);
    if (len > 0 && buff[len - 1] == '\n') {
        buff[len - 1] = '\0';
    }

    while (buff[j] != '\0') {
        // Skip leading spaces
        while (buff[j] == ' ') j++;

        if (buff[j] == '\0') break;  // If only spaces are left, stop

        i = j;  // Start of word

        // Find the end of the word
        while (buff[j] != ' ' && buff[j] != '\0') {
            j++;
        }

        // Only allocate if we have a non-empty token
        if (j > i) {
            int tokenLen = j - i;
            char *token = malloc(tokenLen + 1);
            if (token == NULL) {
                perror("Memory allocation faild");
                exit(EXIT_FAILURE);
            }
            // Copy the word
            strncpy(token, buff + i, tokenLen);
            token[tokenLen] = '\0';

            if (token[0] == '$') {
                char *key = token + 1;
                char *value = NULL;
                int index;

                if ((index = searchLocalVar(localVars, key)) != -1) {
                    value = (char *) malloc(sizeof(char) * strlen(localVars[index]->value));
                    strcpy(value, localVars[index]->value);
                }

                if (value == NULL) {
                    value = getEnvVarValue(key);
                }

                if (value != NULL) {
                    free(token);
                    token = strdup(value);
                    if (token == NULL) {
                        perror("Memory allocation faild");
                        exit(EXIT_FAILURE);
                    }
                } else {
                    token = " ";
                }
            }
            // Handle redirection
            if (strcmp(token, ">") == 0 || strcmp(token, "<") == 0 || strcmp(token, "2>") == 0) {
                // Determine redirection type
                int is_stdout = (strcmp(token, ">") == 0);
                int is_stdin = (strcmp(token, "<") == 0);
                int is_stderr = (strcmp(token, "2>") == 0);
                free(token);  // Free the token now

                // Extract filename
                while (buff[j] == ' ') j++;
                i = j;
                while (buff[j] != ' ' && buff[j] != '\0') j++;
                int fnameLen = j - i;
                char *filename = malloc(fnameLen + 1);
                strncpy(filename, buff + i, fnameLen);
                filename[fnameLen] = '\0';

                // Assign to appropriate field
                if (is_stdout) {
                    cmd->stdoutFile = filename;
                } else if (is_stdin) {
                    cmd->stdinFile = filename;
                } else if (is_stderr) {
                    cmd->stderrFile = filename;
                }
                continue;
            }
            // Handle inline redirection (e.g. >out.txt)
            else if (strncmp(token, "2>", 2) == 0) {
                cmd->stderrFile = strdup(token + 2);
                free(token);
                continue;
            } else if (token[0] == '>') {
                cmd->stdoutFile = strdup(token + 1);
                free(token);
                continue;
            } else if (token[0] == '<') {
                cmd->stdinFile = strdup(token + 1);
                free(token);
                continue;
            }
            if (!cmd->command) {
                cmd->command = strdup(token);
                if (!cmd->command) {
                    printf("Command Mem Allocation Faild!!\n");
                    exit(EXIT_FAILURE);
                }
            }

            cmd->argArr[count++] = strdup(token);
        }
        // Skip spaces between words
        while (buff[j] == ' ') j++;
    }

    cmd->argArr[count] = NULL;  // Null-terminate the array
}

// built-in `echo` command function definition.
void echo(char *args[]) {
    int i = 1;
    while (args[i] != NULL) {
        int argLen = strlen(args[i]);
        // Print Arguemnts
        if (write(1, args[i], argLen) < 0) {
            printf("ERROR: writing faild!!\n");
            exit(-1);
        }
        // print space to separate between arguments.
        write(1, " ", 1);
        i++;
    }
    printf("\n");
}

// built-in `pwd` command function defintion.
void pwd(char currentDir[]) {
    char buff[SIZE];

    if (getcwd(buff, SIZE) == NULL) {  // Function returns NULL on Failure.
        printf("ERROR: can't print current working directory\n");
        exit(-1);
    }

    int buffLen = strlen(buff);

    strcpy(currentDir, buff);
}
