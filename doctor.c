#include "doctor.h"
#include "appointment.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void loadDoctorsFromFile(Doctor *doctorIndex[])
{
    FILE *file = fopen("doctors.csv", "r");
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
        Doctor *newDoctor = (Doctor *)malloc(sizeof(Doctor));
        if (newDoctor == NULL)
        {
            perror("Error allocating memory");
            continue;
        }

        *newDoctor = (const Doctor){0};

        char *start = line;
        char *end;
        int field = 0;
        while (field < 12 && *start)
        {
            end = start;
            while (*end && *end != ',' && *end != '\n')
                end++;

            char value[256] = {0};
            int len = end - start < sizeof(value) - 1 ? end - start : sizeof(value) - 1;
            strncpy(value, start, len);
            value[len] = '\0';

            switch (field)
            {
            case 0:
                newDoctor->doctorID = atoi(value);
                break;
            case 1:
                newDoctor->departmentID = atoi(value);
                break;
            case 2:
                strncpy(newDoctor->fname, value, sizeof(newDoctor->fname));
                break;
            case 3:
                strncpy(newDoctor->mname, value, sizeof(newDoctor->mname));
                break;
            case 4:
                strncpy(newDoctor->lname, value, sizeof(newDoctor->lname));
                break;
            case 5:
                strncpy(newDoctor->DOB, value, sizeof(newDoctor->DOB));
                break;
            case 6:
                newDoctor->gender = value[0];
                break;
            case 7:
                strncpy(newDoctor->phoneNumber, value, sizeof(newDoctor->phoneNumber));
                break;
            case 8:
                strncpy(newDoctor->street, value, sizeof(newDoctor->street));
                break;
            case 9:
                strncpy(newDoctor->city, value, sizeof(newDoctor->city));
                break;
            case 10:
                strncpy(newDoctor->state, value, sizeof(newDoctor->state));
                break;
            case 11:
                strncpy(newDoctor->country, value, sizeof(newDoctor->country));
                break;
            }

            start = *end ? end + 1 : end;
            field++;
        }

        int index = getIndex(newDoctor->doctorID);

        if (doctorIndex[index] == NULL || doctorIndex[index]->doctorID >= newDoctor->doctorID)
        {
            newDoctor->next = doctorIndex[index];
            doctorIndex[index] = newDoctor;
        }
        else
        {
            Doctor *current = doctorIndex[index];
            while (current->next != NULL && current->next->doctorID < newDoctor->doctorID)
            {
                current = current->next;
            }
            newDoctor->next = current->next;
            current->next = newDoctor;
        }
    }

    fclose(file);
}

void saveDoctorsToFile(Doctor *doctorIndex[])
{
    FILE *file = fopen("doctors.csv", "w");
    if (file == NULL)
    {
        perror("Error opening file");
        return;
    }

    fprintf(file, "Doctor ID, Department ID, First Name, Middle Name, Last Name, Date of Birth, Gender, Phone Number, Street, City, State, Country\n");

    for (int i = 0; i < INDEX_SIZE; i++)
    {
        Doctor *current = doctorIndex[i];
        while (current != NULL)
        {
            fprintf(file, "%d,%d,%s,%s,%s,%s,%c,%s,%s,%s,%s,%s\n",
                    current->doctorID, current->departmentID, current->fname, current->mname, current->lname, current->DOB,
                    current->gender, current->phoneNumber, current->street, current->city,
                    current->state, current->country);
            current = current->next;
        }
    }

    fclose(file);
}

void freeDoctorList(Doctor *doctorIndex[])
{
    for (int i = 0; i < INDEX_SIZE; i++)
    {
        Doctor *current = doctorIndex[i];
        while (current != NULL)
        {
            Doctor *temp = current;
            current = current->next;
            free(temp);
        }
        doctorIndex[i] = NULL;
    }
}

