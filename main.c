#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "patient.h"
#include "doctor.h"
#include "appointment.h"

int main(int argc, char const *argv[])
{
    // system("clear");
    loadPatientsFromFile(patientIndex);
    loadDoctorsFromFile(doctorIndex);
    loadAppointmentsFromFile(appointmentIndex);

    int choice = 0;
    printf("Welcome to the Jordanian Hospital\n");

    while (choice != 6)
    {
        printf("Choose a table to work with:\n");
        printf("1- Patient\n");
        printf("2- Doctor\n");
        printf("3- Appointment\n");
        printf("4- Bill\n");
        printf("5- PatientPhones\n");
        printf("6- Exit\n");

        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            system("clear");
            patientTable(patientIndex);
            system("clear");
            savePatientsToFile(patientIndex);
            break;
        case 2:
            system("clear");
            doctorTable(doctorIndex);
            system("clear");
            saveDoctorsToFile(doctorIndex);
            break;
        case 3:
            system("clear");
            appointmentTable(appointmentIndex, doctorIndex, patientIndex);
            system("clear");
            saveAppointmentsToFile(appointmentIndex);
            break;
        case 4:
            system("clear");
            // billTable();
            system("clear");
            break;
        case 5:
            system("clear");
            // diagnosisTable();
            system("clear");
            break;
        case 6:
            system("clear");
            printf("\n\n\n Goodbye! \n\n\n");
            freeDoctorList(doctorIndex);
            freePatientList(patientIndex);
            freeAppointmentList(appointmentIndex);
            break;
        default:
            system("clear");
            printf("Invalid Input\n\n\n");
            break;
        }
    }

    return 0;
}