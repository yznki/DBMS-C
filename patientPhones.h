#ifndef PATIENTPHONES_H
#define PATIENTPHONES_H
#include "patient.h"

typedef struct PatientPhones
{
    int patientSSN;
    char phoneNumber[15];
    struct PatientPhones *next;

} PatientPhones;

PatientPhones *patientPhonesIndex[100];

void loadPatientPhonesFromFile(PatientPhones *patientPhonesIndex[]);
void savePatientPhonesToFile(PatientPhones *patientPhonesIndex[]);
void freePatientPhonesList(PatientPhones *patientPhonesIndex[]);
void patientPhonesTable(PatientPhones *patientPhonesIndex[], Patient *patientIndex[]);

#endif