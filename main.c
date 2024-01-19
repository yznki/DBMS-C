#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "patient.h"
#include "doctor.h"
#include "appointment.h"
#include "bill.h"
#include "patientPhones.h"

void *loadThread(void *arg)
{
    int table = *(int *) arg;

    switch (table)
    {
    case 1:
        loadPatientsFromFile(patientIndex);
        break;
    case 2:
        loadDoctorsFromFile(doctorIndex);
        break;
    case 3:
        loadAppointmentsFromFile(appointmentIndex);
        break;
    case 4:
        loadBillsFromFile(billIndex);
        break;
    case 5:
        loadPatientPhonesFromFile(patientPhonesIndex);
        break;
    default:
        fprintf(stderr, "table\n");
    }

    return NULL;
}

void *freeThread(void *arg) {
    int table = *(int *)arg;

    switch (table)
    {
    case 1:
        freePatientList(patientIndex);
        break;
    case 2:
        freeDoctorList(doctorIndex);
        break;
    case 3:
        freeAppointmentList(appointmentIndex);
        break;
    case 4:
        freeBillList(billIndex);
        break;
    case 5:
        freePatientPhonesList(patientPhonesIndex);
        break;
    default:
        fprintf(stderr, "Invalid table\n");
    }

    return NULL;
}

int main(int argc, char const *argv[])
{
    system("clear");
    pthread_t threads[5];
    int tables[5] = {1, 2, 3, 4, 5};

    for (int i = 0; i < 5; i++)
    {
        pthread_create(&threads[i], NULL, loadThread, &tables[i]);
    }

    for (int i = 0; i < 5; i++)
    {
        pthread_join(threads[i], NULL);
    }

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
            billTable(billIndex, appointmentIndex);
            system("clear");
            saveBillsToFile(billIndex);
            break;
        case 5:
            system("clear");
            patientPhonesTable(patientPhonesIndex, patientIndex);
            system("clear");
            savePatientPhonesToFile(patientPhonesIndex);
            break;
        case 6:
            system("clear");
            printf("\n\n\n Goodbye! \n\n\n");
            for (int i = 0; i < 5; i++)
            {
                pthread_create(&threads[i], NULL, freeThread, &tables[i]);
            }

            for (int i = 0; i < 5; i++)
            {
                pthread_join(threads[i], NULL);
            }
            break;
        default:
            system("clear");
            printf("Invalid Input\n\n\n");
            break;
        }
    }

    return 0;
}