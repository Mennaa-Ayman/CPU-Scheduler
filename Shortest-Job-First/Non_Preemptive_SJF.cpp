#include "Non_Preemptive_SJF.hpp"
#include "../Process/Process.hpp"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <stdexcept>

// ──────────────────────────────────────────────────────────────────────────────
//  Public API
// ──────────────────────────────────────────────────────────────────────────────

void SJFScheduler::addProcess(int pid, int arrivalTime, int burstTime)
{
    if (burstTime <= 0)
        throw std::invalid_argument("Burst time must be positive.");
    if (arrivalTime < 0)
        throw std::invalid_argument("Arrival time cannot be negative.");

    Process p(pid, arrivalTime, burstTime);
    processes_.push_back(p);
    scheduled_ = false;
}

void SJFScheduler::schedule()
{
    if (processes_.empty())
        throw std::runtime_error("No processes to schedule.");

    results_.clear();

    std::vector<Process> remaining = processes_;
    int currentTime = 0;

    while (!remaining.empty()) {
        int idx = selectNext(remaining, currentTime);

        if (idx == -1) {
            // CPU idle — jump to earliest arrival
            int earliest = remaining[0].arrival_time;
            for (const auto& p : remaining)
                earliest = std::min(earliest, p.arrival_time);
            currentTime = earliest;
            continue;
        }

        Process& p      = remaining[idx];
        p.Start_time    = currentTime;
        p.finish_time   = currentTime + p.burst_time;
        currentTime     = p.finish_time;

        results_.push_back(p);
        remaining.erase(remaining.begin() + idx);
    }

    computeStats();
    scheduled_ = true;
}

// ──────────────────────────────────────────────────────────────────────────────
//  Accessors
// ──────────────────────────────────────────────────────────────────────────────

const std::vector<Process>& SJFScheduler::getProcesses() const
{
    if (!scheduled_)
        throw std::runtime_error("schedule() has not been called yet.");
    return results_;
}

double SJFScheduler::getAvgWaiting() const
{
    if (!scheduled_) throw std::runtime_error("schedule() not called.");
    double sum = 0;
    for (const auto& p : results_) sum += p.waiting_time;
    return sum / results_.size();
}

double SJFScheduler::getAvgTurnaround() const
{
    if (!scheduled_) throw std::runtime_error("schedule() not called.");
    double sum = 0;
    for (const auto& p : results_) sum += p.turn_around_time;
    return sum / results_.size();
}

double SJFScheduler::getAvgResponse() const
{
    if (!scheduled_) throw std::runtime_error("schedule() not called.");
    double sum = 0;
    for (const auto& p : results_) sum += (p.Start_time - p.arrival_time);
    return sum / results_.size();
}

double SJFScheduler::getCpuUtilization() const
{
    if (!scheduled_) throw std::runtime_error("schedule() not called.");
    if (results_.empty()) return 0.0;
    int totalBurst = 0;
    for (const auto& p : results_) totalBurst += p.copy_burst_time;
    int span = results_.back().finish_time - results_.front().Start_time;
    return span > 0 ? (100.0 * totalBurst / span) : 100.0;
}

void SJFScheduler::reset()
{
    processes_.clear();
    results_.clear();
    scheduled_ = false;
}

// ──────────────────────────────────────────────────────────────────────────────
//  Pretty-print
// ──────────────────────────────────────────────────────────────────────────────

void SJFScheduler::printResults() const
{
    if (!scheduled_) {
        std::cout << "Run schedule() first.\n";
        return;
    }

    // ── Gantt chart ───────────────────────────────────────────────────────────
    std::cout << "\n=== Gantt Chart ===\n|";
    for (const auto& p : results_)
        std::cout << "  P" << p.process_id << "  |";
    std::cout << "\n";

    std::cout << results_.front().Start_time;
    for (const auto& p : results_)
        std::cout << std::setw(6) << p.finish_time;
    std::cout << "\n\n";

    // ── Per-process table ─────────────────────────────────────────────────────
    const int W = 12;
    std::cout << std::left
              << std::setw(W) << "PID"
              << std::setw(W) << "Arrival"
              << std::setw(W) << "Burst"
              << std::setw(W) << "Start"
              << std::setw(W) << "Finish"
              << std::setw(W) << "Waiting"
              << std::setw(W) << "Turnaround"
              << std::setw(W) << "Response"
              << "\n";
    std::cout << std::string(8 * W, '-') << "\n";

    for (const auto& p : results_)
        std::cout << std::left
                  << std::setw(W) << p.process_id
                  << std::setw(W) << p.arrival_time
                  << std::setw(W) << p.copy_burst_time
                  << std::setw(W) << p.Start_time
                  << std::setw(W) << p.finish_time
                  << std::setw(W) << p.waiting_time
                  << std::setw(W) << p.turn_around_time
                  << std::setw(W) << (p.Start_time - p.arrival_time)
                  << "\n";

    // ── Averages ──────────────────────────────────────────────────────────────
    std::cout << "\n--- Averages ---\n"
              << std::fixed << std::setprecision(2)
              << "  Avg Waiting Time    : " << getAvgWaiting()     << "\n"
              << "  Avg Turnaround Time : " << getAvgTurnaround()  << "\n"
              << "  Avg Response Time   : " << getAvgResponse()    << "\n"
              << "  CPU Utilization     : " << getCpuUtilization() << " %\n\n";
}

// ──────────────────────────────────────────────────────────────────────────────
//  Private helpers
// ──────────────────────────────────────────────────────────────────────────────

int SJFScheduler::selectNext(const std::vector<Process>& remaining,
                             int currentTime) const
{
    int chosen = -1;

    for (int i = 0; i < static_cast<int>(remaining.size()); ++i) {
        const Process& p = remaining[i];
        if (p.arrival_time > currentTime) continue;  // not yet arrived

        if (chosen == -1) { chosen = i; continue; }  // first valid candidate

        const Process& best = remaining[chosen];

        // Shorter burst wins
        if (p.burst_time < best.burst_time) { chosen = i; continue; }
        if (p.burst_time > best.burst_time) continue;

        // Tie-break: earlier arrival
        if (p.arrival_time < best.arrival_time) { chosen = i; continue; }
        if (p.arrival_time > best.arrival_time) continue;

        // Tie-break: lower PID
        if (p.process_id < best.process_id) chosen = i;
    }

    return chosen;
}

void SJFScheduler::computeStats()
{
    for (auto& p : results_) {
        p.turn_around_time = p.finish_time  - p.arrival_time;
        p.waiting_time     = p.turn_around_time - p.copy_burst_time;
    }
}