#include <iostream>
#include <QMessageBox>
#include "person.h"
#include <stdlib.h>
#include "simulation.h"
#include <sstream>
#include<time.h>

// Global parameters
static inputParameters_t g_simulationInputParameters{};
static outputParameters_t g_simulationOutputParameters{};

static std::vector<Person*> g_maleList;
static std::vector<Person*> g_femaleList;

//---------------------------------------------------------------------------
// Fetches population list on the basis of gender
//---------------------------------------------------------------------------
std::vector<Person*> & GetPopulationList(Gender_t gender)
{
    return (gender == MALE) ? g_maleList : g_femaleList;
}

//---------------------------------------------------------------------------
// Clears the specified gender population list
//---------------------------------------------------------------------------
void ClearPopulationList(Gender_t gender)
{
    std::vector<Person*> & populationList = GetPopulationList(gender);
    for (auto it = populationList.begin(); it != populationList.end(); it++)
    {
        Person *person = *it;
        delete person;
        person = nullptr;
    }

    populationList.clear();
}

//---------------------------------------------------------------------------------------------------------------
// @name                                : GetRandomNumber
//
// @description                         : Fetches a random number
//
// @param range                         : range
//
// @returns                             : random number in between [0,range)
//---------------------------------------------------------------------------------------------------------------
int GetRandomNumber(int range)
{
    return rand() % range;
}

//---------------------------------------------------------------------------------------------------------------
// @name                                : InitSimulation
//
// @description                         : Initialize simulation
//
// @returns                             : Output parameters
//---------------------------------------------------------------------------------------------------------------
outputParameters_t InitSimulation()
{
    // Use current time as seed for random generator
    srand(static_cast<unsigned int>(time(nullptr)));

    // Clear output parameters
    memset(&g_simulationOutputParameters, 0, sizeof(g_simulationOutputParameters));

    // Clear population list
    ClearPopulationList(MALE);
    ClearPopulationList(FEMALE);

    return g_simulationOutputParameters;
}

//---------------------------------------------------------------------------------------------------------------
// @name                                : GetOutputParameters
//
// @description                         : Fetches result of simulation
//
// @returns                             : Output parameters
//---------------------------------------------------------------------------------------------------------------
outputParameters_t GetOutputParameters()
{
    return g_simulationOutputParameters;
}

//---------------------------------------------------------------------------------------------------------------
// @name                                : CreatePerson
//
// @description                         : Creates a person as per the specified gender.
//
// @returns                             : Pointer to new person created
//---------------------------------------------------------------------------------------------------------------
Person* CreatePerson(Gender_t gender)
{
    Person *person = nullptr;

    if (gender == MALE)
    {
        size_t maleId = g_simulationOutputParameters.totalMalePopulation;
        person = new Person(maleId, MALE);
        g_simulationOutputParameters.totalMalePopulation++;
        Log(person->GetName() + " born.");
    }
    else
    {
        size_t femaleId = g_simulationOutputParameters.totalFemalePopulation;
        person = new Person(femaleId, FEMALE);
        g_simulationOutputParameters.totalFemalePopulation++;
        Log(person->GetName() + " born.");
    }

    g_simulationOutputParameters.totalBirths++;
    return person;
}

//---------------------------------------------------------------------------------------------------------------
// @name                                : AddPersonToList
//
// @description                         : Adds the person to its population list as per gender.
//
// @returns                             : Nothing
//---------------------------------------------------------------------------------------------------------------
void AddPersonToList(Person *person)
{
    if (person)
    {
        if (person->GetGender() == MALE)
        {
            g_maleList.push_back(person);
        }
        else
        {
            g_femaleList.push_back(person);
        }
    }
}


//---------------------------------------------------------------------------------------------------------------
// @name                                : ClearLogsForIteration
//
// @description                         : Clears the logs for this iteration of simulation
//
// @returns                             : Nothing
//---------------------------------------------------------------------------------------------------------------
void ClearLogsForIteration()
{
    g_simulationOutputParameters.logs.clear();
}

