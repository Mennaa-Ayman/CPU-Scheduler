#ifndef PROCESS_HPP
#define PROCESS_HPP

class Process {
public:
    int process_id;
    int arrival_time;
    int burst_time;     // Process duration
    int copy_burst_time; // to keep the intial burst time value
    int waiting_time;   
    int finish_time;
    int turn_around_time;
    int priority;

    Process(int process_id, int arrival_time = 0, int burst_time = 0, int priority = 5);

    int burst_time_now();

    void calculate_waiting_time();

    void calculate_turn_time();
};

#endif