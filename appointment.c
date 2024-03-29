#include "appointment.h"
#include "patient.h"
#include "doctor.h"
#include "bill.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>

void loadAppointmentsFromFile(Appointment *appointmentIndex[])
{
    FILE *file = fopen("appointments.csv", "r");
    char line[2048];
    if (file == NULL)
    {
        perror("Error opening file");
        return;
    }

    // Read and discard the first line
    if (fgets(line, sizeof(line), file) == NULL)
    {
        perror("Error reading file");
        fclose(file);
        return;
    }

    while (fgets(line, sizeof(line), file))
    {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n')
        {
            line[len - 1] = '\0';
        }

        Appointment *newAppointment = (Appointment *)malloc(sizeof(Appointment));
        if (newAppointment == NULL)
        {
            perror("Error allocating memory");
            continue;
        }

        *newAppointment = (const Appointment){0};

        char *token = strtok(line, ",");
        int field = 0;

        while (token != NULL && field < 4)
        {
            switch (field)
            {
            case 0:
                newAppointment->ID = atoi(token);
                break;
            case 1:
                newAppointment->doctorID = atoi(token);
                break;
            case 2:
                newAppointment->patientID = atoi(token);
                break;
            case 3:
                strncpy(newAppointment->date, token, sizeof(newAppointment->date) - 1);
                newAppointment->date[sizeof(newAppointment->date) - 1] = '\0';
                break;
            }
            token = strtok(NULL, ",");
            field++;
        }

        int index = getIndex(newAppointment->ID);

        if (appointmentIndex[index] == NULL || appointmentIndex[index]->ID > newAppointment->ID)
        {
            newAppointment->next = appointmentIndex[index];
            appointmentIndex[index] = newAppointment;
        }
        else
        {
            Appointment *current = appointmentIndex[index];
            while (current->next != NULL && current->next->ID < newAppointment->ID)
            {
                current = current->next;
            }
            newAppointment->next = current->next;
            current->next = newAppointment;
        }
    }

    fclose(file);
}

void saveAppointmentsToFile(Appointment *appointmentIndex[])
{
    FILE *file = fopen("appointments.csv", "w");
    if (file == NULL)
    {
        perror("Error opening file");
        return;
    }

    fprintf(file, "Appointment ID,Doctor ID,Patient SSN,Appointment Date\n");

    for (int i = 0; i < INDEX_SIZE; i++)
    {
        Appointment *current = appointmentIndex[i];
        while (current != NULL)
        {
            fprintf(file, "%d,%d,%d,%s\n",
                    current->ID, current->doctorID, current->patientID, current->date);
            current = current->next;
        }
    }

    fclose(file);
}

void freeAppointmentList(Appointment *appointmentIndex[])
{
    for (int i = 0; i < INDEX_SIZE; i++)
    {
        Appointment *current = appointmentIndex[i];
        while (current != NULL)
        {
            Appointment *temp = current;
            current = current->next;
            free(temp);
        }
        appointmentIndex[i] = NULL;
    }
}

bool existsInDoctorTable(int doctorID, Doctor *doctorIndex[])
{
    int index = getIndex(doctorID);
    Doctor *current = doctorIndex[index];
    while (current != NULL)
    {
        if (current->doctorID == doctorID)
        {
            return true;
        }
        current = current->next;
    }
    return false;
}

bool existsInPatientTable(int patientID, Patient *patientIndex[])
{
    int index = getIndex(patientID);
    Patient *current = patientIndex[index];
    while (current != NULL)
    {
        if (current->patientSSN == patientID)
        {
            return true;
        }
        current = current->next;
    }
    return false;
}

char *doctorName(int id)
{
    int index = getIndex(id);
    Doctor *current = doctorIndex[index];

    while (current != NULL)
    {
        if (current->doctorID == id)
        {
            char *name = malloc(512 * sizeof(char));
            if (name == NULL)
            {
                return NULL;
            }
            snprintf(name, 512, "%s %s", current->fname, current->lname);
            return name;
        }
        current = current->next;
    }
    return NULL;
}

