#include "Round_Robin.hpp"

RoundRobin::RoundRobin(std::vector<Process> &processes, int tq)
: processes(processes), timequantum(tq > 0 ? tq : 1), quantum_counter(0), time(0), is_busy(false), current_process(nullptr){}

// Returns true when all processes are finished
bool RoundRobin::all_finished(){
    for(auto &p: processes){
        if(p.burst_time > 0) return false;
    }
    return true;
}

// Return current process_id
int RoundRobin::get_current_process(){
    if(all_finished()) return -1;

    // Add all processes to the ready queue at their arrive time.
    for(auto &p: processes){
        if(p.arrival_time == time && p.burst_time > 0){
            readyQueue.push(&p);
        }
    }

    // If CPU is idle, dispatch next ready process.
    if(!is_busy && !readyQueue.empty()){
        current_process = readyQueue.front();
        readyQueue.pop();
        is_busy = true;
        quantum_counter = 0;
    }

    // No process is ready yet, CPU stays idle for this time unit.
    if(!is_busy){
        time++;
        return -1;
    }

    int running_process_id = current_process->process_id;

    // Execute one CPU tick.
    current_process->burst_time--;
    time++;
    quantum_counter++;

    if(current_process->burst_time == 0){
        current_process->finish_time = time;
        current_process->calculate_waiting_time();
        current_process->calculate_turn_time();
        current_process = nullptr;
        is_busy = false;
        quantum_counter = 0;
    }
    else if(quantum_counter == timequantum){
        readyQueue.push(current_process);
        current_process = nullptr;
        is_busy = false;
        quantum_counter = 0;
    }

    return running_process_id;
}


