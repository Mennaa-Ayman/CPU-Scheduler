#include "widget.hpp"
#include "ui_widget.h"
#include "NonLiveMode.hpp"
#include <QMessageBox>
#include "LiveMode.hpp"

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget) {
    ui->setupUi(this);
    ui->processTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->processTable->setColumnCount(3);
    ui->processTable->setHorizontalHeaderLabels({"ID", "Arrival", "Burst"});

    // This ensures they are hidden the moment the app starts
    ui->frame_priority->setVisible(false);
    ui->quantum_frame->setVisible(false);

    // 1. Set up Table Columns [cite: 20]
    ui->processTable->setColumnCount(3);
    ui->processTable->setHorizontalHeaderLabels({"ID", "Arrival", "Burst"});

    // 2. Initialize the Timer [cite: 13]
    simulationTimer = new QTimer(this);
    connect(simulationTimer, &QTimer::timeout, this, &Widget::onTimerTick);

    ui->processTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->processTable->setSelectionMode(QAbstractItemView::SingleSelection);
}

Widget::~Widget() {
    delete ui;
}

void Widget::on_comboBoxalgo_currentTextChanged(const QString &text) {
    // 1. Hide frames to follow "Don't ask for unused info" rule
    ui->frame_priority->setVisible(false);
    ui->quantum_frame->setVisible(false);

    // 2. Adjust UI and Table based on the chosen algorithm [cite: 3, 9]
    if (text == "Round Robin") {
        ui->quantum_frame->setVisible(true);
        ui->quantum_frame->raise();

        ui->processTable->setColumnCount(4);
        ui->processTable->setHorizontalHeaderLabels({"ID", "Arrival", "Burst", "Quantum"});
    }
    else if (text == "Preemptive priority" || text == "Non Preemptive priority") {
        ui->frame_priority->setVisible(true);
        ui->frame_priority->raise();

        ui->processTable->setColumnCount(4);
        ui->processTable->setHorizontalHeaderLabels({"ID", "Arrival", "Burst", "Priority"});
    }
    else {
        ui->processTable->setColumnCount(3);
        ui->processTable->setHorizontalHeaderLabels({"ID", "Arrival", "Burst"});
    }
}

void Widget::on_pushButton_clicked() {
    QString quantum = ui->quantum_text->text();     // ... existing code to add row and first 3 items ...// ADD THIS: If it's Round Robin, add the Quantum value to the 4th columnif (ui->quantum_frame->isVisible()) {         ui->processTable->setItem(row, 3, new QTableWidgetItem(quantum));     }
    QString id = ui->txtProcessID->text();
    int arrival = ui->txtArrivalTime->text().toInt();
    int burst = ui->txtBurstTime->text().toInt();
    int priority = ui->txtPriority->text().toInt();
    QString arrivalStr = ui->txtArrivalTime->text().trimmed();

    QString burstStr = ui->txtBurstTime->text().trimmed();

    // 1. Check ID
    if (id.isEmpty()) {
        QMessageBox::warning(this, "Error", "Process ID cannot be empty!");
        return;
    }

    if (arrivalStr.isEmpty() || burstStr.isEmpty()) {
        QMessageBox::warning(this, "Error", "Arrival and Burst times cannot be empty!");
        return;
    }

    // 2. Check times
    if (arrival < 0 || burst <= 0) {
        QMessageBox::warning(this, "Error", "Arrival must be >= 0 and Burst must be > 0 !");
        return;
    }

    // 3. Check Priority (only if visible)
    if (ui->frame_priority->isVisible() && priority <= 0) {
        QMessageBox::warning(this, "Error", "Priority must be positive!");
        return;
    }
    
    // 4. Check Quantum (only if visible)
    if (ui->quantum_frame->isVisible()) {
        int q = quantum.toInt();
        if (q <= 0) {
            QMessageBox::warning(this, "Error", "Quantum must be positive!");
            return;
        }
    }

    // 1. Save to your Logic Vector
    allProcesses.push_back(Process(id.toInt(), arrival, burst, priority));

    // 2. Add a new row to the UI Table
    int row = ui->processTable->rowCount();
    ui->processTable->insertRow(row);
    ui->processTable->setItem(row, 0, new QTableWidgetItem(id));
    ui->processTable->setItem(row, 1, new QTableWidgetItem(QString::number(arrival)));
    ui->processTable->setItem(row, 2, new QTableWidgetItem(QString::number(burst)));

    // 3. Only add priority to the table if it's visible [cite: 11, 20]
    if (ui->frame_priority->isVisible()) {
        ui->processTable->setItem(row, 3, new QTableWidgetItem(QString::number(priority)));
    }
    else if (ui->quantum_frame->isVisible()) {
        QString quantum = ui->quantum_text->text();
        ui->processTable->setItem(row, 3, new QTableWidgetItem(quantum));
    }
}

