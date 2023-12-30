#ifndef DOCTOR_H
#define DOCTOR_H

typedef struct Doctor
{
    int doctorID, departmentID;
    char fname[256], mname[256], lname[256], DOB[256];
    char genderStr[3], phoneNumber[11], street[256], city[256], state[256], country[256];
    char gender;
    struct Doctor *next;
} Doctor;

Doctor *doctorIndex[100];

void loadDoctorsFromFile(Doctor *doctorIndex[]);
void saveDoctorsToFile(Doctor *doctorIndex[]);
void freeDoctorList(Doctor *doctorIndex[]);
void doctorTable(Doctor *doctorIndex[]);

#endif