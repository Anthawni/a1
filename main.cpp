#include <iostream>
#include <fstream>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <memory>
#include <vector>

#define THREAD_COUNT 7
#define REPEAT_COUNT 8

sem_t FLAG;

void* thread_func(void* arg) {
    pthread_t tid = pthread_self();
    int id = *static_cast<int*>(arg);

    for (int i = 0; i < REPEAT_COUNT; i++) {
        // Even threads sleep for 2 seconds, odd threads sleep for 3 seconds
        if (id % 2 == 0) {
            sleep(2);
        } else {
            sleep(3);
        }

        // Wait for the semaphore
        sem_wait(&FLAG);

        // Write to the file
        std::ofstream file("QUOTE.txt", std::ios_base::app);
        if (!file.is_open()) {
            perror("Failed to open file");
            sem_post(&FLAG);
            pthread_exit(nullptr);
        }

		if (id % 2 == 0) {
            file << "Thread ID " << tid << ": \"Controlling complexity is the essence of computer programming.\" --Brian Kernigan" << std::endl;
        } else {
            file << "Thread ID " << tid << ": \"Computer science is no more about computers than astronomy is about telescopes.\" --Edsger Dijkstra" << std::endl;
		}

        // Print to stdout
        std::cout << "Thread " << tid << " is running" << std::endl;

        // Release the semaphore
        sem_post(&FLAG);
    }

    pthread_exit(nullptr);
}

int main() {
    // Create and initialize the file
    std::ofstream file("QUOTE.txt");
    if (!file.is_open()) {
        perror("Failed to create file");
        exit(EXIT_FAILURE);
    }

    // Write the process ID
    file << "Process ID: " << getpid() << std::endl;
    file.close();

    // Initialize the semaphore
    if (sem_init(&FLAG, 0, 1) != 0) {
        perror("Failed to initialize semaphore");
        exit(EXIT_FAILURE);
    }

    // Create threads
    std::vector<pthread_t> threads(THREAD_COUNT);
    for (int i = 0; i < THREAD_COUNT; i++) {
        int *arg = new int(i); // Allocate memory for thread argument
        if (pthread_create(&threads[i], nullptr, thread_func, arg) != 0) {
            perror("Failed to create thread");
            exit(EXIT_FAILURE);
        }
    }

    // Wait for all threads to complete
    for (auto& thread : threads) {
        pthread_join(thread, nullptr);
    }

    // Destroy the semaphore
    if (sem_destroy(&FLAG) != 0) {
        perror("Failed to destroy semaphore");
        exit(EXIT_FAILURE);
    }

    // Exit gracefully
    std::cout << "All threads have completed. Exiting..." << std::endl;
    return 0;
}