//---------------------------------------------------------------------------------------------------------------
// @name                                : Log
//
// @description                         : Add a loggging string value to log vector
//
// @returns                             : Nothing
//---------------------------------------------------------------------------------------------------------------
void Log(const std::string & str)
{
    g_simulationOutputParameters.logs.push_back(str);
    std::cout << str << std::endl;
}

//---------------------------------------------------------------------------------------------------------------
// @name                                : Simulate
//
// @description                         : Simulation entry point
//
// @param simulationInputParameters     : Input parameters from the User Interface
//
// @returns                             : Simulation result
//---------------------------------------------------------------------------------------------------------------
outputParameters_t Simulate(const inputParameters_t & simulationInputParameters)
{
    // Update input parameter
    g_simulationInputParameters.yearsToSimulate = simulationInputParameters.yearsToSimulate;
    g_simulationInputParameters.avgFertilityRate = simulationInputParameters.avgFertilityRate;    
    g_simulationInputParameters.maxMaleFertilityAge = simulationInputParameters.maxMaleFertilityAge;
    g_simulationInputParameters.maxFemaleFertilityAge = simulationInputParameters.maxFemaleFertilityAge;
    g_simulationInputParameters.minMaleMarriageAge = simulationInputParameters.minMaleMarriageAge;
    g_simulationInputParameters.minFemaleMarriageAge = simulationInputParameters.minFemaleMarriageAge;
    g_simulationInputParameters.avgLifeExpectancy = simulationInputParameters.avgLifeExpectancy;

    // Clear logs for this iteration
    ClearLogsForIteration();

    for (int i = 0; i < g_simulationInputParameters.yearsToSimulate; i++)
    {
        // Execute only if simulation is not complete
        if (!g_simulationOutputParameters.isSimulationComplete)
        {
            SimulateYear();
        }
    }

    return g_simulationOutputParameters;
}

//---------------------------------------------------------------------------------------------------------------
// @name                                : SimulateYear
//
// @description                         : Simulates a year.
//
//@returns                              : Nothing
//---------------------------------------------------------------------------------------------------------------
void SimulateYear()
{
    // Display year info
    std::ostringstream sstream;
    std::string identifier = "\n--- Year ";
    sstream << identifier << g_simulationOutputParameters.totalYears + 1 << " ---";
    std::string yearId = sstream.str();
    Log(yearId);

    // In the beginning create Adam and Eve (1st couple)
    if (g_simulationOutputParameters.totalYears == 0)
    {
        Person *adam = CreatePerson(MALE);
        AddPersonToList(adam);

        Person *eve = CreatePerson(FEMALE);
        AddPersonToList(eve);
    }

    // Check if sustenance population exists
    if (g_simulationOutputParameters.totalMalePopulation == 0 &&
        g_simulationOutputParameters.totalFemalePopulation == 0)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Simulation complete");
        msgBox.setInformativeText("Entire population wiped out!");
        msgBox.setStandardButtons(QMessageBox::Ok);
        g_simulationOutputParameters.isSimulationComplete = true;
        msgBox.exec();
    }
    else
    {
        // Updates for this year
        if (g_simulationOutputParameters.totalMalePopulation)
        {
            UpdatePersonList(MALE);
        }

        if (g_simulationOutputParameters.totalFemalePopulation)
        {
            UpdatePersonList(FEMALE);
        }
    }

    g_simulationOutputParameters.totalYears++;
}


//---------------------------------------------------------------------------------------------------------------
// @name                                : CheckDeath
//
// @description                         : Check if person dies
//
// @returns                             : true/false
//---------------------------------------------------------------------------------------------------------------
bool CheckDeath(Person *person)
{
    int ageMargin = GetRandomNumber(2 * MARGIN_YEARS_PROBABILITY);
    if (person->GetAge() + ageMargin > g_simulationInputParameters.avgLifeExpectancy - MARGIN_YEARS_PROBABILITY)
    {
        std::string msg(person->GetName() + " died at " + std::to_string(person->GetAge()) + " years.");
        Log(msg);
        return true;
    }

    return false;
}

