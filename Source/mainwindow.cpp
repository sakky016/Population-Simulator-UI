#include <QThread>
#include <QMessageBox>
#include "mainwindow.h"
#include "person.h"
#include "ui_mainwindow.h"
#include "simulation.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setFixedSize(this->width(),this->height());

    outputParameters_t outputParameters = InitSimulation();

    // Update display
    UpdateStatistics(outputParameters);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//---------------------------------------------------------------------------
// Displays simulation timing
//---------------------------------------------------------------------------
void MainWindow::ShowSimulationTime(const outputParameters_t & outputParameters, time_t timeDifference)
{
    std::string msg = "Time elapsed: " + std::to_string(timeDifference) + " s. " + "| " +
            "Total Population: " + std::to_string(outputParameters.totalMalePopulation + outputParameters.totalFemalePopulation);

    ui->opSimulationTime->setText(QString::fromUtf8(msg.c_str()));
}

//---------------------------------------------------------------------------
// On button click: Simulate
//---------------------------------------------------------------------------
void MainWindow::on_btnSimulate_clicked()
{
    // Fetch input parameters
    inputParameters_t simulationInputParameters;
    simulationInputParameters.yearsToSimulate = ui->inpYearsToSimulate->displayText().toInt();
    simulationInputParameters.avgFertilityRate = ui->inpAvgFertilityRate->displayText().toInt();
    simulationInputParameters.maxMaleFertilityAge = ui->inpMaxMaleFertilityAge->displayText().toInt();
    simulationInputParameters.maxFemaleFertilityAge = ui->inpMaxFemaleFertilityAge->displayText().toInt();
    simulationInputParameters.minMaleMarriageAge = ui->inpMinMaleMarriageAge->displayText().toInt();
    simulationInputParameters.minFemaleMarriageAge = ui->inpMinFemaleMarriageAge->displayText().toInt();
    simulationInputParameters.avgLifeExpectancy = ui->inpAvgLifeExpectancy->displayText().toInt();

    // Busy indicator: START
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    ui->btnSimulate->setEnabled(false);

    time_t startTime = time(nullptr);

    // Execute simulation
    outputParameters_t outputParameters = Simulate(simulationInputParameters);

    // Update display
    UpdateStatistics(outputParameters);

    time_t endTime = time(nullptr);
    ShowSimulationTime(outputParameters, endTime - startTime);

    // Busy indicator: STOP
    QApplication::restoreOverrideCursor();
    if (!outputParameters.isSimulationComplete)
    {
        ui->btnSimulate->setEnabled(true);
    }
}

//---------------------------------------------------------------------------
// Displays logs in Output window
//---------------------------------------------------------------------------
void MainWindow::ShowLogsOnOutputWindow(const outputParameters_t outputParameters)
{
    for (size_t i = 0; i < outputParameters.logs.size(); i++)
    {
        std::string msg = outputParameters.logs[i];
        QString qmsg = QString::fromUtf8(msg.c_str());
        ui->opLogs->append(qmsg);
    }
}

//---------------------------------------------------------------------------
// Prepare a row in the Population table
//---------------------------------------------------------------------------
int MainWindow::AddPersonDetailsToPopulationTable(const std::vector<Person*> & populationList, int row)
{
    for (auto it = populationList.begin(); it != populationList.end(); it++)
    {
        Person *person = *it;

        // name
        QTableWidgetItem *nameCol = new QTableWidgetItem();
        nameCol->setText(QString::fromUtf8(person->GetName().c_str()));
        ui->opPopulationTable->setItem(row, COLUMN_NAME, nameCol);

        // gender
        QTableWidgetItem *genderCol = new QTableWidgetItem();
        genderCol->setText((person->GetGender() == MALE) ? "Male" : "Female");
        ui->opPopulationTable->setItem(row, COLUMN_GENDER, genderCol);

        // age
        QTableWidgetItem *ageCol = new QTableWidgetItem();
        ageCol->setText(QString::number(person->GetAge()));
        ui->opPopulationTable->setItem(row, COLUMN_AGE, ageCol);

        // spouse
        QTableWidgetItem *spouseCol = new QTableWidgetItem();
        Person *spouse = person->GetSpouse();
        std::string spouseName = spouse ? spouse->GetName() : "NONE";
        spouseCol->setText(QString::fromUtf8(spouseName.c_str()));
        ui->opPopulationTable->setItem(row, COLUMN_SPOUSE, spouseCol);

        // children
        QTableWidgetItem *childrenCol = new QTableWidgetItem();
        std::vector<Person*> children = person->GetChildren();
        std::string childrenNamesList;

        if (children.empty())
        {
            childrenNamesList = "NONE";
        }
        else
        {
            size_t index = 0;
            for (auto it = children.begin(); it != children.end(); it++)
            {
                Person* child = *it;
                childrenNamesList += child->GetName();

                index++;
                bool lastItem = index == children.size();
                if (!lastItem)
                {
                    childrenNamesList += ", ";
                }
            }
        }

        childrenCol->setText(QString::fromUtf8(childrenNamesList.c_str()));
        ui->opPopulationTable->setItem(row, COLUMN_CHILDREN, childrenCol);

        row++;
    }

    return row;
}

