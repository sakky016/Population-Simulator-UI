#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "simulation.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

//---------------------------------------------------------------------------
// Column indices used in Population Table
//---------------------------------------------------------------------------
enum
{
    COLUMN_NAME = 0,
    COLUMN_GENDER,
    COLUMN_AGE,
    COLUMN_SPOUSE,
    COLUMN_CHILDREN,
    COLUMN_MAX, /* This should always be last */
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void UpdateStatistics(const outputParameters_t outputParameters);
    void UpdatePopulationTable(const outputParameters_t outputParameters);
    int AddPersonDetailsToPopulationTable(const std::vector<Person*> & populationList, int row);
    void ShowLogsOnOutputWindow(const outputParameters_t outputParameters);
    void ClearSimulationResults();
    void ShowSimulationTime(const outputParameters_t & outputParameters, time_t timeDifference);

private slots:
    void on_btnSimulate_clicked();

    void on_btnClear_clicked();

    void on_btnPopulationDetail_clicked();

    void on_btnQuit_clicked();

    void on_btnRestartSimulation_clicked();

private:
    Ui::MainWindow *ui;
};



#endif // MAINWINDOW_H