//---------------------------------------------------------------------------------------------------------------
// @name                                : CheckMarriage
//
// @description                         : Check if person marries. Updates spouse details.
//
// @returns                             : true/false
//---------------------------------------------------------------------------------------------------------------
bool CheckMarriage(Person *person)
{
    if (person->GetSpouse() != nullptr)
    {
        // Already married
        return false;
    }

    Gender_t gender = person->GetGender();
    int minMarriageAge = (gender == MALE) ? g_simulationInputParameters.minMaleMarriageAge : g_simulationInputParameters.minFemaleMarriageAge;
    if (person->GetAge() > minMarriageAge)
    {
        // Choose a suitor
        Gender_t partnerGender = (gender == MALE) ? FEMALE : MALE;
        std::vector<Person*> partnerList = (partnerGender == MALE) ? g_maleList : g_femaleList;
        for (auto it = partnerList.begin(); it < partnerList.end(); it++)
        {
            Person *partner = *it;
            int minMarriageAgePartner = (partnerGender == MALE) ? g_simulationInputParameters.minMaleMarriageAge : g_simulationInputParameters.minFemaleMarriageAge;
            if (partner->GetAge() > minMarriageAgePartner &&
                partner->GetSpouse() == nullptr)
            {
                // Found the suitor, update spouse details
                person->SetSpouse(partner);
                partner->SetSpouse(person);

                // Add logging
                std::string msg(person->GetName() + " (age " + std::to_string(person->GetAge()) + ") and " + partner->GetName() + " (age " + std::to_string(partner->GetAge()) + ") " + " married.");
                Log(msg);
                return true;
            }
        }
    }

    return false;
}

//---------------------------------------------------------------------------------------------------------------
// @name                                : MakeBaby
//
// @description                         : Create a new person
//
// @returns                             : Pointer to baby (Person)
//---------------------------------------------------------------------------------------------------------------
Person* MakeBaby(Person* parent)
{
    Person *baby = nullptr;

    if (parent == nullptr || parent->GetSpouse() == nullptr)
    {
        return baby;
    }

    std::vector<Person*> currentChildren = parent->GetChildren();
    if (static_cast<int>(currentChildren.size()) <= g_simulationInputParameters.avgFertilityRate)
    {
        // 50-50 probability of making a baby this year
        int makeBabyThisYearProbability = GetRandomNumber(100);
        if (makeBabyThisYearProbability > 50)
        {
            // 50-50 probabilty of boy or girl
            int genderProbability = GetRandomNumber(100);
            Gender_t babyGender = (genderProbability > 50) ? MALE : FEMALE;
            baby = CreatePerson(babyGender);

            // Update couple details if baby was created
            if (baby)
            {
                parent->AddChild(baby);

                Person *spouse = parent->GetSpouse();
                spouse->AddChild(baby);

                std::string msg = parent->GetName() + " and " + spouse->GetName() + " were blessed with baby " + baby->GetName();
                Log(msg);
            }
        }
    }

    return baby;
}

//---------------------------------------------------------------------------------------------------------------
// @name                                : CheckChildren
//
// @description                         : Check if the couple had any children this year. Updates childrens' details.
//
// @returns                             : Pointer to baby if created, nullptr otherwise
//---------------------------------------------------------------------------------------------------------------
Person* CheckChildren(Person *person)
{
    Person *baby = nullptr;

    if (person == nullptr || person->GetSpouse() == nullptr)
    {
        return baby;
    }

    Gender_t gender = person->GetGender();

    // If there is atleast 1 year gap between married age and current age AND
    // There should be a gap of atleast NEXT_CHILD_GAP years.
    if (person->GetMarriedAge() < person->GetAge() &&
        person->GetLastChildAt() + NEXT_CHILD_GAP < person->GetAge())
    {
        Person *spouse = person->GetSpouse();
        if (spouse)
        {
            if (gender == MALE)
            {
                if (person->GetAge() < g_simulationInputParameters.maxMaleFertilityAge &&
                    spouse->GetAge() < g_simulationInputParameters.maxFemaleFertilityAge)
                {
                    // Reproduction possible
                    baby = MakeBaby(person);
                }
            }
            else
            {
                if (person->GetAge() < g_simulationInputParameters.maxFemaleFertilityAge &&
                    spouse->GetAge() < g_simulationInputParameters.maxMaleFertilityAge)
                {
                    // Reproduction possible
                    baby = MakeBaby(person);
                }
            }
        }
    }

    return baby;
}

