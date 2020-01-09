#ifndef SIMULATION_H
#define SIMULATION_H

#include "person.h"
#include <Qthread>
#include <string>
#include <unordered_set>
#include <vector>

// This is the margin which calculation of years will use
const int MARGIN_YEARS_PROBABILITY = 10;
const int NEXT_CHILD_GAP = 2;

// Input parameters that control the simulation
typedef struct inputParameters_tag
{
    int yearsToSimulate;
    int avgFertilityRate;
    int maxMaleFertilityAge;
    int maxFemaleFertilityAge;
    int minMaleMarriageAge;
    int minFemaleMarriageAge;
    int avgLifeExpectancy;
}inputParameters_t;

// Statistics
typedef struct outputParameters_tag
{
    time_t simulationTime;
    bool isSimulationComplete;
    long totalYears;
    long totalBirths;
    long totalDeaths;
    size_t totalMalePopulation;
    size_t totalFemalePopulation;
    double sexRatio;
    double avgMaleAge;
    double avgFemaleAge;
    std::vector<std::string> logs;
}outputParameters_t;


Q_DECLARE_METATYPE(outputParameters_t)

// Functions
outputParameters_t InitSimulation();
outputParameters_t GetOutputParameters();
std::unordered_set<Person*> & GetPopulationList(Gender_t gender);
outputParameters_t Simulate(const inputParameters_t &  simulationInputParameters);
void SimulateYear();
Person* CreatePerson(Gender_t gender);
void UpdatePersonList(Gender_t gender);
void DisplayPopulationDetails(Gender_t gender);

void Log(const std::string & str);
void ClearLogsForIteration();

//-----------------------------------------------------
// Simulation worker thread
//-----------------------------------------------------
class Worker : public QThread
{
    Q_OBJECT

private:
    inputParameters_t m_input;

public:
    Worker(inputParameters_t input) : QThread()
    {
        m_input.yearsToSimulate = input.yearsToSimulate;
        m_input.avgFertilityRate = input.avgFertilityRate;
        m_input.maxMaleFertilityAge = input.maxMaleFertilityAge;
        m_input.maxFemaleFertilityAge = input.maxFemaleFertilityAge;
        m_input.minMaleMarriageAge = input.minMaleMarriageAge;
        m_input.minFemaleMarriageAge = input.minFemaleMarriageAge;
        m_input.avgLifeExpectancy = input.avgLifeExpectancy;

        m_input = input;
    }

    void run() override
    {
        time_t startTime = time(nullptr);
        outputParameters_t result = Simulate(m_input);
        time_t endTime = time(nullptr);
        result.simulationTime = endTime - startTime;
        emit SimulationResultReady(result);
    }

signals:
    void SimulationResultReady(outputParameters_t result);
};

#endif // SIMULATION_H
