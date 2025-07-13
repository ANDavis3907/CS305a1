#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>

// Memory Configuration Constants
const int TOTAL_MEMORY_SIZE_KB = 20;
const int PAGE_SIZE_KB = 1;
const int NUM_PAGES = TOTAL_MEMORY_SIZE_KB / PAGE_SIZE_KB;

// Job State Enumeration
enum JobState {
    JOB_STATE_END,
    JOB_STATE_SLEEP,
    JOB_STATE_RUNNING,
    JOB_STATE_WAITING,
    JOB_STATE_COMPLETED_AND_DEALLOCATED
};

// Converts JobState enum to string
std::string jobStateToString(JobState state) {
    switch (state) {
        case JOB_STATE_END: return "End";
        case JOB_STATE_SLEEP: return "Sleep";
        case JOB_STATE_RUNNING: return "Running";
        case JOB_STATE_WAITING: return "Waiting";
        case JOB_STATE_COMPLETED_AND_DEALLOCATED: return "Completed (Deallocated)";
        default: return "Unknown";
    }
}

// Job Structure
struct Job {
    int id;
    int startTime;
    int requiredSize;
    int executionInterval;
    std::string initialState;

    JobState currentState;
    int remainingExecutionTime;
    int timeInMemory;
    int allocatedStartPage;
    int allocatedEndPage;
    bool isAllocated;
    bool hasCompleted;
    bool hasDeallocated;

    // Job constructor
    Job(int id, int startTime, int requiredSize, int executionInterval, const std::string& initialState)
        : id(id), startTime(startTime), requiredSize(requiredSize), executionInterval(executionInterval),
          initialState(initialState), currentState(JOB_STATE_WAITING), remainingExecutionTime(executionInterval),
          timeInMemory(0), allocatedStartPage(-1), allocatedEndPage(-1), isAllocated(false),
          hasCompleted(false), hasDeallocated(false) {}
};

// Global memory representation (0 = Free, Job ID = Occupied)
std::vector<int> memory(NUM_PAGES, 0);

// Prints the current state of memory
void printMemoryState() {
    std::cout << "Memory State (0 = Free, Job ID = Occupied):" << std::endl;
    for (int i = 0; i < NUM_PAGES; ++i) {
        std::cout << "[" << (i < 10 ? "0" : "") << i << ":" << (memory[i] == 0 ? "Free" : std::to_string(memory[i])) << "] ";
        if ((i + 1) % 5 == 0) {
            std::cout << std::endl;
        }
    }
    if (NUM_PAGES % 5 != 0) {
        std::cout << std::endl;
    }
    std::cout << "------------------------------------------" << std::endl;
}

// Loads jobs from a text file
std::vector<Job> loadJobsFromFile(const std::string& filename) {
    std::vector<Job> jobs;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return jobs;
    }

    std::string line;
    while (std::getline(file, line)) {
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
        
        if (line.empty()) {
            continue;
        }

        std::stringstream ss(line);
        std::string segment;
        std::vector<std::string> segments;

        while (std::getline(ss, segment, '\t')) {
            segments.push_back(segment);
        }

        if (segments.size() == 5) {
            try {
                int id = std::stoi(segments[0]);
                int startTime = std::stoi(segments[1]);
                int requiredSize = std::stoi(segments[2]);
                int executionInterval = std::stoi(segments[3]);
                std::string initialState = segments[4];

                jobs.emplace_back(id, startTime, requiredSize, executionInterval, initialState);
            } catch (const std::invalid_argument& e) {
                std::cerr << "Error parsing line (invalid argument): " << line << " - " << e.what() << std::endl;
            } catch (const std::out_of_range& e) {
                std::cerr << "Error parsing line (out of range): " << line << " - " << e.what() << std::endl;
            }
        } else {
            std::cerr << "Warning: Skipping malformed line (expected 5 fields): " << line << std::endl;
        }
    }

    file.close();
    return jobs;
}

// Main function for simulation setup and job loading demonstration
int main() {
    std::cout << "Memory Simulation Setup:" << std::endl;
    std::cout << "Total Memory Size: " << TOTAL_MEMORY_SIZE_KB << " KB" << std::endl;
    std::cout << "Page Size: " << PAGE_SIZE_KB << " KB" << std::endl;
    std::cout << "Number of Pages: " << NUM_PAGES << std::endl;
    std::cout << std::endl;

    printMemoryState();

    std::vector<Job> jobs1 = loadJobsFromFile("table1.txt");
    std::cout << "\n--- Jobs Loaded from table1.txt ---" << std::endl;
    for (const auto& job : jobs1) {
        std::cout << "Job ID: " << job.id
                  << ", Start Time: " << job.startTime
                  << ", Required Size: " << job.requiredSize
                  << ", Execution Interval: " << job.executionInterval
                  << ", Initial State: " << job.initialState
                  << ", Current State: " << jobStateToString(job.currentState)
                  << std::endl;
    }
    std::cout << "Total jobs loaded from table1.txt: " << jobs1.size() << std::endl;

    std::vector<Job> jobs2 = loadJobsFromFile("table2.txt");
    std::cout << "\n--- Jobs Loaded from table2.txt ---" << std::endl;
    for (const auto& job : jobs2) {
        std::cout << "Job ID: " << job.id
                  << ", Start Time: " << job.startTime
                  << ", Required Size: " << job.requiredSize
                  << ", Execution Interval: " << job.executionInterval
                  << ", Initial State: " << job.initialState
                  << ", Current State: " << jobStateToString(job.currentState)
                  << std::endl;
    }
    std::cout << "Total jobs loaded from table2.txt: " << jobs2.size() << std::endl;

    return 0;
}
