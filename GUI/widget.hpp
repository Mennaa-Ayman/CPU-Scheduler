#ifndef WIDGET_H
#define WIDGET_H
#include <QTimer>
#include <QWidget>
#include <vector>
#include "../Process/Process.hpp"
#include "../First-Come-First-Serve/FCFS.hpp"
#include "../Round-Robin/Round_Robin.hpp"
#include "../Shortest-Job-First/Preemptive_SJF.hpp"
#include "../Shortest-Job-First/Non_Preemptive_SJF.hpp"
#include "NonLiveMode.hpp"
#include "LiveMode.hpp"

class chart;

class ResultsWindow;

QT_BEGIN_NAMESPACE

namespace Ui {

class Widget;

}

QT_END_NAMESPACE

class Widget : public QWidget

{

    Q_OBJECT

public:

    explicit Widget(QWidget *parent = nullptr);

    ~Widget() override;

private slots:

    void on_comboBoxalgo_currentTextChanged(const QString &text);

    void on_pushButton_clicked();

    void on_pushButton_3_clicked(); // Must match pushButton_3 exactly

    void onTimerTick();

    void on_runButton_clicked();

    void on_pauseButton_clicked();

private:

    Ui::Widget *ui;

    std::vector<Process> allProcesses;

    QTimer *simulationTimer;

    int currentTime = 0;

    int currentRunningID = -1;

    int getRemainingTime(int pID);

    chart *chartwindow;

    ResultsWindow *resultsWindow = nullptr; // Add this line

    bool liveModeActive = false;

};

#endif // WIDGET_H

