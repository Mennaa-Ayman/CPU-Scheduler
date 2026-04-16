#include "SchedulingManager.hpp"
#include <algorithm>
#include <iostream>
#include <limits>

SchedulingManager::SchedulingManager(SchedulerType type, int quantum)
    : type_(type), state_(IDLE), currentTime_(0), quantum_(quantum > 0 ? quantum : 1),
      currentProcessIndex_(-1), quantumCounter_(0), lastRunningPID_(-1)
{
}

SchedulingManager::~SchedulingManager() = default;

void SchedulingManager::initialize(const std::vector<Process>& processes)
{
    allProcesses_ = processes;
    for (auto& process : allProcesses_) {
        if (process.copy_burst_time <= 0) {
            process.copy_burst_time = process.burst_time;
        }
        process.burst_time = process.copy_burst_time > 0 ? process.copy_burst_time : process.burst_time;
        process.waiting_time = 0;
        process.finish_time = 0;
        process.turn_around_time = 0;
        process.Start_time = 0;
        process.IsInQueue = false;
        process.IsCompleted = false;
    }

    std::sort(allProcesses_.begin(), allProcesses_.end(), [](const Process& a, const Process& b) {
        if (a.arrival_time != b.arrival_time) return a.arrival_time < b.arrival_time;
        return a.process_id < b.process_id;
    });

    currentTime_ = 0;
    currentProcessIndex_ = -1;
    quantumCounter_ = 0;
    lastRunningPID_ = -1;
    readyQueueIndices_ = std::queue<int>();
    state_ = RUNNING;
}

void SchedulingManager::enqueueArrivals_()
{
    for (int i = 0; i < static_cast<int>(allProcesses_.size()); ++i) {
        auto& process = allProcesses_[i];
        if (!process.IsCompleted && !process.IsInQueue && process.arrival_time <= currentTime_) {
            process.IsInQueue = true;
            if (type_ == FCFS || type_ == ROUND_ROBIN) {
                readyQueueIndices_.push(i);
            }
        }
    }
}

bool SchedulingManager::allFinished_() const
{
    for (const auto& process : allProcesses_) {
        if (!process.IsCompleted) {
            return false;
        }
    }
    return true;
}

int SchedulingManager::selectNextReadyIndex_() const
{
    if (readyQueueIndices_.empty()) {
        return -1;
    }
    return readyQueueIndices_.front();
}

