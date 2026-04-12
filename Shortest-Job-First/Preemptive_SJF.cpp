#include "Preemptive_SJF.hpp"

using namespace std;

bool PreemptiveSJF::compareArrival(const Process& a, const Process& b) {
    return a.arrival_time < b.arrival_time;
}

PreemptiveSJF::PreemptiveSJF(vector<Process>& processes)
    : time(0), completed(0), minIndex(-1), minRemainingTime(1e9), processes(processes) {

    sort(this->processes.begin(), this->processes.end(), compareArrival);
}
// Returns true when all processes are finished
bool PreemptiveSJF::all_finished() {
    return completed == processes.size();
}
// Return current process_id
int PreemptiveSJF::get_current_process() {
    if (minIndex != -1)
        return processes[minIndex].process_id;
    return -1;
}

void PreemptiveSJF::run() {
    int n = processes.size();
	// Loop until all processes are completed
    while (completed != n) {
        minIndex = -1;
        minRemainingTime = 1e9;

        // choose shortest remaining (burst_time now = remaining)
        for (int i = 0; i < n; i++) {
            if (processes[i].arrival_time <= time &&
                processes[i].burst_time > 0 &&
                processes[i].burst_time < minRemainingTime) {

                minRemainingTime = processes[i].burst_time;
                minIndex = i;
            }
        }
		// no process is ready, CPU idle
        if (minIndex == -1) {
            time++;
            continue;
        }

        // execute
        processes[minIndex].burst_time--;
        time++;

        // finish process
        if (processes[minIndex].burst_time == 0) {
            completed++;
            processes[minIndex].finish_time = time;

            // calculations 
            processes[minIndex].turn_around_time =
                processes[minIndex].finish_time - processes[minIndex].arrival_time;

            processes[minIndex].waiting_time =
                processes[minIndex].turn_around_time - processes[minIndex].copy_burst_time;
        }
    }
}