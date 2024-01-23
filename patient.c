#include "patient.h"
#include "utils.h"
#include "appointment.h"
#include "patientPhones.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

void loadPatientsFromFile(Patient *patientIndex[])
{
    FILE *file = fopen("patients.csv", "r");
    char line[1024];
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
        Patient *newPatient = (Patient *)malloc(sizeof(Patient));
        if (newPatient == NULL)
        {
            perror("Error allocating memory");
            continue;
        }

        *newPatient = (const Patient){0}; 

        char *ptr = line;
        int field = 0;

        while (field < 10 && *ptr)
        {
            char *start = ptr;
            while (*ptr && *ptr != ',' && *ptr != '\n')
                ptr++;

            size_t len = ptr - start;
            if (len > 0)
            {
                char temp[256] = {0};
                strncpy(temp, start, len < sizeof(temp) ? len : sizeof(temp) - 1);

                switch (field)
                {
                case 0:
                    newPatient->patientSSN = atoi(temp);
                    break;
                case 1:
                    strncpy(newPatient->fname, temp, sizeof(newPatient->fname));
                    break;
                case 2:
                    strncpy(newPatient->mname, temp, sizeof(newPatient->mname));
                    break;
                case 3:
                    strncpy(newPatient->lname, temp, sizeof(newPatient->lname));
                    break;
                case 4:
                    strncpy(newPatient->DOB, temp, sizeof(newPatient->DOB));
                    break;
                case 5:
                    newPatient->gender = temp[0];
                    break;
                case 6:
                    strncpy(newPatient->street, temp, sizeof(newPatient->street));
                    break;
                case 7:
                    strncpy(newPatient->city, temp, sizeof(newPatient->city));
                    break;
                case 8:
                    strncpy(newPatient->state, temp, sizeof(newPatient->state));
                    break;
                case 9:
                    strncpy(newPatient->country, temp, sizeof(newPatient->country));
                    break;
                }
            }

            field++;
            if (*ptr)
                ptr++;
        }

        int index = getIndex(newPatient->patientSSN);

        if (patientIndex[index] == NULL || patientIndex[index]->patientSSN > newPatient->patientSSN)
        {
            newPatient->next = patientIndex[index];
            patientIndex[index] = newPatient;
        }
        else
        {
            Patient *current = patientIndex[index];
            while (current->next != NULL && current->next->patientSSN < newPatient->patientSSN)
            {
                current = current->next;
            }
            newPatient->next = current->next;
            current->next = newPatient;
        }
    }

    fclose(file);
}

void savePatientsToFile(Patient *patientIndex[])
{
    FILE *file = fopen("patients.csv", "w");
    if (file == NULL)
    {
        perror("Error opening file");
        return;
    }

    fprintf(file, "PatientSSN, First Name, Middle Name, Last Name, Date of Birth, Gender, Street, City, State, Country\n");

    for (int i = 0; i < INDEX_SIZE; i++)
    {
        Patient *current = patientIndex[i];
        while (current != NULL)
        {
            fprintf(file, "%d,%s,%s,%s,%s,%c,%s,%s,%s,%s\n",
                    current->patientSSN, current->fname, current->mname, current->lname, current->DOB,
                    current->gender, current->street, current->city,
                    current->state, current->country);
            current = current->next;
        }
    }

    fclose(file);
}

void freePatientList(Patient *patientIndex[])
{
    for (int i = 0; i < INDEX_SIZE; i++)
    {
        Patient *current = patientIndex[i];
        while (current != NULL)
        {
            Patient *temp = current;
            current = current->next;
            free(temp);
        }
        patientIndex[i] = NULL;
    }
}

void *printPatientInfo(void *arg)
{
    int *range = (int *)arg;
    int start = range[0];
    int end = range[1];

    for (int i = start; i < end; i++)
    {
        Patient *current = patientIndex[i];
        while (current != NULL)
        {
            char fullName[512], address[512];
            if (strcmp(current->mname, "") == 0)
                sprintf(fullName, "%s %s", current->fname, current->lname);
            else
                sprintf(fullName, "%s %s %s", current->fname, current->mname, current->lname);

            sprintf(address, "%s %s %s %s", current->street, current->city, current->state, current->country);

            printf("%-15d%-50s%-15s%-10c%-50s\n",
                   current->patientSSN, fullName, current->DOB, current->gender, address);
            current = current->next;
        }
    }

    return NULL;
}

