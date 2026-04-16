#include "NonLiveMode.hpp"
#include "ui_NonLiveMode.h"

#include <QBrush>
#include <QPen>
#include <QColor>
#include <algorithm>
#include <queue>
#include "../First-Come-First-Serve/FCFS.hpp"
#include "../Shortest-Job-First/Non_Preemptive_SJF.hpp"
#include "../Shortest-Job-First/Preemptive_SJF.hpp"
#include "../Round-Robin/Round_Robin.hpp"

chart::chart(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::chart)
{
    ui->setupUi(this);

    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    // Default setup, will be dynamically updated later
    ui->tableWidget->setColumnCount(5);
    ui->tableWidget->setHorizontalHeaderLabels({"PID", "Arrival", "Burst", "Waiting", "Turnaround"});
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

chart::~chart()
{
    delete ui;
}

void chart::setScheduleData(QTableWidget *inputTable, QString algo, int quantum)
{
    this->selectedAlgo = algo;
    this->localQuantum = quantum;
    this->setWindowTitle("Scheduling Results: " + algo);

    localProcesses.clear();
    timeline.clear();

    // Read data from the input table
    int rowCount = inputTable->rowCount();
    for (int row = 0; row < rowCount; ++row) {
        if (inputTable->item(row, 0) && inputTable->item(row, 1) && inputTable->item(row, 2)) {
            Process p(
                inputTable->item(row, 0)->text().toInt(),
                inputTable->item(row, 1)->text().toInt(),
                inputTable->item(row, 2)->text().toInt(),
                0 // Default priority
                );

            // Read priority or quantum if the column exists
            if (inputTable->columnCount() > 3) {
                QString headerText = inputTable->horizontalHeaderItem(3)->text();
                if (headerText == "Priority" || headerText == "Quantum") {
                    if (inputTable->item(row, 3)) {
                        p.priority = inputTable->item(row, 3)->text().toInt();
                    }
                }
            }
            localProcesses.push_back(p);
        }
    }

    // ==========================================================
    // Execute the selected algorithm
    // ==========================================================

    if (algo == "FCFS") {
        std::queue<int> readyQueue;
        FCFS_Simulation(localProcesses, readyQueue);
        for (const auto& p : localProcesses) {
            timeline.push_back({p.process_id, p.Start_time, p.finish_time});
        }
    }

    else if (algo == "Round Robin") {
        RoundRobin rr(localProcesses, localQuantum);
        int lastPid = -1, blockStart = 0, prevTime = 0;

        while (!rr.all_finished()) {
            int pid = rr.get_current_process();
            int currTime = rr.time;

            if (pid != lastPid) {
                if (lastPid != -1) timeline.push_back({lastPid, blockStart, prevTime});
                blockStart = prevTime;
                lastPid = pid;
            }
            prevTime = currTime;
        }
        if (lastPid != -1) timeline.push_back({lastPid, blockStart, prevTime});
        localProcesses = rr.processes;
    }

    else if (algo == "Non Preemptive SJF" || algo == "SJF") {
        SJFScheduler sjf;
        for (const auto& p : localProcesses) sjf.addProcess(p.process_id, p.arrival_time, p.burst_time);
        sjf.schedule();
        auto results = sjf.getProcesses();
        for (const auto& p : results) {
            timeline.push_back({p.process_id, p.Start_time, p.finish_time});
            for(auto& lp : localProcesses) {
                if(lp.process_id == p.process_id) {
                    lp.finish_time = p.finish_time;
                    lp.waiting_time = p.waiting_time;
                    lp.turn_around_time = p.turn_around_time;
                }
            }
        }
    }

    else if (algo == "Preemptive SJF" || algo == "Preemptive_SJF" || algo == "SRTF") {
        int n = localProcesses.size();
        std::vector<Process> sim = localProcesses;
        int completed = 0, time = 0, lastPid = -1, blockStart = 0;

        while (completed < n) {
            int minIdx = -1, minRem = 1e9;
            for (int i = 0; i < n; i++) {
                if (sim[i].arrival_time <= time && sim[i].burst_time > 0 && sim[i].burst_time < minRem) {
                    minRem = sim[i].burst_time;
                    minIdx = i;
                }
            }
            int pid = (minIdx == -1) ? -1 : sim[minIdx].process_id;
            if (pid != lastPid) {
                if (lastPid != -1) timeline.push_back({lastPid, blockStart, time});
                blockStart = time; lastPid = pid;
            }
            if (minIdx == -1) { time++; continue; }
            sim[minIdx].burst_time--;
            time++;
            if (sim[minIdx].burst_time == 0) completed++;
        }
        if (lastPid != -1) timeline.push_back({lastPid, blockStart, time});

        PreemptiveSJF psjf(localProcesses);
        psjf.run();
        localProcesses = psjf.processes;
    }

    else if (algo == "Preemptive priority") {
        int n = localProcesses.size();
        std::vector<Process> sim = localProcesses;
        int completed = 0, time = 0, lastPid = -1, blockStart = 0;

        while (completed < n) {
            int bestIdx = -1, highestPri = 1e9;
            for (int i = 0; i < n; i++) {
                if (sim[i].arrival_time <= time && sim[i].burst_time > 0 && sim[i].priority < highestPri) {
                    highestPri = sim[i].priority;
                    bestIdx = i;
                }
            }
            int pid = (bestIdx == -1) ? -1 : sim[bestIdx].process_id;
            if (pid != lastPid) {
                if (lastPid != -1) timeline.push_back({lastPid, blockStart, time});
                blockStart = time; lastPid = pid;
            }
            if (bestIdx == -1) { time++; continue; }
            sim[bestIdx].burst_time--;
            time++;
            if (sim[bestIdx].burst_time == 0) completed++;
        }
        if (lastPid != -1) timeline.push_back({lastPid, blockStart, time});

        for (auto& p : localProcesses) {
            int completionTime = 0;
            for (const auto& b : timeline) if (b.pID == p.process_id) completionTime = std::max(completionTime, b.endTime);
            p.finish_time = completionTime;
            p.turn_around_time = p.finish_time - p.arrival_time;
            p.waiting_time = p.turn_around_time - p.copy_burst_time;
        }
    }

    else if (algo == "Non Preemptive priority") {
        int n = localProcesses.size();
        std::vector<Process> sim = localProcesses;
        int completed = 0, time = 0;

        while (completed < n) {
            int bestIdx = -1, highestPri = 1e9;
            for (int i = 0; i < n; i++) {
                if (sim[i].arrival_time <= time && sim[i].burst_time > 0 && sim[i].priority < highestPri) {
                    highestPri = sim[i].priority;
                    bestIdx = i;
                }
            }
            if (bestIdx == -1) { time++; continue; }
            int start = time;
            time += sim[bestIdx].burst_time;
            sim[bestIdx].burst_time = 0;
            timeline.push_back({sim[bestIdx].process_id, start, time});
            completed++;
        }

        for (auto& p : localProcesses) {
            int completionTime = 0;
            for (const auto& b : timeline) if (b.pID == p.process_id) completionTime = std::max(completionTime, b.endTime);
            p.finish_time = completionTime;
            p.turn_around_time = p.finish_time - p.arrival_time;
            p.waiting_time = p.turn_around_time - p.copy_burst_time;
        }
    }

    // Sort the timeline
    std::sort(timeline.begin(), timeline.end(), [](const ExecutionBlock& a, const ExecutionBlock& b) {
        return a.startTime < b.startTime;
    });

    drawGanttChart();
    populateTableAndAverages();

    this->show();
    update();
}

void chart::populateTableAndAverages()
{
    // Determine dynamic columns based on the selected algorithm
    bool isPriority = (selectedAlgo == "Preemptive priority" || selectedAlgo == "Non Preemptive priority");
    bool isRR = (selectedAlgo == "Round Robin");

    int colCount = 5;
    if (isPriority || isRR) {
        colCount = 6;
    }

    ui->tableWidget->setColumnCount(colCount);

    // Set headers dynamically
    QStringList headers = {"PID", "Arrival", "Burst"};
    if (isPriority) headers << "Priority";
    else if (isRR) headers << "Quantum";
    headers << "Waiting" << "Turnaround";

    ui->tableWidget->setHorizontalHeaderLabels(headers);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->tableWidget->setRowCount(localProcesses.size());

    double totalWait = 0, totalTurnaround = 0;

    for (int i = 0; i < localProcesses.size(); ++i) {
        int pid = localProcesses[i].process_id;
        int arrival = localProcesses[i].arrival_time;
        int burst = localProcesses[i].copy_burst_time;
        int waiting = localProcesses[i].waiting_time;
        int turnaround = localProcesses[i].turn_around_time;

        totalTurnaround += turnaround;
        totalWait += waiting;

        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(pid)));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(arrival)));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(burst)));

        // Handle the dynamic column offset
        int offset = 0;
        if (isPriority) {
            ui->tableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(localProcesses[i].priority)));
            offset = 1;
        } else if (isRR) {
            ui->tableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(localQuantum)));
            offset = 1;
        }

        ui->tableWidget->setItem(i, 3 + offset, new QTableWidgetItem(QString::number(waiting)));
        ui->tableWidget->setItem(i, 4 + offset, new QTableWidgetItem(QString::number(turnaround)));
    }

    double avgWait = totalWait / localProcesses.size();
    double avgTurnaround = totalTurnaround / localProcesses.size();

    ui->avgWaitOutput->setText(QString::number(avgWait, 'f', 2));
    ui->avgTurnaroundOutput->setText(QString::number(avgTurnaround, 'f', 2));
}

