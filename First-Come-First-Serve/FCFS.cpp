#include "FCFS.hpp"
#include<iostream>
#include<vector>
#include<queue>
#include<chrono>
#include<thread>
#include "../Process/Process.hpp"
using namespace std;

void FCFS_Simulation(vector<Process> &processes, queue<int> &ReadyProcessID){
    int currentTime=0;
    int currentProcessIndex=-1;
    int NumberOfCompletedProcesses=0;
    int TotalProcessesNo= processes.size();

    
   while(NumberOfCompletedProcesses<TotalProcessesNo){
    //setting processes in queue
        for(int i=0;i<TotalProcessesNo;i++){
            if(currentTime == processes[i].arrival_time && !processes[i].IsInQueue){
                ReadyProcessID.push(i);
                processes[i].IsInQueue = true;
                cout<<"At time "<<currentTime<< ", process of id "<<processes[i].process_id<< " is set in queue."<<"\n";
        }
   }
   // Cpu selects processes waiting in queue bu order of first coming
   if(currentProcessIndex==-1 && !ReadyProcessID.empty()){
       currentProcessIndex = ReadyProcessID.front();
       ReadyProcessID.pop();
       processes[currentProcessIndex].Start_time = currentTime; 
       cout<<"At time "<<currentTime<< ", cpu is having process of id "<< processes[currentProcessIndex].process_id<<"\n";
}
    // checking processes execution and decreasing burst time
    if(currentProcessIndex!=-1){
        processes[currentProcessIndex].burst_time--;
         cout << "At time " << currentTime << ", process of id " << processes[currentProcessIndex].process_id << " is running."<< "\n";

        if(processes[currentProcessIndex].burst_time==0){
            processes[currentProcessIndex].finish_time=currentTime + 1;
            processes[currentProcessIndex].IsCompleted=true;
            NumberOfCompletedProcesses++;
            cout<<"At time "<< currentTime +1<<", process of id "<< processes[currentProcessIndex].process_id<<" is finished"<<"\n";
            currentProcessIndex=-1;
    }
}
    else
        cout<<"At time "<< currentTime<<", cpu is idle "<<"\n";
   
    // for visualizing live console and updating current time for more processes to enter
    this_thread::sleep_for(chrono::seconds(1));
    currentTime++;
}
     cout<<"At time "<< currentTime<<", all processes finished"<<"\n";

     // calculations
     int Sum_Turnaround_Time=0;
     int Sum_Waiting_Time=0;
     float AvgTurnaround_time=0.0;
     float AvgWaiting_time=0.0;

     for(int i=0;i<TotalProcessesNo;i++){
        processes[i].calculate_turn_time();
        processes[i].calculate_waiting_time();

       Sum_Turnaround_Time += processes[i].turn_around_time;
       Sum_Waiting_Time += processes[i].waiting_time;
     }
     AvgTurnaround_time=(float)Sum_Turnaround_Time/TotalProcessesNo;
     AvgWaiting_time=(float)Sum_Waiting_Time/TotalProcessesNo;

    cout <<"Average turnaround time is: "<< AvgTurnaround_time << "\n";
    cout <<"Average waiting time is: "<< AvgWaiting_time<< "\n";
}

/*int main(){
    vector<Process> processes;
    queue<int> ReadyProcessID;

    // for testing
    processes.push_back(Process(1, 0, 4)); 
    processes.push_back(Process(2, 1, 2));
    processes.push_back(Process(3, 2, 1));

    // start live simulation
    FCFS_Simulation(processes,ReadyProcessID);
    
    // display calculations
    cout << "\nPID\tAT\tBT\tCT\tTAT\tWT\n";
    for (const auto& p : processes) {
        cout << p.process_id << "\t" 
             << p.arrival_time << "\t" 
             << p.copy_burst_time << "\t" 
             << p.finish_time << "\t" 
             << p.turn_around_time << "\t" 
             << p.waiting_time << "\n";
    }
}
*/
