#ifndef NON_PREEMPTIVE_PRIORITY_HPP
#define NON_PREEMPTIVE_PRIORITY_HPP
#include "Process.hpp"
#include <vector>
#include <queue>
#include <iostream>
#include <thread>
#include <chrono>

// lower priority number = higher priority
struct NonPreemptivePriorityCompare {
    bool operator()(const Process* a, const Process* b) {
        return a->priority > b->priority;
    }
};

// non-preemptive: once a process starts, it runs until it finishes
class NonPreemptivePriorityScheduler {
private:
    std::vector<Process> allProcesses;
    std::priority_queue<Process*, std::vector<Process*>, NonPreemptivePriorityCompare> readyQueue;
    int timer = 0;

public:
    void addProcess(const Process& p) { allProcesses.push_back(p); }
    void runLive();
    void runInstant();
    void displayResults();
};

#endif
