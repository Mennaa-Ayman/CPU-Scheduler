#include "SchedulingManager.hpp"
#include <algorithm>
#include <iostream>
#include <limits>

// Constructor for SchedulingManager
// Initializes the scheduler with a specific type (e.g., FCFS, Round Robin) and a time quantum (used specifically for Round Robin).
SchedulingManager::SchedulingManager(SchedulerType type, int quantum)
    : type_(type), state_(IDLE), currentTime_(0), quantum_(quantum > 0 ? quantum : 1),
      currentProcessIndex_(-1), quantumCounter_(0), lastRunningPID_(-1)
{
}

// Default destructor
SchedulingManager::~SchedulingManager() = default;

// Initializes the scheduler with a list of processes
// Resets process parameters and sorts them by arrival time and process ID.
void SchedulingManager::initialize(const std::vector<Process>& processes)
{
    allProcesses_ = processes;
    for (auto& process : allProcesses_) {
        // Ensure copy_burst_time is initialized to keep track of the original burst time
        if (process.copy_burst_time <= 0) {
            process.copy_burst_time = process.burst_time;
        }
        // Restore burst_time from copy_burst_time in case the process was modified previously
        process.burst_time = process.copy_burst_time > 0 ? process.copy_burst_time : process.burst_time;
        
        // Reset execution metrics
        process.waiting_time = 0;
        process.finish_time = 0;
        process.turn_around_time = 0;
        process.Start_time = 0;
        
        // Reset state flags
        process.IsInQueue = false;
        process.IsCompleted = false;
    }

    // Sort processes primarily by arrival time, and secondarily by process ID
    std::sort(allProcesses_.begin(), allProcesses_.end(), [](const Process& a, const Process& b) {
        if (a.arrival_time != b.arrival_time) return a.arrival_time < b.arrival_time;
        return a.process_id < b.process_id;
    });

    // Reset scheduler state variables for a fresh run
    currentTime_ = 0;
    currentProcessIndex_ = -1;
    quantumCounter_ = 0;
    lastRunningPID_ = -1;
    readyQueueIndices_ = std::queue<int>(); // Clear the ready queue
    state_ = RUNNING; // Set state to RUNNING to start execution
}

// Checks for new arrivals at the current time and adds them to the ready queue
void SchedulingManager::enqueueArrivals_()
{
    for (int i = 0; i < static_cast<int>(allProcesses_.size()); ++i) {
        auto& process = allProcesses_[i];
        // If the process hasn't completed, isn't already in the queue, and has arrived
        if (!process.IsCompleted && !process.IsInQueue && process.arrival_time <= currentTime_) {
            process.IsInQueue = true;
            // For FCFS and Round Robin, explicitly push the index to the ready queue
            if (type_ == FCFS || type_ == ROUND_ROBIN) {
                readyQueueIndices_.push(i);
            }
        }
    }
}

// Checks if all processes hold the IsCompleted flag
bool SchedulingManager::allFinished_() const
{
    for (const auto& process : allProcesses_) {
        if (!process.IsCompleted) {
            return false; // Found a process that hasn't finished
        }
    }
    return true; // All processes are completed
}

// Retrieves the index of the next process in the ready queue (Used for FCFS and RR)
int SchedulingManager::selectNextReadyIndex_() const
{
    if (readyQueueIndices_.empty()) {
        return -1; // Queue is empty
    }
    return readyQueueIndices_.front();
}