char *patientName(int ssn)
{
    int index = getIndex(ssn);
    Patient *current = patientIndex[index];

    while (current != NULL)
    {
        if (current->patientSSN == ssn)
        {
            char *name = malloc(512 * sizeof(char));
            if (name == NULL)
            {
                return NULL;
            }
            snprintf(name, 512, "%s %s", current->fname, current->lname);
            return name;
        }
        current = current->next;
    }
    return NULL;
}

void *printAppointmentInfo(void *arg)
{
    int *range = (int *)arg;
    int start = range[0];
    int end = range[1];

    for (int i = start; i < end; i++)
    {
        Appointment *current = appointmentIndex[i];
        while (current != NULL)
        {
            char *dName = doctorName(current->doctorID);
            char *pName = patientName(current->patientID);
            printf("%-20d%-32s%-32s%-20s\n", current->ID, dName, pName, current->date);
            free(dName);
            free(pName);
            current = current->next;
        }
    }

    return NULL;
}

void deleteAppointmentFromBills(Bill *billIndex[], int appointmentID) {

    for (int i = 0; i < INDEX_SIZE; i++)
    {
        Bill *current = billIndex[i];
        Bill *previous = NULL;

        while (current != NULL)
        {
            if (current->appointmentID == appointmentID)
            {
                if (previous == NULL)
                {
                    billIndex[i] = current->next;
                }
                else
                {
                    previous->next = current->next;
                }

                Bill *temp = current;
                current = current->next;
                free(temp);
            }
            else
            {
                previous = current;
                current = current->next;
            }
        }
    }
}

void insertAppointment(Appointment newAppointment)
{

    int continueInsertion = 1;
    while (continueInsertion == 1)
    {
        printf("\n\nAnything with '*' is required.\n\n");

        printf("Enter Appointment ID * or 0 to exit: ");
        if (scanf("%d", &newAppointment.ID) != 1)
        {
            printf("\nAppointment ID cannot be empty/zero/character.\nTry Again...\n\n");
            while (getchar() != '\n')
                ;
            continue;
        }

        if (newAppointment.ID == 0)
        {
            break;
        }

        int index = getIndex(newAppointment.ID);
        Appointment *current = appointmentIndex[index];
        int idAlreadyExists = 0;

        while (current != NULL)
        {
            if (current->ID == newAppointment.ID)
            {
                idAlreadyExists = 1;
                break;
            }
            current = current->next;
        }

        if (idAlreadyExists)
        {
            printf("\nError: An appointment with ID %d already exists.\nTry Again...\n\n", newAppointment.ID);
            continue;
        }

        printf("Enter Doctor ID *: ");
        scanf("%d", &newAppointment.doctorID);
        while (getchar() != '\n')
            ;
        if (!existsInDoctorTable(newAppointment.doctorID, doctorIndex))
        {
            printf("\nError: Doctor ID does not exist.\nTry Again...\n\n");
            continue;
        }

        printf("Enter Patient ID *: ");
        scanf("%d", &newAppointment.patientID);
        while (getchar() != '\n')
            ;
        if (!existsInPatientTable(newAppointment.patientID, patientIndex))
        {
            printf("\nError: Patient ID does not exist.\nTry Again...\n\n");
            continue;
        }

        printf("Enter Date (DD-MM-YYYY) *: ");
        fgets(newAppointment.date, sizeof(newAppointment.date), stdin);
        newAppointment.date[strcspn(newAppointment.date, "\n")] = '\0';
        if (strlen(newAppointment.date) == 0)
        {
            printf("\nDate cannot be empty.\nTry Again...\n\n");
            continue;
        }

        Appointment *newNode = (Appointment *)malloc(sizeof(Appointment));
        if (newNode == NULL)
        {
            perror("Error allocating memory");
            continue;
        }
        *newNode = newAppointment;
        newNode->next = appointmentIndex[index];
        appointmentIndex[index] = newNode;

        printf("\nNew appointment added successfully.\n");

        printf("1- Insert another appointment.\n");
        printf("2- Return to main menu.\n");
        scanf("%d", &continueInsertion);
        while (getchar() != '\n')
            ;
        system("clear");
    }
    if (continueInsertion == 2)
    {
        system("clear");
    }
}

