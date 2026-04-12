#ifndef FCFS_HPP
#define FCFS_HPP

#include "../Process/Process.hpp"
#include <vector>
#include <queue>

void FCFS_Simulation(std::vector<Process> &processes, std::queue<int> &ReadyProcessID);

#endif