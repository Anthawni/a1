#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <sstream>
#include <cstring>
#include <cstdlib>

// declare functions 
void execute_command(const std::vector<std::string>& commands);
void parse_command(const std::string& command, std::vector<std::string>& args);
void execute_with_pipes(const std::vector<std::vector<std::string>>& commands);


int main() {
    std::string input;

    // loop to read usser input 
    while (true) {
        // edoras user shell prompt
        std::cout << "cssc4410% ";
        std::getline(std::cin, input);

        // EXIT if user types exit
        if (input == "exit") {
            break;
        } 

        std::vector<std::vector<std::string>> commands;
        size_t pos = 0;
        std::string token;

        // separate commands by a pipe |
        while ((pos = input.find('|')) != std::string::npos) {
            token = input.substr(0, pos);
            input.erase(0, pos + 1);
            std::vector<std::string> args;
            parse_command(token, args);
            commands.push_back(args);
        }

        // parsing the final command (after the last pipe |)
        std::vector<std::string> args;
        parse_command(input, args);
        commands.push_back(args);


        // if there's pipes execute them
        execute_with_pipes(commands);
    }

    return 0;
}

// function to execute if there is only one command
void execute_command(const std::vector<std::string>& commands) {
    const char** args = new const char*[commands.size() + 1];
    for (size_t i = 0; i < commands.size(); ++i) {
        args[i] = commands[i].c_str();
    }
    args[commands.size()] = NULL;

    // execute command
    execvp(args[0], const_cast<char* const*>(args));
    perror("execvp");
    exit(EXIT_FAILURE);
}

// function that plances the command string into a vector of arguements 
void parse_command(const std::string& command, std::vector<std::string>& args) {
    std::istringstream iss(command);
    std::string token;
    while (iss >> token) {
        args.push_back(token);
    }
}

// function to execute a single command in a separate process
void execute_single_command(const std::vector<std::string>& args) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) { 
        execute_command(args);
    } else { // Parent process
        wait(NULL);
    }
}

// execute commands with pipes
void execute_with_pipes(const std::vector<std::vector<std::string>>& commands) {
    int num_commands = commands.size();
    int pipefds[num_commands - 1][2]; 

    // make pipes
    for (int i = 0; i < num_commands - 1; ++i) {
        if (pipe(pipefds[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    // Execute commands
    for (int i = 0; i < num_commands; ++i) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) { 
            // Set up pipes using input from the prev command
            if (i != 0) {
                if (dup2(pipefds[i - 1][0], STDIN_FILENO) == -1) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }
            // output for the next command's pip input
            if (i != num_commands - 1) {
                if (dup2(pipefds[i][1], STDOUT_FILENO) == -1) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            // Close pipe file descriptors
            for (int j = 0; j < num_commands - 1; ++j) {
                close(pipefds[j][0]);
                close(pipefds[j][1]);
            }

            execute_command(commands[i]);
        }
    }

    // Close pipe file desc
    for (int i = 0; i < num_commands - 1; ++i) {
        close(pipefds[i][0]);
        close(pipefds[i][1]);
    }

    // Wait for all child processes to finish
    for (int i = 0; i < num_commands; ++i) {
        wait(NULL);
    }
}
