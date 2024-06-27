#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <climits>

class MemoryBlock {
public:
    int size;
    int process_id;
    MemoryBlock* next;

    MemoryBlock(int s, int pid = -1) : size(s), process_id(pid), next(nullptr) {}
};

class MemoryManager {
private:
    MemoryBlock* head;
    std::string technique;

    void merge_free_blocks() {
        MemoryBlock* current = head;
        while (current && current->next) {
            if (current->process_id == -1 && current->next->process_id == -1) {
                current->size += current->next->size;
                MemoryBlock* to_delete = current->next;
                current->next = current->next->next;
                delete to_delete;
            } else {
                current = current->next;
            }
        }
    }

public:
    MemoryManager(const std::string& tech) : technique(tech) {
        head = new MemoryBlock(128);  // 256 KB divided into 2 KB units (128 units)
    }

    ~MemoryManager() {
        while (head) {
            MemoryBlock* temp = head;
            head = head->next;
            delete temp;
        }
    }

    int allocate_mem(int process_id, int num_units) {
        if (technique == "first_fit")
            return first_fit_allocate(process_id, num_units);
        else if (technique == "best_fit")
            return best_fit_allocate(process_id, num_units);
        return -1;
    }

    int first_fit_allocate(int process_id, int num_units) {
        MemoryBlock* current = head;
        int nodes_traversed = 0;

        while (current) {
            nodes_traversed++;
            if (current->process_id == -1 && current->size >= num_units) {
                if (current->size > num_units) {
                    MemoryBlock* new_block = new MemoryBlock(current->size - num_units);
                    new_block->next = current->next;
                    current->size = num_units;
                    current->next = new_block;
                }
                current->process_id = process_id;
                return nodes_traversed;
            }
            current = current->next;
        }

        return -1;
    }

    int best_fit_allocate(int process_id, int num_units) {
        MemoryBlock* current = head;
        MemoryBlock* best_block = nullptr;
        int best_block_size = INT_MAX;
        int nodes_traversed = 0;

        while (current) {
            nodes_traversed++;
            if (current->process_id == -1 && current->size >= num_units && current->size < best_block_size) {
                best_block = current;
                best_block_size = current->size;
            }
            current = current->next;
        }

        if (best_block) {
            if (best_block->size > num_units) {
                MemoryBlock* new_block = new MemoryBlock(best_block->size - num_units);
                new_block->next = best_block->next;
                best_block->size = num_units;
                best_block->next = new_block;
            }
            best_block->process_id = process_id;
            return nodes_traversed;
        }

        return -1;
    }

    int deallocate_mem(int process_id) {
        MemoryBlock* current = head;

        while (current) {
            if (current->process_id == process_id) {
                current->process_id = -1;
                merge_free_blocks();
                return 1;
            }
            current = current->next;
        }

        return -1;
    }

    int fragment_count() {
        MemoryBlock* current = head;
        int fragments = 0;

        while (current) {
            if (current->process_id == -1 && current->size <= 2) {
                fragments++;
            }
            current = current->next;
        }

        return fragments;
    }
};

std::vector<std::pair<std::string, std::pair<int, int>>> generate_requests(int num_requests) {
    std::vector<std::pair<std::string, std::pair<int, int>>> requests;
    srand(time(0));

    for (int i = 0; i < num_requests; ++i) {
        int process_id = rand() % 10000 + 1;
        if (rand() % 2 == 0) {
            int num_units = rand() % 8 + 3;  // 3 to 10 units
            requests.push_back({"alloc", {process_id, num_units}});
        } else {
            requests.push_back({"dealloc", {process_id, 0}});
        }
    }

    return requests;
}

class Statistics {
private:
    int external_fragments;
    int total_allocations;
    int total_nodes_traversed;
    int failed_allocations;

public:
    Statistics() : external_fragments(0), total_allocations(0), total_nodes_traversed(0), failed_allocations(0) {}

    void update(int fragment_count, int nodes_traversed, bool allocation_successful) {
        external_fragments += fragment_count;
        total_allocations++;
        total_nodes_traversed += nodes_traversed;
        if (!allocation_successful) {
            failed_allocations++;
        }
    }

    void report() {
        double avg_fragments = static_cast<double>(external_fragments) / total_allocations;
        double avg_nodes_traversed = static_cast<double>(total_nodes_traversed) / total_allocations;
        double allocation_failure_percentage = (static_cast<double>(failed_allocations) / total_allocations) * 100;
        std::cout << "Average number of external fragments: " << avg_fragments << "\n";
        std::cout << "Average allocation time (nodes traversed): " << avg_nodes_traversed << "\n";
        std::cout << "Allocation failure percentage: " << allocation_failure_percentage << "%\n";
    }
};

void run_simulation(int num_requests) {
    MemoryManager first_fit_manager("first_fit");
    MemoryManager best_fit_manager("best_fit");
    Statistics stats_first_fit;
    Statistics stats_best_fit;

    std::vector<std::pair<std::string, std::pair<int, int>>> requests = generate_requests(num_requests);

    for (const auto& request : requests) {
        if (request.first == "alloc") {
            int process_id = request.second.first;
            int num_units = request.second.second;

            int nodes_traversed = first_fit_manager.allocate_mem(process_id, num_units);
            stats_first_fit.update(first_fit_manager.fragment_count(), nodes_traversed, nodes_traversed != -1);

            nodes_traversed = best_fit_manager.allocate_mem(process_id, num_units);
            stats_best_fit.update(best_fit_manager.fragment_count(), nodes_traversed, nodes_traversed != -1);
        } else if (request.first == "dealloc") {
            int process_id = request.second.first;
            first_fit_manager.deallocate_mem(process_id);
            best_fit_manager.deallocate_mem(process_id);
        }
    }

    std::cout << "First Fit Statistics:\n";
    stats_first_fit.report();
    std::cout << "\nBest Fit Statistics:\n";
    stats_best_fit.report();
}

int main() {
    run_simulation(10000);
    return 0;
}
