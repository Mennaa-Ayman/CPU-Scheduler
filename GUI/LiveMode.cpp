#include "LiveMode.hpp"
#include "ui_LiveMode.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QGraphicsRectItem>
#include <QPen>
#include <QBrush>
#include <QColor>
#include <QTimer>
#include <algorithm>
#include <iostream>

namespace {
QString algorithmKey(const QString& algo) {
    return algo.trimmed().toLower();
}
}

ResultsWindow::ResultsWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ResultsWindow)
    , scheduler_(nullptr)
    , simulationTimer_(nullptr)
    , timelineScene_(nullptr)
    , quantum_(1)
    , processIdCounter_(100)  
    , isPaused_(false)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_QuitOnClose, false);
    
    timelineScene_ = new QGraphicsScene(this);
    ui->graphicsView->setScene(timelineScene_);

    ui->processTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->processTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->processTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    setWindowTitle("Live Mode");
}

ResultsWindow::~ResultsWindow()
{
    if (simulationTimer_) {
        simulationTimer_->stop();
        delete simulationTimer_;
    }
    delete ui;
}

void ResultsWindow::setScheduleData(std::vector<Process> processes, QString algo, int quantum)
{
    selectedAlgo_ = algo;
    quantum_ = quantum;
    ganttHistory_.clear();
    processIdCounter_ = 100;
    
    std::cout << "[LiveMode] Setting schedule data for algorithm: " << algo.toStdString() 
              << " with quantum=" << quantum << "\n";

    SchedulingManager::SchedulerType schedulerType = resolveSchedulerType_(algo);

    scheduler_ = std::make_unique<SchedulingManager>(schedulerType, quantum);
    scheduler_->initialize(processes);

    if (!simulationTimer_) {
        simulationTimer_ = new QTimer(this);
        connect(simulationTimer_, &QTimer::timeout, this, &ResultsWindow::onSimulationTick);
    }

    ui->processTable->clearContents();
    ui->processTable->setRowCount(0);
    timelineScene_->clear();
    ui->label->setText("0.0");
    ui->label_12->setText("0.0");
    setWindowTitle(QString("Live Mode - %1").arg(algo));

    startSimulation_();
}

void ResultsWindow::startSimulation_()
{
    isPaused_ = false;
    if (simulationTimer_) {
        simulationTimer_->start(1000);  // 1000ms = 1 second per tick
    }
    std::cout << "[LiveMode] Simulation started\n";
}

void ResultsWindow::onSimulationTick()
{
    if (!scheduler_ || isPaused_) {
        return;
    }

    int runningPID = scheduler_->tick();
    ganttHistory_.push_back(runningPID);
    updateProcessTable_();
    updateGanttChart_();
    ui->label_12->setText(QString::number(scheduler_->getCurrentTime()));
    setWindowTitle(QString("Live Mode - t=%1").arg(scheduler_->getCurrentTime()));

    if (scheduler_->isComplete()) {
        simulationTimer_->stop();
        QMessageBox::information(this, "Complete", "All processes finished!");
        std::cout << "[LiveMode] Simulation completed\n";
    }
}

void ResultsWindow::on_pushButton_2_clicked()
{
    if (!scheduler_) return;

    isPaused_ = true;
    scheduler_->pause();
    if (simulationTimer_) {
        simulationTimer_->stop();
    }
    emit simulationPaused();
    std::cout << "[LiveMode] Simulation paused\n";
}

void ResultsWindow::on_pushButton_3_clicked()
{
    if (!scheduler_) return;

    isPaused_ = false;
    scheduler_->resume();
    if (simulationTimer_ && !scheduler_->isComplete()) {
        simulationTimer_->start(1000);
    }
    emit simulationResumed();
    std::cout << "[LiveMode] Simulation resumed\n";
}

void ResultsWindow::on_pushButton_clicked()
{
    if (!scheduler_) {
        QMessageBox::warning(this, "Error", "Scheduler not initialized!");
        return;
    }

    if (!isPaused_) {
        QMessageBox::warning(this, "Wait", "Please Pause the simulation first!");
        return;
    }

    // Get Process ID
    bool okPid;
    int pid = ui->plainTextEdit->text().toInt(&okPid);
    if (!okPid || pid < 0) {
        QMessageBox::warning(this, "Invalid Input", "Please enter a valid numeric Process ID.");
        return;
    }

    // Get burst time
    bool okBurst;
    int burst = ui->plainTextEdit_2->text().toInt(&okBurst);
    if (!okBurst || burst <= 0) {
        QMessageBox::warning(this, "Invalid Input", "Please enter a valid numeric CPU Burst Time (>0).");
        return;
    }

    bool ok = true;
    // Prompt for priority if applicable
    int priority = 5;  // default
    if (selectedAlgo_.contains("priority", Qt::CaseInsensitive)) {
        priority = QInputDialog::getInt(
            this,
            "New Process",
            "Enter Priority (lower number = higher priority):",
            5, 1, 10, 1, &ok
        );
        if (!ok) {
            return;
        }
    }

    // Create new process
    Process newProcess(pid);
    newProcess.arrival_time = scheduler_->getCurrentTime();
    newProcess.burst_time = burst;
    newProcess.copy_burst_time = burst;
    newProcess.priority = priority;
    newProcess.waiting_time = 0;
    newProcess.finish_time = 0;
    newProcess.turn_around_time = 0;
    newProcess.IsCompleted = false;
    newProcess.IsInQueue = false;

    // Inject into scheduler
    scheduler_->injectProcess(newProcess);

    // Update display
    updateProcessTable_();

    QMessageBox::information(
        this, 
        "Injected", 
        QString("Process %1 injected with burst time %2\nReady queue re-evaluated!").arg(newProcess.process_id).arg(burst)
    );

    std::cout << "[LiveMode] Process " << newProcess.process_id << " injected\n";
}

