#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

void run_executable(const std::string& command, const std::string& argument) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return;
    }

    if (pid == 0) { // Child process
        if (argument.empty()) {
            execl(command.c_str(), command.c_str(), NULL);
        } else {
            execl(command.c_str(), command.c_str(), argument.c_str(), NULL);
        }
        perror("execl"); // Only reached if execl fails
        exit(EXIT_FAILURE);
    } else { // Parent process
        waitpid(pid, NULL, 0);
    }
}

void handle_piped_commands(const std::vector<std::string>& commands) {
    size_t num_commands = commands.size();
    int pipefds[2 * (num_commands - 1)];
    
    for (size_t i = 0; i < num_commands - 1; ++i) {
        if (pipe(pipefds + i*2) < 0) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    for (size_t i = 0; i < num_commands; ++i) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) { // Child process
            if (i != 0) {
                if (dup2(pipefds[(i-1)*2], STDIN_FILENO) < 0) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            if (i != num_commands - 1) {
                if (dup2(pipefds[i*2 + 1], STDOUT_FILENO) < 0) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            for (size_t j = 0; j < 2 * (num_commands - 1); ++j) {
                close(pipefds[j]);
            }

            std::istringstream iss(commands[i]);
            std::string executable;
            std::vector<std::string> args;
            std::string arg;

            iss >> executable;
            while (iss >> arg) {
                args.push_back(arg);
            }

            std::vector<const char*> c_args;
            c_args.push_back(executable.c_str());
            for (const auto& a : args) {
                c_args.push_back(a.c_str());
            }
            c_args.push_back(NULL);

            execvp(executable.c_str(), const_cast<char* const*>(c_args.data()));
            perror("execvp"); // Only reached if execvp fails
            exit(EXIT_FAILURE);
        }
    }

    for (size_t i = 0; i < 2 * (num_commands - 1); ++i) {
        close(pipefds[i]);
    }

    for (size_t i = 0; i < num_commands; ++i) {
        wait(NULL);
    }
}

int main() {
    std::string username = "cssc0000"; // Replace this with the actual class account username
    std::string input;

    while (true) {
        std::cout << username << "% ";
        std::getline(std::cin, input);

        if (input.empty()) {
            continue;
        }

        if (input == "exit") {
            break;
        }

        std::istringstream iss(input);
        std::vector<std::string> commands;
        std::string token;

        while (std::getline(iss, token, '|')) {
            commands.push_back(token);
        }

        if (commands.size() > 1) {
            handle_piped_commands(commands);
        } else {
            std::istringstream single_iss(input);
            std::string command, argument;
            single_iss >> command;
            single_iss >> argument;

            if (access(command.c_str(), X_OK) == 0) {
                run_executable(command, argument);
            } else {
                std::cerr << "Error: " << command << " is not a valid executable file." << std::endl;
            }
        }
    }

    return 0;
}
