#ifndef PREEMPTIVE_PRIORITY_HPP
#define PREEMPTIVE_PRIORITY_HPP

#include "Process.hpp" // استخدام الملف الخاص بكِ
#include <vector>
#include <queue>
#include <iostream>
#include <thread>
#include <chrono>

struct PriorityCompare {

    bool operator()(const Process* a, const Process* b) {
        return a->priority > b->priority;
    }
};

class PriorityScheduler {
private:
    std::vector<Process> allProcesses;
    std::priority_queue<Process*, std::vector<Process*>, PriorityCompare> readyQueue;
    int currentTime = 0;

public:
    void addProcess(const Process& p);
    void runLive();    // تطبيق نظام الـ 1 ثانية = 1 وحدة زمنية [cite: 13]
    void runInstant(); // تشغيل بدون انتظار [cite: 14]
    void displayResults();
};

#endif