void ResultsWindow::updateProcessTable_()
{
    if (!ui || !scheduler_) return;

    auto processes = scheduler_->getCurrentProcesses();
    int runningPID = scheduler_->getRunningProcessID();

    ui->processTable->setColumnCount(6);
    ui->processTable->setHorizontalHeaderLabels({"PID", "Arrival", "Remaining", "Priority", "Waiting", "Status"});
    ui->processTable->setRowCount(static_cast<int>(processes.size()));

    double totalWaiting = 0.0;
    double totalTurnaround = 0.0;

    for (int i = 0; i < static_cast<int>(processes.size()); ++i) {
        const auto& process = processes[i];
        QString status = "Ready";
        if (process.IsCompleted) {
            status = "Done";
        } else if (process.process_id == runningPID) {
            status = "Running";
        } else if (process.arrival_time > scheduler_->getCurrentTime()) {
            status = "Waiting";
        }

        totalWaiting += process.waiting_time;
        if (process.finish_time > 0) {
            totalTurnaround += process.turn_around_time;
        }

        ui->processTable->setItem(i, 0, new QTableWidgetItem(QString::number(process.process_id)));
        ui->processTable->setItem(i, 1, new QTableWidgetItem(QString::number(process.arrival_time)));
        ui->processTable->setItem(i, 2, new QTableWidgetItem(QString::number(process.burst_time)));
        ui->processTable->setItem(i, 3, new QTableWidgetItem(QString::number(process.priority)));
        ui->processTable->setItem(i, 4, new QTableWidgetItem(QString::number(process.waiting_time)));
        ui->processTable->setItem(i, 5, new QTableWidgetItem(status));
    }

    if (!processes.empty()) {
        ui->label->setText(QString::number(totalWaiting / processes.size(), 'f', 2));
        ui->label_12->setText(QString::number(totalTurnaround / processes.size(), 'f', 2));
    }
}

void ResultsWindow::updateGanttChart_()
{
    if (!ui || !scheduler_) return;

    timelineScene_->clear();

    const int cellWidth = 50;
    const int cellHeight = 40;
    const int xOffset = 10;
    const int yOffset = 10;

    for (int i = 0; i < static_cast<int>(ganttHistory_.size()); ++i) {
        int pid = ganttHistory_[i];
        QRectF rect(xOffset + i * cellWidth, yOffset, cellWidth, cellHeight);
        QColor color = pid < 0 ? QColor("#4E4E52") : colorForProcess_(pid);
        timelineScene_->addRect(rect, QPen(Qt::black), QBrush(color));

        QString text = pid < 0 ? "Idle" : QString("P%1").arg(pid);
        QGraphicsTextItem* label = timelineScene_->addText(text);
        label->setDefaultTextColor(Qt::white);
        label->setPos(rect.x() + 8, rect.y() + 8);

        QGraphicsTextItem* timeLabel = timelineScene_->addText(QString::number(i));
        timeLabel->setDefaultTextColor(Qt::white);
        timeLabel->setPos(rect.x(), rect.y() + cellHeight + 2);
    }

    timelineScene_->setSceneRect(0, 0, std::max(1, static_cast<int>(ganttHistory_.size()) * cellWidth + 40), 100);
}

void ResultsWindow::updateLiveDisplay(int currentTime, int runningProcessID, const std::vector<Process>& processes)
{
    // Legacy method for compatibility - now handled by timer
    // Can keep for backward compatibility or remove if not used
}

void ResultsWindow::injectNewProcess()
{
    // Can be called programmatically instead of button click
    on_pushButton_clicked();
}

SchedulingManager::SchedulerType ResultsWindow::resolveSchedulerType_(const QString& algo) const
{
    const QString key = algorithmKey(algo);
    if (key == "fcfs" || key.contains("first come") || key.contains("firstcome")) {
        return SchedulingManager::FCFS;
    }
    if (key.contains("round robin")) {
        return SchedulingManager::ROUND_ROBIN;
    }
    if (key.contains("non preemptive priority") || key.contains("nonpreemptive priority")) {
        return SchedulingManager::PRIORITY_NON_PREEMPTIVE;
    }
    if (key.contains("preemptive priority")) {
        return SchedulingManager::PRIORITY_PREEMPTIVE;
    }
    if (key.contains("non preemptive sjf") || key.contains("nonpreemptive sjf") || key.contains("sjf non preemptive")) {
        return SchedulingManager::SJF_NON_PREEMPTIVE;
    }
    if (key.contains("preemptive sjf") || key.contains("preemptive_sjf") || key.contains("sjf preemptive")) {
        return SchedulingManager::SJF_PREEMPTIVE;
    }
    return SchedulingManager::FCFS;
}

QColor ResultsWindow::colorForProcess_(int processId) const
{
    static const QColor palette[] = {
        QColor("#F48FB1"), QColor("#81C784"), QColor("#64B5F6"), QColor("#FFB74D"),
        QColor("#BA68C8"), QColor("#4DD0E1"), QColor("#AED581"), QColor("#FF8A65")
    };
    const int index = processId < 0 ? 0 : (processId % (sizeof(palette) / sizeof(palette[0])));
    return palette[index];
}

