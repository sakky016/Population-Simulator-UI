#include "person.h"
#include "simulation.h"
#include <sstream>

Person::Person(size_t id, Gender_t gender)
{
    m_id = id;
    m_gender = gender;
    m_age = 0;
    m_spouse = nullptr;         // no spouse
    m_lastChildAtAge = -1;      // no child
    m_marriedAtAge = -1;        // unmarried

    std::ostringstream sstream;
    std::string identifier = (gender == MALE) ? "M_" : "F_";
    sstream << identifier << m_id;
    m_name = sstream.str();

    m_children.reserve(5);
}

Person::~Person()
{
    // DO NOT delete this address. Simply remove the reference
    m_spouse = nullptr;

    m_children.clear();
}

void Person::SetSpouse(Person* spouse)
{
    m_spouse = spouse;
    m_marriedAtAge = m_age;
}

void Person::AddChild(Person *child)
{
    m_children.push_back(child);
    m_lastChildAtAge = m_age;
}

void Person::Update()
{
    m_age++;   
}