void deletePatientFromAppointments(Appointment *appointmentIndex[], int ssn)
{

    for (int i = 0; i < INDEX_SIZE; i++)
    {
        Appointment *current = appointmentIndex[i];
        Appointment *previous = NULL;

        while (current != NULL)
        {
            if (current->patientID == ssn)
            {
                if (previous == NULL)
                {
                    appointmentIndex[i] = current->next;
                }
                else
                {
                    previous->next = current->next;
                }

                Appointment *temp = current;
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

void deleteFromPatientPhones(PatientPhones *patientPhonesIndex[], int ssn)
{

    for (int i = 0; i < INDEX_SIZE; i++)
    {
        PatientPhones *current = patientPhonesIndex[i];
        PatientPhones *previous = NULL;

        while (current != NULL)
        {
            if (current->patientSSN == ssn)
            {
                if (previous == NULL)
                {
                    patientPhonesIndex[i] = current->next;
                }
                else
                {
                    previous->next = current->next;
                }

                PatientPhones *temp = current;
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

void insertPatient(Patient patientRecord)
{
    int continueInsertion = 1;
    while (continueInsertion == 1)
    {
        printf("\n\nAnything with '*' is required.\n\n");
        printf("Enter patientSSN or 0 to exit *: ");
        if (scanf("%d", &patientRecord.patientSSN) != 1)
        {

            printf("\nSSN cannot be empty/zero/character.\nTry Again...\n\n");
            while (getchar() != '\n')
                ;
            continue;
        }

        if (patientRecord.patientSSN == 0)
        {
            break;
        }

        int index = getIndex(patientRecord.patientSSN);
        Patient *current = patientIndex[index];
        int ssnAlreadyExists = 0;

        while (current != NULL)
        {
            if (current->patientSSN == patientRecord.patientSSN)
            {
                ssnAlreadyExists = 1;
                break;
            }
            current = current->next;
        }

        if (ssnAlreadyExists)
        {
            printf("\n\nError: A patient with SSN %d already exists.\n\n", patientRecord.patientSSN);
            continue;
        }

        while (getchar() != '\n')
            ;

        printf("Enter First Name *: ");
        fgets(patientRecord.fname, sizeof(patientRecord.fname), stdin);
        patientRecord.fname[strcspn(patientRecord.fname, "\n")] = 0;
        if (strlen(patientRecord.fname) == 0)
        {
            printf("\nFirst name cannot be empty.\nTry Again...\n\n");
            continue;
        }

        printf("Enter Middle Name: ");
        fgets(patientRecord.mname, sizeof(patientRecord.mname), stdin);
        patientRecord.mname[strcspn(patientRecord.mname, "\n")] = 0;

        printf("Enter Last Name *: ");
        fgets(patientRecord.lname, sizeof(patientRecord.lname), stdin);
        patientRecord.lname[strcspn(patientRecord.lname, "\n")] = 0;
        if (strlen(patientRecord.lname) == 0)
        {
            printf("\nLast name cannot be empty.\nTry Again...\n\n");
            continue;
        }

        printf("Enter Date of Birth (DD-MM-YYYY) *: ");
        fgets(patientRecord.DOB, sizeof(patientRecord.DOB), stdin);
        patientRecord.DOB[strcspn(patientRecord.DOB, "\n")] = 0;
        if (strlen(patientRecord.DOB) == 0)
        {
            printf("\nDate of Birth cannot be empty.\nTry Again...\n\n");
            continue;
        }

        do
        {
            printf("Enter Gender (M or F) *: ");
            fgets(patientRecord.genderStr, sizeof(patientRecord.genderStr), stdin);
            patientRecord.gender = patientRecord.genderStr[0];
            if (patientRecord.gender != 'F' && patientRecord.gender != 'M')
            {
                printf("\nPlease choose M or F.\n");
            }
        } while (patientRecord.gender != 'M' && patientRecord.gender != 'F');

        printf("Enter Street Name: ");
        fgets(patientRecord.street, sizeof(patientRecord.street), stdin);
        patientRecord.street[strcspn(patientRecord.street, "\n")] = 0;

        printf("Enter City: ");
        fgets(patientRecord.city, sizeof(patientRecord.city), stdin);
        patientRecord.city[strcspn(patientRecord.city, "\n")] = 0;

        printf("Enter State: ");
        fgets(patientRecord.state, sizeof(patientRecord.state), stdin);
        patientRecord.state[strcspn(patientRecord.state, "\n")] = 0;

        printf("Enter Country: ");
        fgets(patientRecord.country, sizeof(patientRecord.country), stdin);
        patientRecord.country[strcspn(patientRecord.country, "\n")] = 0;

        Patient *newNode = (Patient *)malloc(sizeof(Patient));
        if (newNode == NULL)
        {
            perror("Error allocating memory");
            continue;
        }

        *newNode = patientRecord;
        newNode->next = patientIndex[index];
        patientIndex[index] = newNode;

        printf("\n\nThe following is the new record data:\n");
        printf("patientSSN - %d, First Name - %s, Middle Name - %s, Last Name - %s, DOB - %s, Gender - %c, Street - %s, City - %s, State - %s, Country - %s\n\n", patientRecord.patientSSN, patientRecord.fname, patientRecord.mname, patientRecord.lname, patientRecord.DOB, patientRecord.gender, patientRecord.street, patientRecord.city, patientRecord.state, patientRecord.country);

        printf("1- Insert another record.\n");
        printf("2- Return to main menu.\n");

        if (scanf("%d", &continueInsertion) != 1 || (continueInsertion != 1 && continueInsertion != 2))
        {
            printf("Invalid input. Returning to main menu.\n");
            continueInsertion = 0;
        }
        if (continueInsertion == 2)
        {
            continueInsertion = 0;
        }

        system("clear");
    }
    if (continueInsertion == 2)
    {
        system("clear");
    }
}

void deletePatient() {
    int patientID, deletion = 1;
    while (deletion != 2)
    {
        if (deletion == 1)
        {
            printf("Enter SSN of Patient to delete or 0 to exit: ");
            scanf("%d", &patientID);

            if (patientID == 0)
            {
                break;
            }

            int index = getIndex(patientID);
            Patient *current = patientIndex[index];

            if (current != NULL && current->patientSSN == patientID)
            {
                patientIndex[index] = current->next;
                free(current);
                printf("\n\nPatient with SSN %d deleted.\n\n", patientID);
                deletePatientFromAppointments(appointmentIndex, patientID);
                deleteFromPatientPhones(patientPhonesIndex, patientID);
            }
            else
            {

                while (current != NULL && current->next != NULL && current->next->patientSSN != patientID)
                {
                    current = current->next;
                }

                if (current == NULL || current->next == NULL)
                {
                    printf("\n\nPatient with SSN %d not found.\n\n", patientID);
                }
                else
                {

                    Patient *temp = current->next;
                    current->next = temp->next;
                    free(temp);
                    printf("\n\nPatient with SSN %d deleted.\n\n", patientID);
                    deletePatientFromAppointments(appointmentIndex, patientID);
                    deleteFromPatientPhones(patientPhonesIndex, patientID);
                }
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

void selectPatients() {
    printf("%-15s%-50s%-15s%-10s%-50s\n", "Patient SSN", "Name", "DOB", "Gender", "Address");
    pthread_t threads[2];
    int ranges[4] = {0, INDEX_SIZE / 2, INDEX_SIZE / 2, INDEX_SIZE};

    pthread_create(&threads[0], NULL, printPatientInfo, &ranges[0]);
    pthread_create(&threads[1], NULL, printPatientInfo, &ranges[2]);

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
}

void searchPatient() {

    int filterChoice;
    printf("According to which attribute?\n");
    printf("1- SSN\n");
    printf("2- Name\n");

    scanf("%d", &filterChoice);
    printf("\n\n\n");

    switch (filterChoice)
    {
    case 1:
    {
        int patientSSN;
        printf("Enter Patient SSN: ");
        scanf("%d", &patientSSN);
        printf("\n\n\n");

        int index = getIndex(patientSSN);
        Patient *current = patientIndex[index];
        int found = 0;
        printf("%-15s%-50s%-15s%-10s\n", "Patient SSN", "Name", "DOB", "Gender");
        while (current != NULL)
        {
            if (current->patientSSN == patientSSN)
            {
                char fullName[256];
                if (strcmp(current->mname, "") == 0) // Check if middle name is empty
                    sprintf(fullName, "%s %s", current->fname, current->lname);
                else
                    sprintf(fullName, "%s %s %s", current->fname, current->mname, current->lname);

                printf("%-15d%-50s%-15s%-10c\n",
                       current->patientSSN, fullName, current->DOB, current->gender);
                found = 1;
                break;
            }
            current = current->next;
        }
        if (!found)
        {
            printf("\nNo record found for Patient SSN %d.\n", patientSSN);
        }
    }
    break;

    case 2:
    {
        char fname[256];
        printf("Enter First Name: ");
        while (getchar() != '\n')
            ;
        fgets(fname, sizeof(fname), stdin);
        fname[strcspn(fname, "\n")] = 0;
        printf("\n\n\n");

        int found = 0;
        printf("%-15s%-50s%-15s%-10s\n", "Patient SSN", "Name", "DOB", "Gender");
        for (int i = 0; i < INDEX_SIZE; i++)
        {
            Patient *current = patientIndex[i];
            while (current != NULL)
            {
                if (strcmp(current->fname, fname) == 0)
                {
                    char fullName[256];
                    if (strcmp(current->mname, "") == 0) // Check if middle name is empty
                        sprintf(fullName, "%s %s", current->fname, current->lname);
                    else
                        sprintf(fullName, "%s %s %s", current->fname, current->mname, current->lname);

                    printf("%-15d%-50s%-15s%-10c\n",
                           current->patientSSN, fullName, current->DOB, current->gender);
                    found = 1;
                }
                current = current->next;
            }
        }

        if (!found)
        {
            printf("\nNo patients found with the name '%s'.\n", fname);
        }
    }
    break;

    default:
        printf("Invalid choice.\n");
        break;
    }
}

void updatePatient() {
    int patientSSN;
    printf("Enter SSN of Patient to update or 0 to exit: ");
    scanf("%d", &patientSSN);

    if (patientSSN == 0)
    {
        return;
    }

    int index = getIndex(patientSSN);
    Patient *current = patientIndex[index];
    while (current != NULL && current->patientSSN != patientSSN)
    {
        current = current->next;
    }

    if (current == NULL)
    {
        printf("No record found for Patient SSN %d.\n", patientSSN);
    }
    else
    {
        int updateChoice;
        printf("Select field to update:\n");
        printf("1- First Name\n");
        printf("2- Country Address\n");
        scanf("%d", &updateChoice);

        switch (updateChoice)
        {
        case 1:
            printf("Enter new First Name: ");
            while (getchar() != '\n')
                ;
            fgets(current->fname, sizeof(current->fname), stdin);
            current->fname[strcspn(current->fname, "\n")] = 0;
            printf("First name updated.\n");
            break;

        case 2:
            printf("Enter new Country Address: ");
            while (getchar() != '\n')
                ;
            fgets(current->country, sizeof(current->country), stdin);
            current->country[strcspn(current->country, "\n")] = 0;
            printf("Country Address updated.\n");
            break;

        default:
            printf("Invalid choice. No updates made.\n");
            break;
        }

        printf("Updated Record: SSN: %d, Name: %s %s, Country: %s\n",
               current->patientSSN, current->fname, current->lname, current->country);
    }

    printf("\n\nEnter any key to continue\n\n");
    getchar();
    getchar();
}

void patientTable(Patient *patientIndex[], PatientPhones *patientPhonesIndex[], Appointment *appointmentIndex[])
{

    Patient patientRecord;

    int choice = 0;

    while (choice != 5)
    {
        system("clear");
        printf("1- Insert new Patient(s)\n");
        printf("2- Delete Patient(s)\n");
        printf("3- Select Patient(s)\n");
        printf("4- Update Patient(s)\n");
        printf("5- Exit\n");

        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            insertPatient(patientRecord);
            break;

        case 2:
            deletePatient();
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
                selectPatients();
            }
            else if (selection == 2)
            {
                searchPatient();
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
            updatePatient();
            break;

        default:
            break;
        }
        system("clear");
    }
}