void deleteAppointment()
{
    int appointmentID, deletion = 1;
    while (deletion != 2)
    {
        if (deletion == 1)
        {
            printf("Enter ID of Appointment to delete or 0 to exit: ");
            scanf("%d", &appointmentID);

            if (appointmentID == 0)
            {
                break;
            }

            int index = getIndex(appointmentID);
            Appointment *current = appointmentIndex[index];
            Appointment *prev = NULL;

            while (current != NULL)
            {
                if (current->ID == appointmentID)
                {
                    if (prev == NULL)
                    {
                        appointmentIndex[index] = current->next;
                    }
                    else
                    {
                        prev->next = current->next;
                    }
                    free(current);
                    printf("\n\nAppointment with ID %d deleted.\n\n", appointmentID);
                    deleteAppointmentFromBills(billIndex, appointmentID);
                    break;
                }
                prev = current;
                current = current->next;
            }

            if (current == NULL)
            {
                printf("\n\nAppointment with ID %d not found.\n\n", appointmentID);
            }
        }
        else
        {
            printf("Invalid choice\n\n");
        }

        printf("1- Delete another record.\n");
        printf("2- Return to main menu.\n");
        scanf("%d", &deletion);
    }
    system("clear");
}

void selectAppointments()
{
    printf("%-20s%-32s%-32s%-20s\n", "Appointment ID", "Doctor ID", "Patient ID", "Date");
    pthread_t threads[2];
    int ranges[4] = {0, INDEX_SIZE / 2, INDEX_SIZE / 2, INDEX_SIZE};

    pthread_create(&threads[0], NULL, printAppointmentInfo, &ranges[0]);
    pthread_create(&threads[1], NULL, printAppointmentInfo, &ranges[2]);

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
}

void searchAppointment()
{

    int filterChoice, searchID;
    printf("According to which attribute?\n");
    printf("1- Appointment ID\n");
    printf("2- Doctor ID\n");
    printf("3- Patient ID\n");

    scanf("%d", &filterChoice);
    printf("\n\n\n");

    int found = 0;

    switch (filterChoice)
    {
    case 1:
        printf("Enter Appointment ID: ");
        scanf("%d", &searchID);
        printf("%-20s%-32s%-32s%-20s\n", "Appointment ID", "Doctor ID", "Patient ID", "Date");
        for (int i = 0; i < INDEX_SIZE; i++)
        {
            Appointment *current = appointmentIndex[i];
            while (current != NULL)
            {
                if (current->ID == searchID)
                {
                    char *dName = doctorName(current->doctorID);
                    char *pName = patientName(current->patientID);
                    printf("%-20d%-32s%-32s%-20s\n", current->ID, dName, pName, current->date);
                    free(dName);
                    free(pName);
                    found = 1;
                }
                current = current->next;
            }
        }
        if (!found)
        {
            printf("\nNo appointments found with Appointment ID %d.\n", searchID);
        }
        break;
    case 2:
        printf("Enter Doctor ID: ");
        scanf("%d", &searchID);
        printf("%-20s%-32s%-32s%-20s\n", "Appointment ID", "Doctor ID", "Patient ID", "Date");
        for (int i = 0; i < INDEX_SIZE; i++)
        {
            Appointment *current = appointmentIndex[i];
            while (current != NULL)
            {
                if (current->doctorID == searchID)
                {
                    char *dName = doctorName(current->doctorID);
                    char *pName = patientName(current->patientID);
                    printf("%-20d%-32s%-32s%-20s\n", current->ID, dName, pName, current->date);
                    free(dName);
                    free(pName);
                    found = 1;
                }
                current = current->next;
            }
        }
        if (!found)
        {
            printf("\nNo appointments found for Doctor ID %d.\n", searchID);
        }
        break;
    case 3:
        printf("Enter Patient ID: ");
        scanf("%d", &searchID);
        printf("%-20s%-32s%-32s%-20s\n", "Appointment ID", "Doctor ID", "Patient ID", "Date");
        for (int i = 0; i < INDEX_SIZE; i++)
        {
            Appointment *current = appointmentIndex[i];
            while (current != NULL)
            {
                if (current->patientID == searchID)
                {
                    char *dName = doctorName(current->doctorID);
                    char *pName = patientName(current->patientID);
                    printf("%-20d%-32s%-32s%-20s\n", current->ID, dName, pName, current->date);
                    free(dName);
                    free(pName);
                    found = 1;
                }
                current = current->next;
            }
        }
        if (!found)
        {
            printf("\nNo appointments found for Patient ID %d.\n", searchID);
        }
        break;
    default:
        printf("Invalid choice.\n");
        break;
    }
}