void chart::drawGanttChart()
{
    scene->clear();
    int x_offset = 0, y_offset = 20, rect_height = 50, scale = 30;

    QList<QColor> colors = {
        QColor("#F48FB1"),
        QColor("#90CAF9"),
        QColor("#A5D6A7"),
        QColor("#CE93D8"),
        QColor("#FFCC80"),
        QColor("#FFF59D")
    };

    for (const auto& block : timeline) {
        int duration = block.endTime - block.startTime;
        int rect_width = duration * scale;
        QColor blockColor = colors[block.pID % colors.size()];

        scene->addRect(x_offset, y_offset, rect_width, rect_height, QPen(Qt::black), QBrush(blockColor));

        QGraphicsTextItem *text = scene->addText("P" + QString::number(block.pID));
        text->setPos(x_offset + (rect_width / 2) - 12, y_offset + 12);

        QGraphicsTextItem *timeText = scene->addText(QString::number(block.startTime));
        timeText->setPos(x_offset - 5, y_offset + rect_height + 5);

        x_offset += rect_width;
        timeText->setDefaultTextColor(QColor("#F8BBD0"));
    }

    if (!timeline.empty()) {
        QGraphicsTextItem *finalTime = scene->addText(QString::number(timeline.back().endTime));
        finalTime->setDefaultTextColor(QColor("#F8BBD0"));
        finalTime->setPos(x_offset - 5, y_offset + rect_height + 5);
    }
}

// Empty implementation for compatibility
void chart::addLiveBlock(int pID, int time) {}
