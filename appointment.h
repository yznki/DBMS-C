#ifndef APPOINTMENT_H
#define APPOINTMENT_H
#include "doctor.h"
#include "patient.h"

typedef struct Appointment
{
    int ID, doctorID, patientID;
    char date[256];
    struct Appointment *next;
} Appointment;

Appointment *appointmentIndex[100];

void loadAppointmentsFromFile(Appointment *appointmentIndex[]);
void saveAppointmentsToFile(Appointment *appointmentIndex[]);
void freeAppointmentList(Appointment *appointmentIndex[]);
void appointmentTable(Appointment *appointmentIndex[], Doctor *doctorIndex[], Patient *patientIndex[]);

#endif