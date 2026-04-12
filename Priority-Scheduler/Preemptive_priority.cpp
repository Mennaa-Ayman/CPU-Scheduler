#include "Preemptive_priority.hpp"
#include <iomanip>

void PriorityScheduler::addProcess(const Process& p) {
    allProcesses.push_back(p);
}

// ── shared logic ────────────────────────────────────────────────
static void tick(std::vector<Process>& allProcesses,
                 std::priority_queue<Process*, std::vector<Process*>, PriorityCompare>& readyQueue,
                 Process*& currentProcess,
                 int& completed,
                 int currentTime)
{
    // 1. push newly arrived processes
    for (auto& p : allProcesses)
        if (p.arrivalTime == currentTime)
            readyQueue.push(&p);

    // 2. preemption check
    if (!readyQueue.empty()) {
        if (currentProcess == nullptr ||
            readyQueue.top()->priority < currentProcess->priority) {
            if (currentProcess != nullptr && currentProcess->remainingTime > 0)
                readyQueue.push(currentProcess);
            currentProcess = readyQueue.top();
            readyQueue.pop();
        }
    }

    // 3. execute 1 unit
    if (currentProcess != nullptr) {
        std::cout << "[Time " << currentTime << "] Running: " << currentProcess->id
                  << "  (Remaining: " << currentProcess->remainingTime << ")\n";
        currentProcess->remainingTime--;

        if (currentProcess->remainingTime == 0) {
            currentProcess->finishTime     = currentTime + 1;
            currentProcess->turnaroundTime = currentProcess->finishTime - currentProcess->arrivalTime;
            currentProcess->waitingTime    = currentProcess->turnaroundTime - currentProcess->burstTime;
            completed++;
            currentProcess = nullptr;
        }
    } else {
        std::cout << "[Time " << currentTime << "] CPU Idle\n";
    }
}

// ── live mode: 1 unit = 1 second ────────────────────────────────
void PriorityScheduler::runLive() {
    int completed = 0;
    int n = (int)allProcesses.size();
    Process* current = nullptr;

    std::cout << "=== Live Preemptive Priority Scheduler ===\n";
    while (completed < n) {
        tick(allProcesses, readyQueue, current, completed, currentTime);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        currentTime++;
    }
    displayResults();
}

// ── instant mode: no delay (for quick testing) ──────────────────
void PriorityScheduler::runInstant() {
    int completed = 0;
    int n = (int)allProcesses.size();
    Process* current = nullptr;

    std::cout << "=== Instant Preemptive Priority Scheduler ===\n";
    while (completed < n) {
        tick(allProcesses, readyQueue, current, completed, currentTime);
        currentTime++;
    }
    displayResults();
}

// ── results ─────────────────────────────────────────────────────
void PriorityScheduler::displayResults() {
    double totalWait = 0, totalTAT = 0;

    std::cout << "\n--- Final Statistics ---\n";
    std::cout << std::left
              << std::setw(10) << "Process"
              << std::setw(14) << "Waiting Time"
              << "Turnaround Time\n";
    std::cout << std::string(38, '-') << "\n";

    for (const auto& p : allProcesses) {
        std::cout << std::left
                  << std::setw(10) << p.id
                  << std::setw(14) << p.waitingTime
                  << p.turnaroundTime << "\n";
        totalWait += p.waitingTime;
        totalTAT  += p.turnaroundTime;
    }

    std::cout << "\nAvg Waiting Time    : " << totalWait / allProcesses.size() << "\n";
    std::cout << "Avg Turnaround Time : " << totalTAT  / allProcesses.size() << "\n";
}

//____________________main code______________________________
// #include "Priority-Scheduler/Preemptive_priority.hpp"
//
//
// int main() {
//     PriorityScheduler scheduler;
//
//     scheduler.addProcess(Process("P1", 0, 8, 3));
//     scheduler.addProcess(Process("P2", 1, 4, 1));
//     scheduler.addProcess(Process("P3", 2, 5, 2));
//     scheduler.addProcess(Process("P4", 3, 2, 1));
//
//
//
//     scheduler.runInstant();
//
//     return 0;
// }