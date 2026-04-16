#include "Preemptive_priority.hpp"
#include <iomanip>

void PriorityScheduler::addProcess(const Process& p) {
    allProcesses.push_back(p);
}

static void tick(std::vector<Process>& allProcesses,
                 std::priority_queue<Process*, std::vector<Process*>, PriorityCompare>& readyQueue,
                 Process*& currentProcess,
                 int& completed,
                 int currentTime)
{
    for (auto& p : allProcesses) {
        if (p.arrival_time == currentTime && !p.IsInQueue) {
            readyQueue.push(&p);
            p.IsInQueue = true;
        }
    }

    if (!readyQueue.empty()) {
        if (currentProcess == nullptr || readyQueue.top()->priority < currentProcess->priority) {
            if (currentProcess != nullptr && currentProcess->burst_time > 0) {
                readyQueue.push(currentProcess);
            }
            currentProcess = readyQueue.top();
            readyQueue.pop();
        }
    }

    if (currentProcess != nullptr) {
        std::cout << "[Time " << currentTime << "] Running Process ID: " << currentProcess->process_id
                  << " (Remaining: " << currentProcess->burst_time << ")\n";

        currentProcess->burst_time--;

        if (currentProcess->burst_time == 0) {
            currentProcess->finish_time = currentTime + 1;
            currentProcess->calculate_turn_time();
            currentProcess->calculate_waiting_time();
            currentProcess->IsCompleted = true;
            completed++;
            currentProcess = nullptr;
        }
    } else {
        std::cout << "[Time " << currentTime << "] CPU Idle\n";
    }
}

void PriorityScheduler::runLive() {
    int completed = 0;
    int n = (int)allProcesses.size();
    Process* current = nullptr;

    while (completed < n) {
        tick(allProcesses, readyQueue, current, completed, currentTime);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        currentTime++;
    }
    displayResults();
}

void PriorityScheduler::runInstant() {
    int completed = 0;
    int n = (int)allProcesses.size();
    Process* current = nullptr;

    while (completed < n) {
        tick(allProcesses, readyQueue, current, completed, currentTime);
        currentTime++;
    }
    displayResults();
}

void PriorityScheduler::displayResults() {
    double totalWait = 0, totalTAT = 0;
    std::cout << "\n--- Statistics ---\n";
    for (const auto& p : allProcesses) {
        std::cout << "ID: " << p.process_id
                  << " | Wait: " << p.waiting_time
                  << " | TAT: " << p.turn_around_time << "\n";
        totalWait += p.waiting_time;
        totalTAT += p.turn_around_time;
    }
    std::cout << "Avg Waiting Time: " << totalWait / allProcesses.size() << "\n";
    std::cout << "Avg Turnaround Time: " << totalTAT / allProcesses.size() << "\n";
}

//
// #include "Priority-Scheduler/Preemptive_priority.hpp"
//
//
// int main() {
//     PriorityScheduler scheduler;
//
//     scheduler.addProcess(Process(1, 0, 8, 3));
//     scheduler.addProcess(Process(2, 1, 4, 1));
//     scheduler.addProcess(Process(3, 2, 5, 2));
//     scheduler.addProcess(Process(4, 3, 2, 1));
//
//
//
//     scheduler.runInstant();
//
//     return 0;
// }
//


