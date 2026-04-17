#ifndef SCHEDULING_MANAGER_HPP
#define SCHEDULING_MANAGER_HPP

#include "Process/Process.hpp"
#include <vector>
#include <queue>

/**
 * SchedulingManager - Unified interface for CPU scheduling with pause/resume/inject
 * 
 * Provides:
 * - Live simulation tick-by-tick execution
 * - Pause/Resume capability with state preservation
 * - Dynamic process injection at any paused moment
 * - Algorithm-agnostic interface (works with all schedulers)
 */
class SchedulingManager {
public:
    enum SchedulerType {
        FCFS,
        SJF_NON_PREEMPTIVE,
        SJF_PREEMPTIVE,
        PRIORITY_NON_PREEMPTIVE,
        PRIORITY_PREEMPTIVE,
        ROUND_ROBIN
    };

    enum SimulationState {
        IDLE,
        RUNNING,
        PAUSED,
        COMPLETED
    };

    SchedulingManager(SchedulerType type, int quantum = 1);
    ~SchedulingManager();

    // ========== Simulation Control ==========
    void initialize(const std::vector<Process>& processes);
    int tick();
    void pause();
    void resume();
    void injectProcess(Process newProcess);
    bool isComplete() const;

    // ========== State Queries ==========
    SimulationState getState() const { return state_; }
    int getCurrentTime() const { return currentTime_; }
    int getRunningProcessID() const;
    std::vector<Process> getCurrentProcesses() const { return allProcesses_; }
    
    // ========== Utility ==========
    void reset();

private:
    SchedulerType type_;
    SimulationState state_;
    int currentTime_;
    int quantum_;
    int currentProcessIndex_;
    int quantumCounter_;
    int lastRunningPID_;
    std::vector<Process> allProcesses_;

    // Ready queue state
    std::queue<int> readyQueueIndices_;

    // Helper methods
    void enqueueArrivals_();
    int selectNextReadyIndex_() const;
    int selectBestReadyIndex_(bool byPriority) const;
    bool allFinished_() const;
    void finishCurrentProcess_();
};

#endif // SCHEDULING_MANAGER_HPP