// Selects the index of the best process to run based on priority or burst time (SJF)
int SchedulingManager::selectBestReadyIndex_(bool byPriority) const
{
    int bestIndex = -1;
    for (int i = 0; i < static_cast<int>(allProcesses_.size()); ++i) {
        const auto& process = allProcesses_[i];
        // Skip completed processes, processes that haven't arrived, or those with no burst time left
        if (process.IsCompleted || process.arrival_time > currentTime_ || process.burst_time <= 0) {
            continue;
        }

        // Initialize bestIndex with the first valid process found
        if (bestIndex == -1) {
            bestIndex = i;
            continue;
        }

        const auto& best = allProcesses_[bestIndex];
        if (byPriority) {
            // Priority Scheduling logic (lower number means higher priority)
            if (process.priority < best.priority ||
                (process.priority == best.priority &&
                 (process.arrival_time < best.arrival_time ||
                  (process.arrival_time == best.arrival_time && process.process_id < best.process_id)))) {
                bestIndex = i;
            }
        } else {
            // Shortest Job First (SJF) logic (lower burst time is better)
            if (process.burst_time < best.burst_time ||
                (process.burst_time == best.burst_time &&
                 (process.arrival_time < best.arrival_time ||
                  (process.arrival_time == best.arrival_time && process.process_id < best.process_id)))) {
                bestIndex = i;
            }
        }
    }
    return bestIndex;
}

// Marks the current process as completed and updates its metrics
void SchedulingManager::finishCurrentProcess_()
{
    if (currentProcessIndex_ < 0 || currentProcessIndex_ >= static_cast<int>(allProcesses_.size())) {
        return; // Invalid index, nothing to do
    }

    auto& process = allProcesses_[currentProcessIndex_];
    if (process.burst_time == 0) {
        process.finish_time = currentTime_ + 1; // It finishes at the end of the current tick
        process.IsCompleted = true;
        process.calculate_turn_time(); // Calculate Turnaround Time = Finish Time - Arrival Time
        process.calculate_waiting_time(); // Calculate Waiting Time = Turnaround Time - Burst Time
        
        // Reset current execution tracking
        currentProcessIndex_ = -1;
        quantumCounter_ = 0;
    }
}

// Executes one time unit (tick) of the scheduling algorithm
int SchedulingManager::tick()
{
    // Do nothing if the scheduler isn't currently running
    if (state_ != RUNNING) {
        return -1;
    }

    // Add any newly arrived processes to the queue
    enqueueArrivals_();

    int runningIndex = -1;

    // Logic based on the chosen scheduling algorithm
    switch (type_) {
    case FCFS:
        // First-Come-First-Serve
        if (currentProcessIndex_ == -1 && !readyQueueIndices_.empty()) {
            currentProcessIndex_ = selectNextReadyIndex_();
            readyQueueIndices_.pop();
            // Record start time if this is the first execution
            if (currentProcessIndex_ != -1 && allProcesses_[currentProcessIndex_].Start_time == 0) {
                allProcesses_[currentProcessIndex_].Start_time = currentTime_;
            }
        }
        break;

    case ROUND_ROBIN:
        // Round Robin
        if (currentProcessIndex_ == -1 && !readyQueueIndices_.empty()) {
            currentProcessIndex_ = selectNextReadyIndex_();
            readyQueueIndices_.pop();
            quantumCounter_ = 0; // Reset quantum for the new process
            // Record start time if this is the first execution
            if (currentProcessIndex_ != -1 && allProcesses_[currentProcessIndex_].Start_time == 0) {
                allProcesses_[currentProcessIndex_].Start_time = currentTime_;
            }
        }
        break;

    case SJF_NON_PREEMPTIVE:
        // Shortest Job First (Non-Preemptive)
        if (currentProcessIndex_ == -1 || allProcesses_[currentProcessIndex_].IsCompleted) {
            currentProcessIndex_ = selectBestReadyIndex_(false); // false = by burst time
            // Record start time 
            if (currentProcessIndex_ != -1 && allProcesses_[currentProcessIndex_].Start_time == 0) {
                allProcesses_[currentProcessIndex_].Start_time = currentTime_;
            }
        }
        break;

    case SJF_PREEMPTIVE:
        // Shortest Job First (Preemptive) - Shortest Remaining Time First (SRTF)
        // Constantly check if a better process is available
        currentProcessIndex_ = selectBestReadyIndex_(false); // false = by burst time
        // Record start time 
        if (currentProcessIndex_ != -1 && allProcesses_[currentProcessIndex_].Start_time == 0) {
            allProcesses_[currentProcessIndex_].Start_time = currentTime_;
        }
        break;

    case PRIORITY_NON_PREEMPTIVE:
        // Priority Scheduling (Non-Preemptive)
        if (currentProcessIndex_ == -1 || allProcesses_[currentProcessIndex_].IsCompleted) {
            currentProcessIndex_ = selectBestReadyIndex_(true); // true = by priority
            // Record start time
            if (currentProcessIndex_ != -1 && allProcesses_[currentProcessIndex_].Start_time == 0) {
                allProcesses_[currentProcessIndex_].Start_time = currentTime_;
            }
        }
        break;

    case PRIORITY_PREEMPTIVE:
        // Priority Scheduling (Preemptive)
        // Constantly check if a higher priority process is available
        currentProcessIndex_ = selectBestReadyIndex_(true); // true = by priority
        // Record start time
        if (currentProcessIndex_ != -1 && allProcesses_[currentProcessIndex_].Start_time == 0) {
            allProcesses_[currentProcessIndex_].Start_time = currentTime_;
        }
        break;
    }

    // If a process is selected to run
    if (currentProcessIndex_ != -1) {
        auto& process = allProcesses_[currentProcessIndex_];
        process.burst_time--; // Decrement remaining burst time
        runningIndex = currentProcessIndex_;
        lastRunningPID_ = process.process_id;

        // Specific handling for Round Robin quantum
        if (type_ == ROUND_ROBIN) {
            ++quantumCounter_;
            if (process.burst_time == 0) {
                // Process finished its execution
                finishCurrentProcess_();
            } else if (quantumCounter_ >= quantum_) {
                // Quantum expired, put process back in the ready queue
                readyQueueIndices_.push(currentProcessIndex_);
                currentProcessIndex_ = -1; // Deselect process
                quantumCounter_ = 0; // Reset quantum counter
            }
        } else {
            // Processing for other algorithms
            if (process.burst_time == 0) {
                finishCurrentProcess_(); // Mark the process as complete
            }
        }
    } else {
        lastRunningPID_ = -1; // CPU is idle
    }

    currentTime_++; // Move time forward by one unit

    // Check if the entire scheduling is complete
    if (allFinished_()) {
        state_ = COMPLETED;
    }

    // Return the ID of the running process, or -1 if idle
    return runningIndex == -1 ? -1 : allProcesses_[runningIndex].process_id;
}

