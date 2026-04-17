# CPU Scheduling

## Types of Schedulers
1. FCFS
2. SJF (Preemptive and Non Preemptive)
3. Priority (Preemptive and Non Preemptive) (the smaller the priority number the higher the priority)
4. Round Robin

## Input: 
Type of scheduler + no of Processes ready to run currently + required 
information about each process according to the scheduler type.

## Operation:
Two modes of operation:
**1. Static Mode:**
- Calculates and displays the full scheduling results (Gantt chart, turnaround times, and waiting times) instantly for a pre-defined batch of processes without a timer.

**2. Dynamic Mode:**
- Simulates the CPU scheduling process in real-time.
- Allows users to pause, resume, and dynamically inject new processes during execution.
- Remaining burst time table is updated as time progresses.

## Outputs:
- Timeline showing the order and time taken by each process (Gantt Chart) drawn live.
- Average waiting time and average turnaround time
- Remaining burst time updated table live

## How to run
**1. Set-Location**
```
 "../CPU_Scheduling_Project"  
```
**2. Build the GUI**
```
cmake --build build-gui --target CPU_Scheduler
```
**3. Execute it**
```
CPU_Scheduler.exe
```