int SchedulingManager::selectBestReadyIndex_(bool byPriority) const
{
    int bestIndex = -1;
    for (int i = 0; i < static_cast<int>(allProcesses_.size()); ++i) {
        const auto& process = allProcesses_[i];
        if (process.IsCompleted || process.arrival_time > currentTime_ || process.burst_time <= 0) {
            continue;
        }

        if (bestIndex == -1) {
            bestIndex = i;
            continue;
        }

        const auto& best = allProcesses_[bestIndex];
        if (byPriority) {
            if (process.priority < best.priority ||
                (process.priority == best.priority &&
                 (process.arrival_time < best.arrival_time ||
                  (process.arrival_time == best.arrival_time && process.process_id < best.process_id)))) {
                bestIndex = i;
            }
        } else {
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

void SchedulingManager::finishCurrentProcess_()
{
    if (currentProcessIndex_ < 0 || currentProcessIndex_ >= static_cast<int>(allProcesses_.size())) {
        return;
    }

    auto& process = allProcesses_[currentProcessIndex_];
    if (process.burst_time == 0) {
        process.finish_time = currentTime_ + 1;
        process.IsCompleted = true;
        process.calculate_turn_time();
        process.calculate_waiting_time();
        currentProcessIndex_ = -1;
        quantumCounter_ = 0;
    }
}

int SchedulingManager::tick()
{
    if (state_ != RUNNING) {
        return -1;
    }

    enqueueArrivals_();

    int runningIndex = -1;

    switch (type_) {
    case FCFS:
        if (currentProcessIndex_ == -1 && !readyQueueIndices_.empty()) {
            currentProcessIndex_ = selectNextReadyIndex_();
            readyQueueIndices_.pop();
            if (currentProcessIndex_ != -1 && allProcesses_[currentProcessIndex_].Start_time == 0) {
                allProcesses_[currentProcessIndex_].Start_time = currentTime_;
            }
        }
        break;

    case ROUND_ROBIN:
        if (currentProcessIndex_ == -1 && !readyQueueIndices_.empty()) {
            currentProcessIndex_ = selectNextReadyIndex_();
            readyQueueIndices_.pop();
            quantumCounter_ = 0;
            if (currentProcessIndex_ != -1 && allProcesses_[currentProcessIndex_].Start_time == 0) {
                allProcesses_[currentProcessIndex_].Start_time = currentTime_;
            }
        }
        break;

    case SJF_NON_PREEMPTIVE:
        if (currentProcessIndex_ == -1 || allProcesses_[currentProcessIndex_].IsCompleted) {
            currentProcessIndex_ = selectBestReadyIndex_(false);
            if (currentProcessIndex_ != -1 && allProcesses_[currentProcessIndex_].Start_time == 0) {
                allProcesses_[currentProcessIndex_].Start_time = currentTime_;
            }
        }
        break;

    case SJF_PREEMPTIVE:
        currentProcessIndex_ = selectBestReadyIndex_(false);
        if (currentProcessIndex_ != -1 && allProcesses_[currentProcessIndex_].Start_time == 0) {
            allProcesses_[currentProcessIndex_].Start_time = currentTime_;
        }
        break;

    case PRIORITY_NON_PREEMPTIVE:
        if (currentProcessIndex_ == -1 || allProcesses_[currentProcessIndex_].IsCompleted) {
            currentProcessIndex_ = selectBestReadyIndex_(true);
            if (currentProcessIndex_ != -1 && allProcesses_[currentProcessIndex_].Start_time == 0) {
                allProcesses_[currentProcessIndex_].Start_time = currentTime_;
            }
        }
        break;

    case PRIORITY_PREEMPTIVE:
        currentProcessIndex_ = selectBestReadyIndex_(true);
        if (currentProcessIndex_ != -1 && allProcesses_[currentProcessIndex_].Start_time == 0) {
            allProcesses_[currentProcessIndex_].Start_time = currentTime_;
        }
        break;
    }

    if (currentProcessIndex_ != -1) {
        auto& process = allProcesses_[currentProcessIndex_];
        process.burst_time--;
        runningIndex = currentProcessIndex_;
        lastRunningPID_ = process.process_id;

        if (type_ == ROUND_ROBIN) {
            ++quantumCounter_;
            if (process.burst_time == 0) {
                finishCurrentProcess_();
            } else if (quantumCounter_ >= quantum_) {
                readyQueueIndices_.push(currentProcessIndex_);
                currentProcessIndex_ = -1;
                quantumCounter_ = 0;
            }
        } else {
            if (process.burst_time == 0) {
                finishCurrentProcess_();
            }
        }
    } else {
        lastRunningPID_ = -1;
    }

    currentTime_++;

    if (allFinished_()) {
        state_ = COMPLETED;
    }

    return runningIndex == -1 ? -1 : allProcesses_[runningIndex].process_id;
}

void SchedulingManager::pause()
{
    if (state_ == RUNNING) {
        state_ = PAUSED;
    }
}

void SchedulingManager::resume()
{
    if (state_ == PAUSED) {
        state_ = RUNNING;
    }
}

void SchedulingManager::injectProcess(Process newProcess)
{
    if (state_ != PAUSED) {
        std::cout << "[Manager] ERROR: Can only inject when paused!\n";
        return;
    }

    newProcess.arrival_time = currentTime_;
    newProcess.copy_burst_time = newProcess.burst_time;
    newProcess.waiting_time = 0;
    newProcess.finish_time = 0;
    newProcess.turn_around_time = 0;
    newProcess.Start_time = 0;
    newProcess.IsCompleted = false;
    newProcess.IsInQueue = false;

    allProcesses_.push_back(newProcess);
}

bool SchedulingManager::isComplete() const
{
    return state_ == COMPLETED;
}

int SchedulingManager::getRunningProcessID() const
{
    if (currentProcessIndex_ >= 0 && currentProcessIndex_ < static_cast<int>(allProcesses_.size())) {
        return allProcesses_[currentProcessIndex_].process_id;
    }
    return -1;
}

void SchedulingManager::reset()
{
    state_ = IDLE;
    currentTime_ = 0;
    currentProcessIndex_ = -1;
    quantumCounter_ = 0;
    lastRunningPID_ = -1;
    allProcesses_.clear();
    readyQueueIndices_ = std::queue<int>();
}
