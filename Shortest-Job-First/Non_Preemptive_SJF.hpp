#pragma once
#include "../Process/Process.hpp"
#include <vector>

// ─────────────────────────────────────────────
//  Process Control Block for SJF
// ─────────────────────────────────────────────


// ─────────────────────────────────────────────
//  SJF Scheduler (Non-Preemptive)
//  Rule: shortest burst time runs first.
//  Ties broken by arrival time, then by PID.
// ─────────────────────────────────────────────
class SJFScheduler {
public:
    void addProcess(int pid, int arrivalTime, int burstTime);

    void schedule();

    const std::vector<Process>& getProcesses() const;
    double getAvgWaiting()      const;
    double getAvgTurnaround()   const;
    double getAvgResponse()     const;
    double getCpuUtilization()  const;

    void printResults() const;
    void reset();

private:
    std::vector<Process> processes_;
    std::vector<Process> results_;
    bool scheduled_ = false;

    // Returns index of shortest ready job at currentTime, or -1 if none ready
    int selectNext(const std::vector<Process>& remaining, int currentTime) const;

    void computeStats();
};