void deleteDoctorFromAppointments(Appointment *appointmentIndex[], int ID)
{

    for (int i = 0; i < INDEX_SIZE; i++)
    {
        Appointment *current = appointmentIndex[i];
        Appointment *previous = NULL;

        while (current != NULL)
        {
            if (current->patientID == ID)
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

void insertDoctor()
{
    int continueInsertion = 1;
    while (continueInsertion == 1)
    {
        Doctor newDoctor;
        printf("\n\nAnything with '*' is required.\n\n");
        printf("Enter doctorID * or 0 to exit: ");
        if (scanf("%d", &newDoctor.doctorID) != 1)
        {
            printf("Doctor ID cannot be empty/zero/character.\n");
            while (getchar() != '\n')
                ;
            continue;
        }

        if (newDoctor.doctorID == 0)
        {
            break;
        }

        int index = getIndex(newDoctor.doctorID);
        Doctor *current = doctorIndex[index];
        int idAlreadyExists = 0;

        while (current != NULL)
        {
            if (current->doctorID == newDoctor.doctorID)
            {
                idAlreadyExists = 1;
                break;
            }
            current = current->next;
        }

        if (idAlreadyExists)
        {
            printf("\n\nError: A doctor with ID %d already exists.\n\n", newDoctor.doctorID);
            continue;
        }

        while (getchar() != '\n')
            ;

        printf("Enter Department ID *: ");
        if (scanf("%d", &newDoctor.departmentID) != 1 || newDoctor.departmentID == 0)
        {
            printf("Invalid input for Department ID.\n");
            while (getchar() != '\n')
                ;
            continue;
        }
        while (getchar() != '\n')
            ;

        printf("Enter First Name *: ");
        fgets(newDoctor.fname, sizeof(newDoctor.fname), stdin);
        newDoctor.fname[strcspn(newDoctor.fname, "\n")] = 0;
        if (strlen(newDoctor.fname) == 0)
        {
            printf("\nFirst name cannot be empty.\nTry Again...\n\n");
            continue;
        }

        printf("Enter Middle Name: ");
        fgets(newDoctor.mname, sizeof(newDoctor.mname), stdin);
        newDoctor.mname[strcspn(newDoctor.mname, "\n")] = 0;

        printf("Enter Last Name *: ");
        fgets(newDoctor.lname, sizeof(newDoctor.lname), stdin);
        newDoctor.lname[strcspn(newDoctor.lname, "\n")] = 0;
        if (strlen(newDoctor.fname) == 0)
        {
            printf("\nLast name cannot be empty.\nTry Again...\n\n");
            continue;
        }

        printf("Enter Date of Birth (DD-MM-YYYY) *: ");
        fgets(newDoctor.DOB, sizeof(newDoctor.DOB), stdin);
        newDoctor.DOB[strcspn(newDoctor.DOB, "\n")] = 0;
        if (strlen(newDoctor.fname) == 0)
        {
            printf("\nDate of Birth cannot be empty.\nTry Again...\n\n");
            continue;
        }

        do
        {
            printf("Enter Gender (M or F) *: ");
            fgets(newDoctor.genderStr, sizeof(newDoctor.genderStr), stdin);
            newDoctor.gender = newDoctor.genderStr[0];
            if (newDoctor.gender != 'F' && newDoctor.gender != 'M')
            {
                printf("\nPlease choose M or F.\n");
            }
        } while (newDoctor.gender != 'M' && newDoctor.gender != 'F');

        printf("Enter Phone Number (+962xxxxxxxxx)*: ");
        fgets(newDoctor.phoneNumber, sizeof(newDoctor.phoneNumber), stdin);
        newDoctor.phoneNumber[strcspn(newDoctor.phoneNumber, "\n")] = 0;

        if (!isValidPhoneNumberFormat(newDoctor.phoneNumber))
        {
            printf("\nInvalid phone number format.\nTry Again...\n\n");
            continue;
        }

        printf("Enter Street Name: ");
        fgets(newDoctor.street, sizeof(newDoctor.street), stdin);
        newDoctor.street[strcspn(newDoctor.street, "\n")] = 0;

        printf("Enter City: ");
        fgets(newDoctor.city, sizeof(newDoctor.city), stdin);
        newDoctor.city[strcspn(newDoctor.city, "\n")] = 0;

        printf("Enter State: ");
        fgets(newDoctor.state, sizeof(newDoctor.state), stdin);
        newDoctor.state[strcspn(newDoctor.state, "\n")] = 0;

        printf("Enter Country: ");
        fgets(newDoctor.country, sizeof(newDoctor.country), stdin);
        newDoctor.country[strcspn(newDoctor.country, "\n")] = 0;

        Doctor *newNode = (Doctor *)malloc(sizeof(Doctor));
        if (newNode == NULL)
        {
            perror("Error allocating memory");
            continue;
        }

        *newNode = newDoctor;
        newNode->next = doctorIndex[index];
        doctorIndex[index] = newNode;

        printf("\nNew doctor added successfully.\n");

        printf("1- Insert another doctor.\n");
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

void deleteDoctor()
{
    int doctorID, deletion = 1;
    while (deletion != 2)
    {
        if (deletion == 1)
        {
            printf("Enter ID of Doctor to delete or 0 to exit: ");
            scanf("%d", &doctorID);

            if (doctorID == 0)
            {
                break;
            }

            int index = getIndex(doctorID);
            Doctor *current = doctorIndex[index], *prev = NULL;

            while (current != NULL)
            {
                if (current->doctorID == doctorID)
                {
                    if (prev == NULL)
                    {
                        doctorIndex[index] = current->next;
                    }
                    else
                    {
                        prev->next = current->next;
                    }
                    free(current);
                    printf("\nDoctor with ID %d deleted.\n", doctorID);
                    deleteDoctorFromAppointments(appointmentIndex, doctorID);
                    break;
                }
                prev = current;
                current = current->next;
            }

            if (current == NULL)
            {
                printf("\nDoctor with ID %d not found.\n", doctorID);
            }
        }

        printf("1- Delete another doctor.\n");
        printf("2- Return to main menu.\n");
        scanf("%d", &deletion);
    }
}

void selectDoctors()
{
    printf("%-15s%-20s%-30s%-15s%-10s%-15s%-25s\n", "Doctor ID", "Department ID", "Name", "DOB", "Gender", "Phone", "Address");
    for (int i = 0; i < INDEX_SIZE; i++)
    {
        Doctor *current = doctorIndex[i];
        while (current != NULL)
        {
            char fullName[256];
            char fullAddress[512];
            if (strcmp(current->mname, "") == 0) // Check if middle name is empty
                sprintf(fullName, "%s %s", current->fname, current->lname);
            else
                sprintf(fullName, "%s %s %s", current->fname, current->mname, current->lname);

            sprintf(fullAddress, "%s, %s, %s, %s", current->street, current->city, current->state, current->country);

            printf("%-15d%-20d%-30s%-15s%-10c%-15s%-25s\n",
                   current->doctorID, current->departmentID, fullName,
                   current->DOB, current->gender, current->phoneNumber, fullAddress);
            current = current->next;
        }
    }
}

void searchDoctor()
{
    int filterChoice;
    printf("According to which attribute?\n");
    printf("1- Doctor ID\n");
    printf("2- Department ID\n");
    printf("3- Name\n");

    scanf("%d", &filterChoice);
    printf("\n\n\n");

    switch (filterChoice)
    {
    case 1:
    {
        int doctorID;
        printf("Enter Doctor ID: ");
        scanf("%d", &doctorID);
        printf("\n\n\n");

        int index = getIndex(doctorID);
        Doctor *current = doctorIndex[index];
        int found = 0;
        printf("%-15s%-20s%-30s%-15s%-10s%-15s\n", "Doctor ID", "Department ID", "Name", "DOB", "Gender", "Phone");
        while (current != NULL)
        {
            if (current->doctorID == doctorID)
            {
                char fullName[256];
                if (strcmp(current->mname, "") == 0)
                    sprintf(fullName, "%s %s", current->fname, current->lname);
                else
                    sprintf(fullName, "%s %s %s", current->fname, current->mname, current->lname);

                printf("%-15d%-20d%-30s%-15s%-10c%-15s\n",
                       current->doctorID, current->departmentID, fullName,
                       current->DOB, current->gender, current->phoneNumber);
                found = 1;
                break;
            }
            current = current->next;
        }
        if (!found)
        {
            printf("\nNo record found for Doctor ID %d.\n", doctorID);
        }
    }
    break;

    case 2:
    {
        int deptID;
        printf("Enter Department ID: ");
        scanf("%d", &deptID);
        printf("\n\n\n");

        int found = 0;
        printf("%-15s%-20s%-30s%-15s%-10s%-15s\n", "Doctor ID", "Department ID", "Name", "DOB", "Gender", "Phone");
        for (int i = 0; i < INDEX_SIZE; i++)
        {
            Doctor *current = doctorIndex[i];
            while (current != NULL)
            {
                if (current->departmentID == deptID)
                {
                    char fullName[256];
                    if (strcmp(current->mname, "") == 0)
                        sprintf(fullName, "%s %s", current->fname, current->lname);
                    else
                        sprintf(fullName, "%s %s %s", current->fname, current->mname, current->lname);

                    printf("%-15d%-20d%-30s%-15s%-10c%-15s\n",
                           current->doctorID, current->departmentID, fullName,
                           current->DOB, current->gender, current->phoneNumber);
                    found = 1;
                }
                current = current->next;
            }
        }

        if (!found)
        {
            printf("\nNo doctors found with Department ID %d.\n", deptID);
        }
    }
    break;

    case 3:
    {
        char name[256];
        printf("Enter First Name: ");
        while (getchar() != '\n')
            ;
        fgets(name, sizeof(name), stdin);
        name[strcspn(name, "\n")] = 0;
        printf("\n\n\n");

        int found = 0;
        printf("%-15s%-20s%-30s%-15s%-10s%-15s\n", "Doctor ID", "Department ID", "Name", "DOB", "Gender", "Phone");
        for (int i = 0; i < INDEX_SIZE; i++)
        {
            Doctor *current = doctorIndex[i];
            while (current != NULL)
            {
                if (strstr(current->fname, name) || strstr(current->mname, name) || strstr(current->lname, name))
                {
                    char fullName[256];
                    if (strcmp(current->mname, "") == 0)
                        sprintf(fullName, "%s %s", current->fname, current->lname);
                    else
                        sprintf(fullName, "%s %s %s", current->fname, current->mname, current->lname);

                    printf("%-15d%-20d%-30s%-15s%-10c%-15s\n",
                           current->doctorID, current->departmentID, fullName,
                           current->DOB, current->gender, current->phoneNumber);
                    found = 1;
                }
                current = current->next;
            }
        }

        if (!found)
        {
            printf("\nNo doctors found with the name '%s'.\n", name);
        }
    }
    break;

    default:
        printf("Invalid choice.\n");
        break;
    }
}

void updateDoctor()
{

    int doctorIDToUpdate;
    printf("Enter ID of Doctor to update or 0 to exit: ");
    scanf("%d", &doctorIDToUpdate);

    if (doctorIDToUpdate == 0)
    {
        return;
    }

    int indexToUpdate = getIndex(doctorIDToUpdate);
    Doctor *currentToUpdate = doctorIndex[indexToUpdate];
    while (currentToUpdate != NULL && currentToUpdate->doctorID != doctorIDToUpdate)
    {
        currentToUpdate = currentToUpdate->next;
    }

    if (currentToUpdate == NULL)
    {
        printf("No record found for Doctor ID %d.\n", doctorIDToUpdate);
    }
    else
    {
        int updateChoice;
        printf("Select field to update:\n");
        printf("1- Department ID\n");
        printf("2- Name\n");
        printf("3- Phone Number\n");
        scanf("%d", &updateChoice);

        switch (updateChoice)
        {
        case 1:
            printf("Enter new Department ID: ");
            scanf("%d", &currentToUpdate->departmentID);
            printf("Department ID updated.\n");
            break;

        case 2:
            printf("Enter new First Name: ");
            scanf("%s", currentToUpdate->fname);
            printf("Enter new Middle Name: ");
            scanf("%s", currentToUpdate->mname);
            printf("Enter new Last Name: ");
            scanf("%s", currentToUpdate->lname);
            printf("Name updated.\n");
            break;

        case 3:;
            char number[13];

            do
            {
                printf("Enter new Phone Number (+9627xxxxxxxx): ");
                scanf("%s", number);
                if (!isValidPhoneNumberFormat(number))
                {
                    printf("\nInvalid phone number format.\nTry Again...\n\n");
                    continue;
                }
                break;

            } while (1);

            strcpy(currentToUpdate->phoneNumber, number);
            printf("Phone number updated.\n");
            break;

        default:
            printf("Invalid choice. No updates made.\n");
            break;
        }
    }

    printf("\n\nEnter any key to continue\n\n");
    getchar();
    getchar();
}

void doctorTable(Doctor *doctorIndex[], Appointment *appointmentIndex[])
{
    int choice = 0;

    while (choice != 5)
    {
        system("clear");
        printf("1- Insert new Doctor(s)\n");
        printf("2- Delete Doctor(s)\n");
        printf("3- Select Doctor(s)\n");
        printf("4- Update Doctor(s)\n");
        printf("5- Exit\n");

        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            insertDoctor();
            break;

        case 2:
            deleteDoctor();
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
                selectDoctors();
            }
            else if (selection == 2)
            {
                searchDoctor();
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
            updateDoctor();
            break;
        }
        system("clear");
    }
}