#ifndef APPOINTMENT_H
#define APPOINTMENT_H

typedef struct Appointment
{
    
    struct Appointment *next;
} Appointment;

Appointment *appointmentIndex[100];

void loadAppointmentsFromFile(Appointment *appointmentIndex[]);
void saveAppointmentsToFile(Appointment *appointmentIndex[]);
void freeAppointmentList(Appointment *appointmentIndex[]);
void appointmentTable(Appointment *appointmentIndex[]);

#endif