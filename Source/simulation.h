#ifndef SIMULATION_H
#define SIMULATION_H

#include "person.h"
#include <string>
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


// Functions
outputParameters_t InitSimulation();
outputParameters_t GetOutputParameters();
std::vector<Person*> & GetPopulationList(Gender_t gender);
outputParameters_t Simulate(const inputParameters_t &  simulationInputParameters);
void SimulateYear();
Person* CreatePerson(Gender_t gender);
void UpdatePersonList(Gender_t gender);
void DisplayPopulationDetails(Gender_t gender);

void Log(const std::string & str);
void ClearLogsForIteration();


#endif // SIMULATION_H
