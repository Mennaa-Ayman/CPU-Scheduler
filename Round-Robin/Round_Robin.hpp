/* =========================================================================================
Round Robin CPU Scheduling Algorithm Implementation
Each process gets small unit of CPU time (time quantum), after the time is elapsed, 
the process is premted & added to the end of the ready queue
========================================================================================= */

#ifndef ROUND_ROBIN_HPP
#define ROUND_ROBIN_HPP
#include "../Process/Process.hpp"
#include <vector>
#include <queue>

class RoundRobin{
private:
    int timequantum;        // CPU unit time for each process
    int quantum_counter;    // time passed from the timequantum
    int time;               // Current time
    bool is_busy;           // Processer Busy or not
    std::queue<Process*> readyQueue;
    std::vector <Process> &processes;
    Process* current_process;

public:
    RoundRobin(std::vector<Process> & processes, int tq);
    int get_current_process();
    bool all_finished();

};

#endif