void Widget::onTimerTick() {
    // 1. Run 1 unit of your scheduling logic (FCFS/SJF/etc.)
    // 2. Update the Burst Time cell in the table for the running process
    for (int i = 0; i < ui->processTable->rowCount(); ++i) {
        int pID = ui->processTable->item(i, 0)->text().toInt();
        if (pID == currentRunningID) {
            // Update the 'Burst Time' column live [cite: 20]
            int remaining = getRemainingTime(pID);
            ui->processTable->item(i, 2)->setText(QString::number(remaining));
        }
    }
    currentTime++;
}

int Widget::getRemainingTime(int pID) {
    for (const auto& p : allProcesses) {
        if (p.process_id == pID) {

            return p.burst_time;
        }
    }
    return 0;
}

void Widget::on_pushButton_3_clicked() {
    int currentRow = ui->processTable->currentRow();
    if (currentRow != -1) {
        // 1. Get the ID from the first column
        int pID = ui->processTable->item(currentRow, 0)->text().toInt();
        // 2. Remove from data vector to keep it in sync
        for (auto it = allProcesses.begin(); it != allProcesses.end(); ++it) {
            if (it->process_id == pID) {
                allProcesses.erase(it);
                break;
            }
        }
        // 3. Remove from UI table
        ui->processTable->removeRow(currentRow);

    }

}
void Widget::on_runButton_clicked() {
    // 1. Initialize the new window
    // We use a different name 'chartwindow' to avoid conflict with class name 'chart'
    chartwindow = new chart(nullptr);

    // 2. Pass algorithm type and process list
    QString selectedAlgo = ui->comboBoxalgo->currentText();

    // 3. Handle Quantum value
    int quantumm = 0;

    if (ui->quantum_frame->isVisible()) {
        quantumm = ui->quantum_text->text().toInt();
    }

    // 4. Send data (Ensure this function is defined in chart.cpp)
    chartwindow->setScheduleData(allProcesses, selectedAlgo, quantumm);

    // 5. Force the window to appear
    chartwindow->setAttribute(Qt::WA_DeleteOnClose); // Clean up memory when closed
    chartwindow->setWindowModality(Qt::ApplicationModal); // Block main window until this is closed
    chartwindow->raise();
    chartwindow->activateWindow();
}

void Widget::on_pauseButton_clicked() {
    resultsWindow = new ResultsWindow(nullptr);
    QString selectedAlgo = ui->comboBoxalgo->currentText();

    int quaantum = 0;
    if (ui->quantum_frame->isVisible()) {
        quaantum = ui->quantum_text->text().toInt();
    }

    if (!resultsWindow) {
    resultsWindow = new ResultsWindow(this);
}
    resultsWindow->show();
    resultsWindow->raise();
    resultsWindow->activateWindow();

    // 5. Force the window to appear
    resultsWindow->setScheduleData(allProcesses, selectedAlgo, quaantum);
    resultsWindow->setAttribute(Qt::WA_DeleteOnClose); // Clean up memory when closed
    resultsWindow->setWindowModality(Qt::ApplicationModal); // Block main window until this is closed
    resultsWindow->raise();
    resultsWindow->activateWindow();

}