#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    umask(0);
    char *help = "help";
    char *readCmd = "read";
    char *mkdirCmd = "mkdir";
    char *writeCmd = "write";

#define BUFFER_SIZE 4096
#define MAX_DIRECTORIES 200

    if (argc == 1)
    {
        printf("missing command");
        return 1;
    }

    // help
    else if (strcmp(argv[1], help) == 0)
    {
        const char *message = "\nUsage: ./syscalls <command> [arguments]\n \nCommands:\n read <file>: Read contents of a file\n write <file> <text>: Write text to a file\n mkdir <directory>: Create a directory\n";
        size_t message_len = strlen(message);
        ssize_t bytes_written = write(1, message, message_len);
    }

    // read
    else if (strcmp(argv[1], readCmd) == 0)
    {
        // if no path is specified
        if (argv[2] == NULL)
        {
            perror("missing argument");
            return 1;
        }
        else
        {
            int fd = open(argv[2], O_RDONLY);
            // if failed to open
            if (fd == -1)
            {
                printf("Failed to open %s", argv[2]);
                return 1;
            }

            char buffer[BUFFER_SIZE];
            ssize_t bytes_read = read(fd, buffer, BUFFER_SIZE);
            // if failed to read
            if (bytes_read == -1)
            {
                printf("Failed to read %s", argv[2]);
                return 1;
            }

            ssize_t bytes_written = write(1, buffer, strlen(buffer));
        }
    }

    // mkdir
    else if (strcmp(argv[1], mkdirCmd) == 0)
    {
        if (argc <= 2)
        {
            printf("missing argument");
            return 1;
        }
        else
        {
            // creating an array of all directory names in order
            char *path = argv[2];
            char *directories[MAX_DIRECTORIES];
            int directoryCount = 0;
            char *token = strtok(path, "/");
            while (token != NULL && directoryCount < MAX_DIRECTORIES)
            {
                directories[directoryCount++] = token;
                token = strtok(NULL, "/");
            }

            // creating parent directory
            int x = mkdir(directories[0]);
            if (x == -1)
            {
                if (errno == EEXIST)
                {
                    printf("%s already exists", argv[2]);
                    return 1;
                }
                else
                {
                    printf("Failed to create %s", argv[2]);
                    return 1;
                }
            }

            char current_path[256] = "";
            strcat(current_path, directories[0]);

            for (int i = 1; i < directoryCount; i++)
            {
                strcat(current_path, "/");
                strcat(current_path, directories[i]);
                x = mkdir(current_path);
                if (x == -1)
                {
                    if (errno == EEXIST)
                    {
                        printf("%s already exists", current_path);
                        return 1;
                    }
                    else
                    {
                        printf("Failed to create %s", current_path);
                        return 1;
                    }
                }
            }
        }
    }

    // write
    else if (strcmp(argv[1], writeCmd) == 0)
    {
        // getting number of arguments provided by user and handling errors if too few
        if (argc <= 2)
        {
            char *message = "missing argument";
            ssize_t bytes_written = write(1, message, strlen(message));
            return 1;
        }

        // getting the file descriptor of the path provided and creating file if it doesn't exist
        int fd = open(argv[2], O_RDWR | O_CREAT | O_APPEND, 0666);
        if (fd == -1)
        {
            printf("Failed to open %s", argv[2]);
            return 1;
        }
        // writing to file and handling errors
        int i = 3;
        ssize_t bytesToWrite;
        const char *newLine = "\n";

        while (argv[i] != NULL)
        {
            printf("hey");
            bytesToWrite = write(fd, argv[i], strlen(argv[i]));
            if (bytesToWrite == -1)
            {
                printf("Failed to write to %s", argv[2]);
                return 1;
            }
            ssize_t nextLine = write(fd, newLine, strlen(newLine));
            i++;
        }
    }

    else
    {
        perror("invalid command");
        return 1;
    }

    return 0;
}
