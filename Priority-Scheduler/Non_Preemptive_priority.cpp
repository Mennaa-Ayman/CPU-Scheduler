#include "Non_Preemptive_priority.hpp"
#include <iomanip>

// simulates one CPU time unit
static void executeOneTick(
    std::vector<Process>& allProcesses,
    std::priority_queue<Process*, std::vector<Process*>, NonPreemptivePriorityCompare>& readyQueue,
    Process*& running,
    int& completedCount,
    int t)
{
    // enqueue arriving processes
    for (auto& p : allProcesses)
        if (p.arrival_time == t && !p.IsInQueue && !p.IsCompleted) {
            p.IsInQueue = true;
            readyQueue.push(&p);
        }

    // pick next only when CPU is free (non-preemptive)
    if (running == nullptr && !readyQueue.empty()) {
        running = readyQueue.top();
        readyQueue.pop();
        running->Start_time = t;
    }

    // execute 1 unit
    if (running != nullptr) {
        std::cout << "[t=" << t << "]  Process " << running->process_id
            << "  |  Remaining: " << running->copy_burst_time << "\n";
        running->copy_burst_time--;

        if (running->copy_burst_time == 0) {
            running->finish_time = t + 1;
            // use burst_time (original) since copy_burst_time is now 0
            running->turn_around_time = running->finish_time - running->arrival_time;
            running->waiting_time = running->turn_around_time - running->burst_time;
            running->IsCompleted = true;
            completedCount++;
            running = nullptr;
        }
    }
    else {
        std::cout << "[t=" << t << "]  CPU Idle\n";
    }
}

// 1 unit = 1 real second
void NonPreemptivePriorityScheduler::runLive() {
    int done = 0;
    int total = (int)allProcesses.size();
    Process* running = nullptr;

    std::cout << "\n====== Non-Preemptive Priority Scheduler [Live Mode] ======\n\n";
    while (done < total) {
        executeOneTick(allProcesses, readyQueue, running, done, timer);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        timer++;
    }
}

// no delay, runs instantly
void NonPreemptivePriorityScheduler::runInstant() {
    int done = 0;
    int total = (int)allProcesses.size();
    Process* running = nullptr;

    std::cout << "\n====== Non-Preemptive Priority Scheduler [Instant Mode] ======\n\n";
    while (done < total) {
        executeOneTick(allProcesses, readyQueue, running, done, timer);
        timer++;
    }
}
