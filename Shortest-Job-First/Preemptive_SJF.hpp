#ifndef PREEMPTIVE_SJF_HPP
#define PREEMPTIVE_SJF_HPP

#include "Process.hpp"
#include <vector>
#include <algorithm>

class PreemptiveSJF {
private:
    int time;
    int completed;
    int minIndex;
    int minRemainingTime;
    std::vector<Process>& processes;

    static bool compareArrival(const Process& a, const Process& b);

public:
    PreemptiveSJF(std::vector<Process>& processes);
    void run();
    bool all_finished();
    int get_current_process();
};

#endif