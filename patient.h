#ifndef PATIENT_H
#define PATIENT_H

struct PatientPhones;
struct Appointment;

typedef struct Patient
{
    int patientSSN;
    char fname[512], mname[512], lname[512], DOB[512];
    char genderStr[3], street[512], city[512], state[512], country[512];
    char gender;
    struct Patient *next;
} Patient;

Patient *patientIndex[100];

void loadPatientsFromFile(Patient *patientIndex[]);
void savePatientsToFile(Patient *patientIndex[]);
void freePatientList(Patient *patientIndex[]);
void patientTable(Patient *patientIndex[], struct PatientPhones *patientPhonesIndex[], struct Appointment *appointmentIndex[]);

#endif