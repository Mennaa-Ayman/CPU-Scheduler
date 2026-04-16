#ifndef CHART_H
#define CHART_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <vector>
#include <QString>
#include "../Process/Process.hpp"

namespace Ui {
class chart;
}

struct ExecutionBlock {
    int pID;
    int startTime;
    int endTime;
};

class chart : public QWidget {
    Q_OBJECT

public:
    explicit chart(QWidget *parent = nullptr);
    ~chart();


    void setScheduleData(QTableWidget *inputTable, QString algo, int quantum);
    void setScheduleData(const std::vector<Process>& processes, QString algo, int quantum)
    {
        QTableWidget table;
        table.setColumnCount(4);
        table.setRowCount(static_cast<int>(processes.size()));
        table.setHorizontalHeaderLabels({"PID", "Arrival", "Burst", "Priority"});

        for (int i = 0; i < static_cast<int>(processes.size()); ++i) {
            const Process& p = processes[i];
            table.setItem(i, 0, new QTableWidgetItem(QString::number(p.process_id)));
            table.setItem(i, 1, new QTableWidgetItem(QString::number(p.arrival_time)));
            table.setItem(i, 2, new QTableWidgetItem(QString::number(p.copy_burst_time)));
            table.setItem(i, 3, new QTableWidgetItem(QString::number(p.priority)));
        }

        setScheduleData(&table, algo, quantum);
    }
    void addLiveBlock(int pID, int time);

private:
    Ui::chart *ui;
    QGraphicsScene *scene;

    std::vector<Process> localProcesses;
    QString selectedAlgo;
    int localQuantum;

    std::vector<ExecutionBlock> timeline;

    void drawGanttChart();
    void populateTableAndAverages();
};

#endif // CHART_H