// Pauses the scheduling simulation
void SchedulingManager::pause()
{
    if (state_ == RUNNING) {
        state_ = PAUSED;
    }
}

// Resumes a paused scheduling simulation
void SchedulingManager::resume()
{
    if (state_ == PAUSED) {
        state_ = RUNNING;
    }
}

// Injects a new process into the simulation dynamically
void SchedulingManager::injectProcess(Process newProcess)
{
    // Injection is only permitted when the manager is paused
    if (state_ != PAUSED) {
        std::cout << "[Manager] ERROR: Can only inject when paused!\n";
        return;
    }

    // Initialize metrics for the newly injected process
    newProcess.arrival_time = currentTime_; // It arrives at the current point in time
    newProcess.copy_burst_time = newProcess.burst_time;
    newProcess.waiting_time = 0;
    newProcess.finish_time = 0;
    newProcess.turn_around_time = 0;
    newProcess.Start_time = 0;
    newProcess.IsCompleted = false;
    newProcess.IsInQueue = false;

    // Add it to the list of all processes
    allProcesses_.push_back(newProcess);
}

// Checks if the simulation has finished (all processes executed)
bool SchedulingManager::isComplete() const
{
    return state_ == COMPLETED;
}

// Returns the Process ID of the currently executing process, or -1 if idle
int SchedulingManager::getRunningProcessID() const
{
    if (currentProcessIndex_ >= 0 && currentProcessIndex_ < static_cast<int>(allProcesses_.size())) {
        return allProcesses_[currentProcessIndex_].process_id;
    }
    return -1;
}

// Resets the scheduling manager completely
void SchedulingManager::reset()
{
    state_ = IDLE;
    currentTime_ = 0;
    currentProcessIndex_ = -1;
    quantumCounter_ = 0;
    lastRunningPID_ = -1;
    allProcesses_.clear(); // Remove all processes
    readyQueueIndices_ = std::queue<int>(); // Clear the queue 
}