void updateAppointment()
{

    int appointmentID;
    printf("Enter Appointment ID to update or 0 to exit: ");
    scanf("%d", &appointmentID);

    if (appointmentID == 0)
    {
        return;
    }

    int index = getIndex(appointmentID);
    Appointment *current = appointmentIndex[index];
    while (current != NULL && current->ID != appointmentID)
    {
        current = current->next;
    }

    if (current == NULL)
    {
        printf("No record found for Appointment ID %d.\n", appointmentID);
    }
    else
    {
        while (getchar() != '\n')
            ;
        printf("Current Date: %s\n", current->date);
        printf("Enter new Date (DD-MM-YYYY): ");

        char tempDate[11];
        int isValidDate = 0;

        while (!isValidDate)
        {
            if (fgets(tempDate, sizeof(tempDate), stdin))
            {
                tempDate[strcspn(tempDate, "\n")] = 0;

                if (strlen(tempDate) == 10 && tempDate[2] == '-' && tempDate[5] == '-')
                {
                    isValidDate = 1;
                }
                else
                {
                    printf("Invalid format.\n\nPlease enter the date in DD-MM-YYYY format: ");
                }
            }
            else
            {
                while (getchar() != '\n')
                    ;
                printf("Input too long.\n\nPlease enter the date in DD-MM-YYYY format: ");
            }
        }

        strcpy(current->date, tempDate);
        printf("Date updated to %s for Appointment ID %d.\n", current->date, appointmentID);
    }

    printf("\n\nEnter any key to continue\n\n");
    getchar();
    getchar();
}

void appointmentTable(Appointment *appointmentIndex[], Doctor *doctorIndex[], Patient *patientIndex[])
{

    Appointment newAppointment;
    int choice = 0;

    while (choice != 5)
    {
        system("clear");
        printf("1- Insert new Appointment(s)\n");
        printf("2- Delete Appointment(s)\n");
        printf("3- Select Appointment(s)\n");
        printf("4- Update Appointment(s)\n");
        printf("5- Exit\n");

        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            insertAppointment(newAppointment);
            break;

        case 2:
            deleteAppointment();
            break;

        case 3:;
            int selection;
            printf("Select with a condition or all?\n");
            printf("1- All\n");
            printf("2- Condition\n");

            scanf("%d", &selection);
            printf("\n\n\n");

            if (selection == 1)
            {
                selectAppointments();
            }
            else if (selection == 2)
            {
                searchAppointment();
            }
            else
            {
                printf("Invalid Input.\n");
            }

            printf("\n\nEnter any key to continue\n\n");
            getchar();
            getchar();

            break;

        case 4:
            updateAppointment();
            break;

        default:
            break;
        }
    }
}
