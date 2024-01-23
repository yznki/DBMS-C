#ifndef DOCTOR_H
#define DOCTOR_H

struct Appointment;

typedef struct Doctor
{
    int doctorID, departmentID;
    char fname[512], mname[512], lname[512], DOB[512];
    char genderStr[3], phoneNumber[15], street[512], city[512], state[512], country[512];
    char gender;
    struct Doctor *next;
} Doctor;

Doctor *doctorIndex[100];

void loadDoctorsFromFile(Doctor *doctorIndex[]);
void saveDoctorsToFile(Doctor *doctorIndex[]);
void freeDoctorList(Doctor *doctorIndex[]);
void doctorTable(Doctor *doctorIndex[], struct Appointment *appointmentIndex[]);

#endif