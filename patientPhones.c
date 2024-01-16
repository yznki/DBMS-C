#include "patientPhones.h"
#include "patient.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

void loadPatientPhonesFromFile(PatientPhones *patientPhonesIndex[])
{

    FILE *file = fopen("patientPhones.csv", "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return;
    }

    char line[1024];

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

        PatientPhones *newPatientPhone = (PatientPhones *)malloc(sizeof(PatientPhones));
        if (newPatientPhone == NULL)
        {
            perror("Error allocating memory");
            continue;
        }

        *newPatientPhone = (const PatientPhones){0};

        char *token = strtok(line, ",");
        if (token != NULL)
        {
            newPatientPhone->patientSSN = atoi(token);
            token = strtok(NULL, ",");
            if (token != NULL)
            {
                strncpy(newPatientPhone->phoneNumber, token, sizeof(newPatientPhone->phoneNumber) - 1);
                newPatientPhone->phoneNumber[sizeof(newPatientPhone->phoneNumber) - 1] = '\0';
            }
        }

        int index = getIndex(newPatientPhone->patientSSN);

        if (patientPhonesIndex[index] == NULL || patientPhonesIndex[index]->patientSSN > newPatientPhone->patientSSN)
        {
            newPatientPhone->next = patientPhonesIndex[index];
            patientPhonesIndex[index] = newPatientPhone;
        }
        else
        {
            PatientPhones *current = patientPhonesIndex[index];
            while (current->next != NULL && current->next->patientSSN < newPatientPhone->patientSSN)
            {
                current = current->next;
            }
            newPatientPhone->next = current->next;
            current->next = newPatientPhone;
        }
    }

    fclose(file);
}

void savePatientPhonesToFile(PatientPhones *patientPhonesIndex[])
{

    FILE *file = fopen("patientPhones.csv", "w");
    if (file == NULL)
    {
        perror("Error opening file");
        return;
    }

    for (int i = 0; i < INDEX_SIZE; i++)
    {

        PatientPhones *current = patientPhonesIndex[i];
        while (current != NULL)
        {
            fprintf(file, "%d,%s\n",
                    current->patientSSN, current->phoneNumber);
            current = current->next;
        }
    }

    fclose(file);
}

void freePatientPhonesList(PatientPhones *patientPhonesIndex[])
{

    for (int i = 0; i < INDEX_SIZE; i++)
    {
        PatientPhones *current = patientPhonesIndex[i];
        while (current != NULL)
        {
            PatientPhones *temp = current;
            current = current->next;
            free(temp);
        }
        patientPhonesIndex[i] = NULL;
    }
}

bool isInPatientTable(int patientID, Patient *patientIndex[])
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

bool isValidPhoneNumberFormat(const char *phoneNumber)
{
    int length = strlen(phoneNumber);

    if (length == 13 && phoneNumber[0] == '+')
    {
        for (int i = 1; i < length; i++)
        {
            if (!isdigit(phoneNumber[i]))
            {
                return false;
            }
        }
        return true;
    }

    return false;
}

