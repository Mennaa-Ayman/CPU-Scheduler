#ifndef LIVEMODE_HPP
#define LIVEMODE_HPP

#include "../Process/Process.hpp"
#include "../Live-Scheduling-Manager/SchedulingManager.hpp"
#include <QWidget>
#include <QTimer>
#include <QGraphicsScene>
#include <memory>
#include <vector>
#include <QString>


namespace Ui {
class ResultsWindow;
}

/**
 * LiveMode - GUI for real-time CPU scheduling simulation with pause/resume/inject
 * Displays live Gantt chart and process table
 * Handles dynamic process injection while paused
 */
class ResultsWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ResultsWindow(QWidget *parent = nullptr);
    ~ResultsWindow();
    
    /**
     * Initialize the live scheduler with processes and algorithm
     */
    void setScheduleData(std::vector<Process> processes, QString algo, int quantum);

    /**
     * Update display with current simulation state
     * Called every timer tick
     */
    void updateLiveDisplay(int currentTime, int runningProcessID, const std::vector<Process>& processes);

    /**
     * Called when user wants to inject a new process
     * Shows dialog and handles injection
     */
    void injectNewProcess();

private slots:
    void on_pushButton_clicked();          // Add Process button
    void on_pushButton_2_clicked();        // Pause button
    void on_pushButton_3_clicked();        // Resume button
    void onSimulationTick();

signals:
    void requestToggleTimer(bool pause);
    void processAddedLive(Process newP);
    void simulationPaused();
    void simulationResumed();

private:
    Ui::ResultsWindow *ui;
    std::unique_ptr<SchedulingManager> scheduler_;
    QTimer *simulationTimer_;
    QGraphicsScene *timelineScene_;
    
    QString selectedAlgo_;
    int quantum_;
    int processIdCounter_;
    bool isPaused_;
    std::vector<int> ganttHistory_;

    // Helper methods
    void startSimulation_();
    void updateProcessTable_();
    void updateGanttChart_();
    SchedulingManager::SchedulerType resolveSchedulerType_(const QString& algo) const;
    QColor colorForProcess_(int processId) const;
};

#endif // LIVEMODE_HPP
