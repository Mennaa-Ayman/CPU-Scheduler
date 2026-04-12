#ifndef PREEMPTIVE_PRIORITY_HPP
#define PREEMPTIVE_PRIORITY_HPP

#include <string>
#include <vector>
#include <queue>
#include <iostream>
#include <thread>
#include <chrono>

struct Process {
    std::string id;
    int arrivalTime;
    int burstTime;
    int priority;
    int remainingTime;
    int finishTime     = 0;
    int turnaroundTime = 0;
    int waitingTime    = 0;

    Process(std::string id, int arrival, int burst, int priority)
        : id(id), arrivalTime(arrival), burstTime(burst),
          priority(priority), remainingTime(burst) {}
};

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