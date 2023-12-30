#include "patient.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void loadPatientsFromFile(Patient *patientIndex[])
{
    FILE *file = fopen("patients.csv", "r");
    char line[1024];
    if (file == NULL)
    {
        perror("Error opening file");
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

        sscanf(line, "%d,%255[^,],%255[^,],%255[^,],%255[^,],%c,%10[^,],%255[^,],%255[^,],%255[^,],%255[^,\n]",
               &newPatient->patientSSN, newPatient->fname, newPatient->mname, newPatient->lname,
               newPatient->DOB, &newPatient->gender, newPatient->phoneNumber, newPatient->street,
               newPatient->city, newPatient->state, newPatient->country);

        int index = getIndex(newPatient->patientSSN);

        if (patientIndex[index] == NULL || patientIndex[index]->patientSSN >= newPatient->patientSSN)
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

    for (int i = 0; i < INDEX_SIZE; i++)
    {
        Patient *current = patientIndex[i];
        while (current != NULL)
        {
            fprintf(file, "%d,%s,%s,%s,%s,%c,%s,%s,%s,%s,%s\n",
                    current->patientSSN, current->fname, current->mname, current->lname, current->DOB,
                    current->gender, current->phoneNumber, current->street, current->city,
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

void patientTable(Patient *patientIndex[])
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
        case 1:;
            int continueInsertion = 1; 
            while (continueInsertion == 1)
            {
                printf("Enter patientSSN: ");
                if (scanf("%d", &patientRecord.patientSSN) != 1 || patientRecord.patientSSN == 0)
                {
                    printf("SSN cannot be empty or zero.\n");
                    while (getchar() != '\n')
                        ;
                    continue; 
                }
                while (getchar() != '\n')
                    ;

                printf("Enter First Name: ");
                fgets(patientRecord.fname, sizeof(patientRecord.fname), stdin);
                patientRecord.fname[strcspn(patientRecord.fname, "\n")] = 0;
                if (strlen(patientRecord.fname) == 0)
                {
                    printf("First name cannot be empty.\n");
                    continue;
                }

                printf("Enter Middle Name: ");
                fgets(patientRecord.mname, sizeof(patientRecord.mname), stdin);
                patientRecord.mname[strcspn(patientRecord.mname, "\n")] = 0;

                printf("Enter Last Name: ");
                fgets(patientRecord.lname, sizeof(patientRecord.lname), stdin);
                patientRecord.lname[strcspn(patientRecord.lname, "\n")] = 0;

                printf("Enter Date of Birth (DD-MM-YYYY): ");
                fgets(patientRecord.DOB, sizeof(patientRecord.DOB), stdin);
                patientRecord.DOB[strcspn(patientRecord.DOB, "\n")] = 0;
                if (strlen(patientRecord.DOB) == 0)
                {
                    printf("Date of Birth cannot be empty.\n");
                    continue;
                }

                do
                {
                    printf("Enter Gender (M or F): ");
                    fgets(patientRecord.genderStr, sizeof(patientRecord.genderStr), stdin);
                    patientRecord.gender = patientRecord.genderStr[0];
                } while (patientRecord.gender != 'M' && patientRecord.gender != 'F');

                printf("Enter Phone Number (07xxxxxxxx): ");
                if (fgets(patientRecord.phoneNumber, sizeof(patientRecord.phoneNumber), stdin) != NULL)
                {
                    size_t len = strlen(patientRecord.phoneNumber);
                    if (len > 0 && patientRecord.phoneNumber[len - 1] == '\n')
                    {
                        patientRecord.phoneNumber[len - 1] = '\0';
                    }
                    else
                    {
                        while (getchar() != '\n' && !feof(stdin) && !ferror(stdin))
                            ;
                    }
                }

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

                int index = getIndex(patientRecord.patientSSN);
                Patient *current = patientIndex[index];
                int ssnAlreadyExists = 0, phoneNumberAlreadyExists = 0;

                while (current != NULL)
                {
                    if (current->patientSSN == patientRecord.patientSSN)
                    {
                        ssnAlreadyExists = 1;
                    }
                    if (strcmp(current->phoneNumber, patientRecord.phoneNumber) == 0)
                    {
                        phoneNumberAlreadyExists = 1;
                    }
                    current = current->next;
                }

                if (ssnAlreadyExists)
                {
                    printf("\n\nError: A patient with SSN %d already exists.\n\n", patientRecord.patientSSN);
                    continue;
                }
                else if (phoneNumberAlreadyExists)
                {
                    printf("\n\nError: A patient with phone number %s already exists.\n\n", patientRecord.phoneNumber);
                    continue;
                }
                else
                {
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
                    printf("patientSSN - %d, First Name - %s, Middle Name - %s, Last Name - %s, DOB - %s, Gender - %c, Phone Number - %s, Street - %s, City - %s, State - %s, Country - %s\n\n", patientRecord.patientSSN, patientRecord.fname, patientRecord.mname, patientRecord.lname, patientRecord.DOB, patientRecord.gender, patientRecord.phoneNumber, patientRecord.street, patientRecord.city, patientRecord.state, patientRecord.country);
                }

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
            break;

        case 2:;
            int patientID, deletion = 1;
            while (deletion != 2)
            {
                if (deletion == 1)
                {
                    printf("Enter SSN of Patient to delete: ");
                    scanf("%d", &patientID);

                    int index = getIndex(patientID);
                    Patient *current = patientIndex[index];

                    if (current != NULL && current->patientSSN == patientID)
                    {
                        patientIndex[index] = current->next;
                        free(current);
                        printf("\n\nPatient with SSN %d deleted.\n\n", patientID);
                    }
                    else
                    {
                        // Search for the patient to be deleted, keep track of the node just before the one to be deleted
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
            break;

        case 3:;

            int selection;
            printf("Select with a condition or all?\n");
            printf("1- All\n");
            printf("2- Condition\n");

            scanf("%d", &selection);

            if (selection == 1)
            {
                for (int i = 0; i < INDEX_SIZE; i++)
                {
                    Patient *current = patientIndex[i];
                    if (current == NULL)
                        continue;

                    while (current != NULL)
                    {
                        printf("Patient SSN: %d, Name: %s %s %s, DOB: %s, Gender: %c, Phone: %s\n",
                               current->patientSSN, current->fname, current->mname, current->lname,
                               current->DOB, current->gender, current->phoneNumber);
                        current = current->next;
                    }
                }
            }
            else if (selection == 2)
            {
                int choiceAttribute;
                printf("According to which attribute?\n");
                printf("1- SSN\n");
                printf("2- First Name\n");

                scanf("%d", &choiceAttribute);

                if (choiceAttribute == 1)
                {
                    int patientSSN;
                    printf("Enter Patient SSN: ");
                    scanf("%d", &patientSSN);

                    int index = getIndex(patientSSN);
                    Patient *current = patientIndex[index];
                    while (current != NULL)
                    {
                        if (current->patientSSN == patientSSN)
                        {
                            printf("Patient SSN: %d, Name: %s %s %s, DOB: %s, Gender: %c, Phone: %s\n",
                                   current->patientSSN, current->fname, current->mname, current->lname,
                                   current->DOB, current->gender, current->phoneNumber);
                            break;
                        }
                        current = current->next;
                    }
                    if (current == NULL)
                    {
                        printf("No record found for Patient SSN %d.\n", patientSSN);
                    }
                }
                else if (choiceAttribute == 2)
                {
                    char fname[256];
                    printf("Enter First Name: ");
                    while (getchar() != '\n')
                        ;
                    fgets(fname, sizeof(fname), stdin);
                    fname[strcspn(fname, "\n")] = 0;

                    for (int i = 0; i < INDEX_SIZE; i++)
                    {
                        Patient *current = patientIndex[i];
                        while (current != NULL)
                        {
                            if (strcmp(current->fname, fname) == 0)
                            {
                                printf("Patient SSN: %d, Name: %s %s %s, DOB: %s, Gender: %c, Phone: %s\n",
                                       current->patientSSN, current->fname, current->mname, current->lname,
                                       current->DOB, current->gender, current->phoneNumber);
                            }
                            current = current->next;
                        }
                    }
                }
                else
                {
                    printf("Invalid Input.\n");
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
            int patientSSN;
            printf("Enter SSN of Patient to update: ");
            scanf("%d", &patientSSN);

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
                printf("2- Phone Number\n");
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
                    printf("Enter new Phone Number: ");
                    scanf("%s", current->phoneNumber);
                    printf("Phone number updated.\n");
                    break;

                default:
                    printf("Invalid choice. No updates made.\n");
                    break;
                }

                printf("Updated Record: SSN: %d, Name: %s, Phone: %s\n",
                       current->patientSSN, current->fname, current->phoneNumber);
            }

            printf("\n\nEnter any key to continue\n\n");
            getchar();
            getchar();

            break;

        default:
            break;
        }
        system("clear");
    }
}