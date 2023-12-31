#ifndef PATIENT_H
#define PATIENT_H

typedef struct Patient
{
    int patientSSN;
    char fname[256], mname[256], lname[256], DOB[256];
    char genderStr[3], street[256], city[256], state[256], country[256];
    char gender;
    struct Patient *next;
} Patient;

Patient *patientIndex[100];

void loadPatientsFromFile(Patient *patientIndex[]);
void savePatientsToFile(Patient *patientIndex[]);
void freePatientList(Patient *patientIndex[]);
void patientTable(Patient *patientIndex[]);

#endif