#include "Process.hpp"

Process::Process(int process_id, int arrival_time, int burst_time, int priority):
    process_id(process_id),arrival_time(arrival_time),burst_time(burst_time), copy_burst_time(burst_time),
    waiting_time(0),finish_time(0), turn_around_time(0), priority(priority)
    {}

int Process::burst_time_now() {
    return copy_burst_time;
}

void Process::calculate_waiting_time() {
    this->waiting_time = finish_time - arrival_time - copy_burst_time;
}

void Process::calculate_turn_time() {
    this->turn_around_time = finish_time - arrival_time;
}