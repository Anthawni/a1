/* 
main.cpp

CS 480 Operating Systems
Programming Assignment 3 
Team Members 
Name: Jeremiah Dobes
RedID: 826788151
Edoras: cssc4429

Name: Anthony Ngo
RedID: 825252396
Edoras: cssc4410       Use this one for testing
*/


#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
using namespace std;

// Constants
const int MEMORY_SIZE = 256;  // 256 KB
const int UNIT_SIZE = 2;      // Each unit is 2 KB
const int NUM_UNITS = MEMORY_SIZE / UNIT_SIZE;

// Memory Node structure for linked list
struct MemoryNode {
    int process_id;
    int size;  // in units
    MemoryNode* next;

    MemoryNode(int pid, int sz) : process_id(pid), size(sz), next(nullptr) {}
};

// Memory class to manage memory allocation using linked list
class MemoryManager {
private:
    MemoryNode* head;

public:
    MemoryManager() : head(nullptr) {}

    // Function to allocate memory using First Fit
    int allocate_first_fit(int process_id, int num_units) {
        MemoryNode* prev = nullptr;
        MemoryNode* curr = head;

        int nodes_traversed = 0;

        while (curr) {
            nodes_traversed++;
            if (curr->size >= num_units) {
                // Allocate memory here
                if (curr->size == num_units) {
                    // Exact fit
                    curr->process_id = process_id;
                } else if (curr->size > num_units) {
                    // Allocate part of the block
                    curr->size -= num_units;
                    MemoryNode* new_block = new MemoryNode(process_id, num_units);
                    new_block->next = curr->next;
                    curr->next = new_block;
                }
                
                return nodes_traversed;
            }
            prev = curr;
            curr = curr->next;
        }
        return -1; // Allocation failed
    }

    // Function to allocate memory using Best Fit
    int allocate_best_fit(int process_id, int num_units) {
        MemoryNode* prev = nullptr;
        MemoryNode* best_fit_node = nullptr;
        MemoryNode* curr = head;

        int best_fit_size = MEMORY_SIZE + 1; // Initialize with a large size

        int nodes_traversed = 0;

        while (curr) {
            nodes_traversed++;
            if (curr->size >= num_units && curr->size < best_fit_size) {
                best_fit_node = curr;
                best_fit_size = curr->size;
            }
            curr = curr->next;
        }

        if (best_fit_node) {
            // Allocate memory here
            if (best_fit_node->size == num_units) {
                // Exact fit
                best_fit_node->process_id = process_id;
            } else {
                // Allocate part of the block
                best_fit_node->size -= num_units;
                MemoryNode* new_block = new MemoryNode(process_id, num_units);
                new_block->next = best_fit_node->next;
                best_fit_node->next = new_block;
            }
            
            return nodes_traversed;
        }

        return -1; // Allocation fail
    }

    // Function to deallocate memory
    int deallocate_mem(int process_id) {
        MemoryNode* prev = nullptr;
        MemoryNode* curr = head;

        while (curr) {
            if (curr->process_id == process_id) {
                // Mark the block as free
                curr->process_id = -1;

                // Merge free blocks if adjacent
                if (prev && prev->process_id == -1) {
                    prev->size += curr->size;
                    prev->next = curr->next;
                    delete curr;
                    curr = prev;
                }
                if (curr->next && curr->next->process_id == -1) {
                    curr->size += curr->next->size;
                    MemoryNode* temp = curr->next;
                    curr->next = temp->next;
                    delete temp;
                }
                return 1; // For Deallocation successful
            }
            prev = curr;
            curr = curr->next;
        }
        return -1; 
    }

    // Function for counting number of frag
    int fragment_count() {
        int count = 0;
        MemoryNode* curr = head;

        while (curr) {
            if (curr->size == 1 || curr->size == 2)
                count++;
            curr = curr->next;
        }
        return count;
    }

    // Function to initialize memory with NUM_UNITS blocks
    void initialize_memory() {
        head = new MemoryNode(-1, NUM_UNITS);
    }

    // Function to print memory state 
    void print_memory() {
        MemoryNode* curr = head;
        while (curr) {
            cout << "Process ID: " << curr->process_id << ", Size: " << curr->size << endl;
            curr = curr->next;
        }
        cout << endl;
    }
};

// function for generating a random number between min and max 
int generate_random(int min, int max) {
    return rand() % (max - min + 1) + min;
}

int main() {
    srand(time(nullptr));

    // Initialize memory manager for First Fit and Best Fit
    MemoryManager first_fit_manager;
    MemoryManager best_fit_manager;
    first_fit_manager.initialize_memory();
    best_fit_manager.initialize_memory();

    // establish parameters
    const int NUM_REQUESTS = 10000;
    int denied_first_fit = 0;
    int denied_best_fit = 0;
    double total_external_frag_first_fit = 0.0;
    double total_external_frag_best_fit = 0.0;
    double total_nodes_traversed_first_fit = 0.0;
    double total_nodes_traversed_best_fit = 0.0;

    // create requests
    for (int i = 0; i < NUM_REQUESTS; ++i) {
        // Generate rand process ID and # of units to request (between 3 - 10)
        int process_id = i + 1;
        int num_units = generate_random(3, 10);

        // alloc in First Fit
        int nodes_traversed_first_fit = first_fit_manager.allocate_first_fit(process_id, num_units);
        if (nodes_traversed_first_fit == -1)
            denied_first_fit++;
        else
            total_nodes_traversed_first_fit += nodes_traversed_first_fit;

        // alloc in Best Fit
        int nodes_traversed_best_fit = best_fit_manager.allocate_best_fit(process_id, num_units);
        if (nodes_traversed_best_fit == -1)
            denied_best_fit++;
        else
            total_nodes_traversed_best_fit += nodes_traversed_best_fit;

        // Update first fit  and best fit frag
        total_external_frag_first_fit += first_fit_manager.fragment_count();
        total_external_frag_best_fit += best_fit_manager.fragment_count();
    }

    // Calculate the averages
    double avg_external_frag_first_fit = total_external_frag_first_fit / NUM_REQUESTS;
    double avg_external_frag_best_fit = total_external_frag_best_fit / NUM_REQUESTS;
    double avg_nodes_traversed_first_fit = total_nodes_traversed_first_fit / NUM_REQUESTS;
    double avg_nodes_traversed_best_fit = total_nodes_traversed_best_fit / NUM_REQUESTS;
    double percent_denied_first_fit = (static_cast<double>(denied_first_fit) / NUM_REQUESTS) * 100;
    double percent_denied_best_fit = (static_cast<double>(denied_best_fit) / NUM_REQUESTS) * 100;

    // Output with the appropriate layout
    cout << "End of First Fit Allocation" << endl;
    cout << "Average External Fragments Each Request: " << avg_external_frag_first_fit << endl;
    cout << "Average Nodes Traversed Each Allocation: " << avg_nodes_traversed_first_fit << endl;
    cout << "Percentage Allocation Requests Denied Overall: " << percent_denied_first_fit << "%" << endl;

    cout << "End of Best Fit Allocation" << endl;
    cout << "Average External Fragments Each Request: " << avg_external_frag_best_fit << endl;
    cout << "Average Nodes Traversed Each Allocation: " << avg_nodes_traversed_best_fit << endl;
    cout << "Percentage Allocation Requests Denied Overall: " << percent_denied_best_fit << "%" << endl;

    return 0;
}
