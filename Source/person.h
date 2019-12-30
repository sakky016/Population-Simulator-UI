#ifndef PERSON_H
#define PERSON_H

#include <string>
#include <vector>

enum Gender_t
{
    MALE,
    FEMALE
};

class Person
{
private:
    size_t m_id;
    std::string m_name;
    Gender_t m_gender;
    int m_age;
    int m_marriedAtAge;
    int m_lastChildAtAge;
    Person *m_spouse;

    std::vector<Person*> m_children;

public:
    Person(size_t id, Gender_t gender);
    ~Person();
    std::string GetName(){return m_name;}
    int GetAge(){return m_age;}
    int GetMarriedAge(){return m_marriedAtAge;}
    int GetLastChildAt(){return m_lastChildAtAge;}
    Gender_t GetGender(){return m_gender;}
    Person* GetSpouse(){return m_spouse;}
    void SetSpouse(Person* spouse);
    void ClearSpouse(){m_spouse = nullptr;}
    std::vector<Person*> GetChildren(){return m_children;}
    void AddChild(Person *child);

    void Update();
};

#endif // PERSON_H
