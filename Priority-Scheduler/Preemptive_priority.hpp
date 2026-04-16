#ifndef PREEMPTIVE_PRIORITY_HPP
#define PREEMPTIVE_PRIORITY_HPP

#include "Process.hpp"
#include <vector>
#include <queue>
#include <iostream>
#include <thread>
#include <chrono>

struct PriorityCompare {

    bool operator()(const Process* a, const Process* b) {
        return a->priority > b->priority;
    }
};

class PriorityScheduler {
private:
    std::vector<Process> allProcesses;
    std::priority_queue<Process*, std::vector<Process*>, PriorityCompare> readyQueue;
    int currentTime = 0;

public:
    void addProcess(const Process& p);
    void runLive();
    void runInstant();
    void displayResults();
};

#endif