//---------------------------------------------------------------------------
// Updates the population table
//---------------------------------------------------------------------------
void MainWindow::UpdatePopulationTable(const outputParameters_t /* outputParameters*/ )
{
    // First clear the table
    ui->opPopulationTable->clearContents();

    std::vector<Person*> males = GetPopulationList(MALE);
    std::vector<Person*> females = GetPopulationList(FEMALE);

    ui->opPopulationTable->setRowCount(static_cast<int>(males.size() + females.size()));
    ui->opPopulationTable->setColumnCount(COLUMN_MAX);

    int row = 0;
    row = AddPersonDetailsToPopulationTable(males, row);
    row = AddPersonDetailsToPopulationTable(females, row);
}

//---------------------------------------------------------------------------
// Updates the UI with latest simulation results
//---------------------------------------------------------------------------
void MainWindow::UpdateStatistics(const outputParameters_t outputParameters)
{
    ui->opTotalYears->setText(QString::number(outputParameters.totalYears));
    ui->opTotalBirths->setText(QString::number(outputParameters.totalBirths));
    ui->opTotalDeaths->setText(QString::number(outputParameters.totalDeaths));
    ui->opTotalMales->setText(QString::number(outputParameters.totalMalePopulation));
    ui->opTotalFemales->setText(QString::number(outputParameters.totalFemalePopulation));
    ui->opSexRatio->setText(QString::number(outputParameters.sexRatio));
    ui->opAvgMaleAge->setText(QString::number(outputParameters.avgMaleAge));
    ui->opAvgFemaleAge->setText(QString::number(outputParameters.avgFemaleAge));

    // Display logs corresponding to this year
    ShowLogsOnOutputWindow(outputParameters);

    // Update the Population Table
    UpdatePopulationTable(outputParameters);
}

//---------------------------------------------------------------------------
// On button click: Clears all the simulation results
//---------------------------------------------------------------------------
void MainWindow::ClearSimulationResults()
{
    // Clear output window
    ui->opLogs->clear();

    // Clear population table
    ui->opPopulationTable->clearContents();
    ui->opPopulationTable->setRowCount(0);

    // Clear simulation timing
    ui->opSimulationTime->clear();
}

//---------------------------------------------------------------------------
// On button click: Clear Output Window
//---------------------------------------------------------------------------
void MainWindow::on_btnClear_clicked()
{
    ClearLogsForIteration();

    ui->opLogs->clear();
}

//---------------------------------------------------------------------------
// On button click: Show Population Detail
//---------------------------------------------------------------------------
void MainWindow::on_btnPopulationDetail_clicked()
{
    Log("\n*** Population Details ***");
    DisplayPopulationDetails(MALE);
    DisplayPopulationDetails(FEMALE);
    outputParameters_t outputParameters = GetOutputParameters();
    ShowLogsOnOutputWindow(outputParameters);
}

//---------------------------------------------------------------------------
// On button click: Quit
//---------------------------------------------------------------------------
void MainWindow::on_btnQuit_clicked()
{
    this->close();
}

//---------------------------------------------------------------------------
// On button click: Restart Simulation
//---------------------------------------------------------------------------
void MainWindow::on_btnRestartSimulation_clicked()
{
    ClearSimulationResults();

    outputParameters_t outputParameters = InitSimulation();

    // Update display
    UpdateStatistics(outputParameters);

    // Enable Simulate button
    ui->btnSimulate->setEnabled(true);
}