//---------------------------------------------------------------------------------------------------------------
// @name                                : UpdatePersonList
//
// @description                         : Simulates the changes in population in 1 year.
//
// @returns                             : Nothing
//---------------------------------------------------------------------------------------------------------------
void UpdatePersonList(Gender_t gender)
{    
    std::vector<Person*> & personList = (gender == MALE) ? g_maleList : g_femaleList;
    for (auto it = personList.begin(); it < personList.end(); /* it++ done in else case */)
    {
        Person *person = *it;

        // Update age of person
        person->Update();

        // Check death probability of this person
        bool bDied = CheckDeath(person);
        if (bDied)
        {
            Person *spouse = person->GetSpouse();
            if (spouse)
            {
                spouse->ClearSpouse();
            }

            delete person;
            person = nullptr;
            it = personList.erase(it);
            g_simulationOutputParameters.totalDeaths++;
        }
        else
        {
            it++;
        }
    } // end of for loop

    // Check marriage and children
    long totalAge = 0;
    std::vector<Person*> babyList; // List of children born this year.
    for (auto it = personList.begin(); it < personList.end(); it++)
    {
        Person *person = *it;

        totalAge += person->GetAge();

        // Marriage probability
        bool bMarriedThisYear = CheckMarriage(person);

        // Making a baby probaility
        Person *baby = CheckChildren(person);
        if (baby)
        {
            babyList.push_back(baby);
        }
    }


    // Add all children born this year to their population list
    for (auto it = babyList.begin(); it < babyList.end(); it++)
    {
        AddPersonToList(*it);
    }

    // Avg age and total population
    double avgAge = personList.size() ? static_cast<double>(totalAge)/ personList.size() : 0.0;
    if (gender == MALE)
    {
        g_simulationOutputParameters.avgMaleAge = avgAge;
        g_simulationOutputParameters.totalMalePopulation = personList.size();
    }
    else
    {
        g_simulationOutputParameters.avgFemaleAge = avgAge;
        g_simulationOutputParameters.totalFemalePopulation = personList.size();
    }

    // Sex ratio
    if (g_simulationOutputParameters.totalMalePopulation != 0)
    {
        g_simulationOutputParameters.sexRatio = (static_cast<double>(g_simulationOutputParameters.totalFemalePopulation) / g_simulationOutputParameters.totalMalePopulation) * 1000;
    }
    else
    {
        g_simulationOutputParameters.sexRatio = 0;
    }
}

//---------------------------------------------------------------------------------------------------------------
// @name                                : DisplayPopulationDetails
//
// @description                         : Displays details of all person in the population list
//
// @returns                             : Nothing
//---------------------------------------------------------------------------------------------------------------
void DisplayPopulationDetails(Gender_t gender)
{
    std::vector<Person*> personList = (gender == MALE) ? g_maleList : g_femaleList;
    std::string genderId = (gender == MALE) ? "Male" : "Female";
    size_t index = 1;
    for (auto it = personList.begin(); it < personList.end(); it++)
    {
        Person *person = *it;
        Person *spouse = person->GetSpouse();
        std::string spouseName = spouse ? spouse->GetName() : "NONE";
        Log("\n" + genderId + "#" + std::to_string(index) + ":");
        Log("Name     : " + person->GetName());
        Log("Age      : " + std::to_string(person->GetAge()));
        Log("Spouse   : " + spouseName);
        Log("Children : " + std::to_string(person->GetChildren().size()));

        index++;
    }
}