void patientPhonesTable(PatientPhones *patientPhonesIndex[], Patient *patientIndex[])
{

    PatientPhones newPatientPhone;
    int choice = 0;

    while (choice != 5)
    {
        system("clear");
        printf("1- Insert new Patient Phone(s)\n");
        printf("2- Delete Patient Phone(s)\n");
        printf("3- Select Patient Phone(s)\n");
        printf("4- Update Patient Phone(s)\n");
        printf("5- Exit\n");

        scanf("%d", &choice);

        switch (choice)
        {
        case 1:;
            int continueInsertion = 1;
            while (continueInsertion == 1)
            {
                printf("\n\nAnything with '*' is required.\n\n");

                printf("Enter patient SSN * or 0 to exit: ");
                if (scanf("%d", &newPatientPhone.patientSSN) != 1)
                {
                    printf("\nPatient SSN cannot be empty/character.\nTry Again...\n\n");
                    while (getchar() != '\n')
                        ;
                    continue;
                }

                if (newPatientPhone.patientSSN == 0)
                {
                    break;
                }

                if (!isInPatientTable(newPatientPhone.patientSSN, patientIndex))
                {
                    printf("\nError: Patient with SSN %d does not exist.\nTry Again...\n\n", newPatientPhone.patientSSN);
                    continue;
                }

                printf("Enter Phone Number (+962xxxxxxxxx) *: ");
                while (getchar() != '\n')
                    ;
                fgets(newPatientPhone.phoneNumber, sizeof(newPatientPhone.phoneNumber), stdin);
                newPatientPhone.phoneNumber[strcspn(newPatientPhone.phoneNumber, "\n")] = '\0';

                if (!isValidPhoneNumberFormat(newPatientPhone.phoneNumber))
                {
                    printf("\nInvalid phone number format.\nTry Again...\n\n");
                    continue;
                }

                int index = getIndex(newPatientPhone.patientSSN);
                PatientPhones *current = patientPhonesIndex[index];
                int phoneAlreadyExists = 0;

                while (current != NULL)
                {
                    if (current->patientSSN == newPatientPhone.patientSSN && strcmp(current->phoneNumber, newPatientPhone.phoneNumber) == 0)
                    {
                        phoneAlreadyExists = 1;
                        break;
                    }
                    current = current->next;
                }

                if (phoneAlreadyExists)
                {
                    printf("\nError: This phone number for patient SSN %d already exists.\nTry Again...\n\n", newPatientPhone.patientSSN);
                    continue;
                }

                PatientPhones *newNode = (PatientPhones *)malloc(sizeof(PatientPhones));
                if (newNode == NULL)
                {
                    perror("Error allocating memory");
                    continue;
                }

                *newNode = newPatientPhone;
                newNode->next = patientPhonesIndex[index];
                patientPhonesIndex[index] = newNode;

                printf("\nNew phone number added successfully for patient SSN %d.\n", newPatientPhone.patientSSN);

                printf("1- Insert another phone number.\n");
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
            break;

        case 2:;
            int patientSSN, deletionChoice = 1;
            int phoneListChoice, phoneCounter;

            while (deletionChoice != 2)
            {
                printf("Enter SSN of Patient to delete a phone record or 0 to exit: ");
                scanf("%d", &patientSSN);

                if (patientSSN == 0)
                {
                    break;
                }

                int index = getIndex(patientSSN);
                PatientPhones *current = patientPhonesIndex[index];
                PatientPhones *phoneNumbers[100];
                phoneCounter = 0;

                printf("Phone records for patient SSN %d:\n", patientSSN);
                while (current != NULL)
                {
                    if (current->patientSSN == patientSSN)
                    {
                        phoneNumbers[phoneCounter] = current;
                        printf("%d - %s\n", phoneCounter + 1, current->phoneNumber);
                        phoneCounter++;
                    }
                    current = current->next;
                }

                if (phoneCounter == 0)
                {
                    printf("\nNo phone records found for Patient with SSN %d.\n\n", patientSSN);
                    continue;
                }

                printf("\nEnter the number of the phone record to delete: ");
                scanf("%d", &phoneListChoice);

                if (phoneListChoice < 1 || phoneListChoice > phoneCounter)
                {
                    printf("\nInvalid choice. Please enter a number between 1 and %d.\n", phoneCounter);
                    continue;
                }

                phoneListChoice--;

                PatientPhones *toDelete = phoneNumbers[phoneListChoice];
                PatientPhones *prev = NULL;
                current = patientPhonesIndex[index];

                while (current != NULL && current != toDelete)
                {
                    prev = current;
                    current = current->next;
                }

                if (prev == NULL)
                {
                    patientPhonesIndex[index] = toDelete->next;
                }
                else
                {
                    prev->next = toDelete->next;
                }

                printf("\nPhone record %s deleted.\n", toDelete->phoneNumber);
                free(toDelete);

                printf("\n1- Delete another phone record.\n");
                printf("2- Return to main menu.\n");
                scanf("%d", &deletionChoice);
            }
            system("clear");
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
                printf("%-15s%-20s\n", "Patient SSN", "Phone Number");
                for (int i = 0; i < INDEX_SIZE; i++)
                {
                    PatientPhones *current = patientPhonesIndex[i];
                    while (current != NULL)
                    {
                        printf("%-15d%-20s\n", current->patientSSN, current->phoneNumber);
                        current = current->next;
                    }
                }
            }
            else if (selection == 2)
            {
                int filterChoice;
                printf("Select a condition:\n");
                printf("1- By Patient SSN\n");
                printf("2- By Phone Number\n");

                scanf("%d", &filterChoice);
                printf("\n\n\n");

                int found = 0;

                switch (filterChoice)
                {
                case 1:
                {
                    int patientSSN;
                    printf("Enter Patient SSN: ");
                    scanf("%d", &patientSSN);
                    printf("%-15s%-20s\n", "Patient SSN", "Phone Number");
                    for (int i = 0; i < INDEX_SIZE; i++)
                    {
                        PatientPhones *current = patientPhonesIndex[i];
                        while (current != NULL)
                        {
                            if (current->patientSSN == patientSSN)
                            {
                                printf("%-15d%-20s\n", current->patientSSN, current->phoneNumber);
                                found = 1;
                            }
                            current = current->next;
                        }
                    }
                    if (!found)
                    {
                        printf("\nNo phone records found for Patient SSN %d.\n", patientSSN);
                    }
                    break;
                }
                case 2:
                {
                    char searchPhone[256];
                    printf("Enter Phone Number: ");
                    while (getchar() != '\n')
                        ;
                    fgets(searchPhone, sizeof(searchPhone), stdin);
                    searchPhone[strcspn(searchPhone, "\n")] = 0;
                    printf("%-15s%-20s\n", "Patient SSN", "Phone Number");
                    for (int i = 0; i < INDEX_SIZE; i++)
                    {
                        PatientPhones *current = patientPhonesIndex[i];
                        while (current != NULL)
                        {
                            if (strcmp(current->phoneNumber, searchPhone) == 0)
                            {
                                printf("%-15d%-20s\n", current->patientSSN, current->phoneNumber);
                                found = 1;
                            }
                            current = current->next;
                        }
                    }
                    if (!found)
                    {
                        printf("\nNo records found for Phone Number %s.\n", searchPhone);
                    }
                    break;
                }
                default:
                    printf("Invalid choice.\n");
                    break;
                }
            }
            else
            {
                printf("Invalid Input.\n");
            }

            printf("\n\nEnter any key to continue\n\n");
            getchar();
            getchar();

            break;

        case 4:;
            int patientSSNToUpdate;
            printf("Enter Patient SSN to update phone record or 0 to exit: ");
            scanf("%d", &patientSSNToUpdate);
            
            if (patientSSNToUpdate == 0)
            {
                break;
            }

            printf("\n\nExisting phone numbers for Patient SSN %d:\n", patientSSNToUpdate);
            int phoneCount = 0;
            PatientPhones *recordsToUpdate[100]; // Patient has a max of 100 numbers.

            for (PatientPhones *current = patientPhonesIndex[getIndex(patientSSNToUpdate)]; current != NULL; current = current->next)
            {
                if (current->patientSSN == patientSSNToUpdate)
                {
                    printf("%d- %s\n", phoneCount + 1, current->phoneNumber);
                    recordsToUpdate[phoneCount++] = current;
                }
            }

            if (phoneCount == 0)
            {
                printf("\nNo phone records found for Patient SSN %d.\n", patientSSNToUpdate);
                printf("\n\nEnter any key to continue\n\n");
                getchar();
                getchar();
                break;
            }

            int recordNumber;
            printf("\nEnter the record number to update: ");
            scanf("%d", &recordNumber);

            if (recordNumber < 1 || recordNumber > phoneCount)
            {
                printf("\nInvalid record number.\n");
                printf("\n\nEnter any key to continue\n\n");
                getchar();
                getchar();
                break;
            }

            char newPhoneNumber[256];

            while (getchar() != '\n')
                ;

            do
            {
                printf("Enter new phone number (+962xxxxxxxxx): ");

                fgets(newPhoneNumber, sizeof(newPhoneNumber), stdin);
                newPhoneNumber[strcspn(newPhoneNumber, "\n")] = '\0';

                if (!isValidPhoneNumberFormat(newPhoneNumber))
                {
                    printf("\nInvalid phone number format.\nTry Again...\n\n");
                    while (getchar() != '\n')
                        ;
                    continue;
                } else {
                    break;
                }

            } while (1);

            int isDuplicate = 0;
            for (int i = 0; i < INDEX_SIZE; i++)
            {
                for (PatientPhones *current = patientPhonesIndex[i]; current != NULL; current = current->next)
                {
                    if (strcmp(current->phoneNumber, newPhoneNumber) == 0)
                    {
                        isDuplicate = 1;
                        break;
                    }
                }
                if (isDuplicate)
                {
                    break;
                }
            }

            if (isDuplicate)
            {
                printf("\nThis phone number already exists. Try again with a different number.\n");
            }
            else
            {
                strncpy(recordsToUpdate[recordNumber - 1]->phoneNumber, newPhoneNumber, sizeof(recordsToUpdate[recordNumber - 1]->phoneNumber));
                printf("\nPhone number updated successfully.\n");
            }

            printf("\n\nEnter any key to continue\n\n");
            getchar();
            getchar();

            break;
        }
    }
}