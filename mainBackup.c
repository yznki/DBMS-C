#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "patient.h"

#define INDEX_SIZE 100

typedef struct Patient
{

    int patientSSN;
    char fname[256], mname[256], lname[256], DOB[256];
    char genderStr[3], phoneNumber[11], street[256], city[256], state[256], country[256];
    char gender;

    struct Patient *next;

} Patient;

Patient *patientIndex[INDEX_SIZE] = {NULL};

int getIndex(int ID)
{
    return ID % INDEX_SIZE;
}

void loadPatientsFromFile(Patient *patientIndex[])
{
    FILE *file = fopen("patients.csv", "r");
    char line[1024];
    if (file == NULL)
    {
        perror("Error opening file");
        return;
    }

    Patient *last = NULL;

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

        newPatient->next = NULL;

        int index = getIndex(newPatient->patientSSN);

        newPatient->next = patientIndex[index];
        patientIndex[index] = newPatient;
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
        printf("1- Insert new Patient(s)\n");
        printf("2- Delete Patient(s)\n");
        printf("3- Select Patient(s)\n");
        printf("4- Update Patient(s)\n");
        printf("5- Exit\n");

        scanf("%d", &choice);

        switch (choice)
        {
        case 1:;
            int insertion = 1;
            while (insertion != 2)
            {
                switch (insertion)
                {
                case 1:
                    printf("Enter patientSSN: ");
                    scanf("%d", &patientRecord.patientSSN);
                    while (getchar() != '\n')
                        ;

                    printf("Enter First Name: ");
                    fgets(patientRecord.fname, sizeof(patientRecord.fname), stdin);
                    patientRecord.fname[strcspn(patientRecord.fname, "\n")] = 0;

                    printf("Enter Middle Name: ");
                    fgets(patientRecord.mname, sizeof(patientRecord.mname), stdin);
                    patientRecord.mname[strcspn(patientRecord.mname, "\n")] = 0;

                    printf("Enter Last Name: ");
                    fgets(patientRecord.lname, sizeof(patientRecord.lname), stdin);
                    patientRecord.lname[strcspn(patientRecord.lname, "\n")] = 0;

                    printf("Enter Date of Birth (DD-MM-YYYY): ");
                    fgets(patientRecord.DOB, sizeof(patientRecord.DOB), stdin);
                    patientRecord.DOB[strcspn(patientRecord.DOB, "\n")] = 0;

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
                            int ch;
                            while ((ch = getchar()) != '\n' && ch != EOF)
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
                        break;
                    }
                    else if (phoneNumberAlreadyExists)
                    {
                        printf("\n\nError: A patient with phone number %s already exists.\n\n", patientRecord.phoneNumber);
                        break;
                    }
                    else
                    {
                        Patient *newNode = (Patient *)malloc(sizeof(Patient));
                        if (newNode == NULL)
                        {
                            perror("Error allocating memory");
                            break;
                        }

                        *newNode = patientRecord;
                        newNode->next = patientIndex[index];
                        patientIndex[index] = newNode;

                        printf("\n\nThe following is the new record data:\n");
                        printf("patientSSN - %d, First Name - %s, Middle Name - %s, Last Name - %s, DOB - %s, Gender - %c, Phone Number - %s, Street - %s, City - %s, State - %s, Country - %s\n\n", patientRecord.patientSSN, patientRecord.fname, patientRecord.mname, patientRecord.lname, patientRecord.DOB, patientRecord.gender, patientRecord.phoneNumber, patientRecord.street, patientRecord.city, patientRecord.state, patientRecord.country);
                        break;
                    }

                    break;

                default:
                    break;
                }

                printf("1- Insert another record.\n");
                printf("2- Return to main menu.\n");

                scanf("%d", &insertion);
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
                        ; // Clear the input buffer
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

                // Optionally, display the updated record
                printf("Updated Record: SSN: %d, Name: %s, Phone: %s\n",
                       current->patientSSN, current->fname, current->phoneNumber);
            }

            break;

        default:
            break;
        }
    }
}

void doctorTable();
void appointmentTable();
void billTable();
void diagnosisTable();
void medicalRecordTable();
void prescriptionTable();
void qualificationsTable();
void departmentTable();

int main(int argc, char const *argv[])
{
    // system("clear");
    int choice = 0;
    printf("Welcome to the Jordanian Hospital\n");

    while (choice != 10)
    {
        printf("Choose a table to work with:\n");
        printf("1- Patient\n");
        printf("2- Doctor\n");
        printf("3- Appointment\n");
        printf("4- Bill\n");
        printf("5- Diagnosis\n");
        printf("6- Medical Record\n");
        printf("7- Prescription\n");
        printf("8- Qualifications\n");
        printf("9- Department\n");
        printf("10- Exit\n");

        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            loadPatientsFromFile(patientIndex);
            system("clear");
            patientTable(patientIndex);
            system("clear");
            savePatientsToFile(patientIndex);
            freePatientList(patientIndex);
            break;
        case 2:
            system("clear");
            doctorTable();
            system("clear");
            break;
        case 3:
            system("clear");
            appointmentTable();
            system("clear");
            break;
        case 4:
            system("clear");
            billTable();
            system("clear");
            break;
        case 5:
            system("clear");
            diagnosisTable();
            system("clear");
            break;
        case 6:
            system("clear");
            medicalRecordTable();
            system("clear");
            break;
        case 7:
            system("clear");
            prescriptionTable();
            system("clear");
            break;
        case 8:
            system("clear");
            qualificationsTable();
            system("clear");
            break;
        case 9:
            system("clear");
            departmentTable();
            system("clear");
            break;
        case 10:
            system("clear");
            printf("\n\n\n\n\nGoodbye!\n\n\n\n\n");
            break;
        default:
            system("clear");
            printf("Invalid Input\n\n\n");
            break;
        }
    }

    return 0;
}

void doctorTable()
{

    int doctorID = 0, departmentID = 0;
    char fname[256], mname[256], lname[256], DOB[256];
    char genderStr[3], phoneNumber[12], street[256], city[256], state[256], country[256];
    char gender;

    int choice = 0;

    while (choice != 5)
    {
        printf("1- Insert new Doctor(s)\n");
        printf("2- Delete Doctor(s)\n");
        printf("3- Select Doctor(s)\n");
        printf("4- Update Doctor(s)\n");
        printf("5- Exit\n");

        scanf("%d", &choice);

        switch (choice)
        {
        case 1:;
            int insertion = 1;
            while (insertion != 2)
            {
                switch (insertion)
                {
                case 1:
                    printf("Enter Doctor ID: ");
                    scanf("%d", &doctorID);
                    while (getchar() != '\n')
                        ;

                    printf("Enter First Name: ");
                    fgets(fname, sizeof(fname), stdin);
                    fname[strcspn(fname, "\n")] = 0;

                    printf("Enter Middle Name: ");
                    fgets(mname, sizeof(mname), stdin);
                    mname[strcspn(mname, "\n")] = 0;

                    printf("Enter Last Name: ");
                    fgets(lname, sizeof(lname), stdin);
                    lname[strcspn(lname, "\n")] = 0;

                    printf("Enter Date of Birth (DD-MM-YYYY): ");
                    fgets(DOB, sizeof(DOB), stdin);
                    DOB[strcspn(DOB, "\n")] = 0;

                    do
                    {
                        printf("Enter Gender (M or F): ");
                        fgets(genderStr, sizeof(genderStr), stdin);
                        gender = genderStr[0];
                    } while (gender != 'M' && gender != 'F');

                    printf("Enter Phone Number (07xxxxxxxx): ");
                    fgets(phoneNumber, sizeof(phoneNumber), stdin);
                    phoneNumber[strcspn(phoneNumber, "\n")] = 0;

                    printf("Enter Street Name: ");
                    fgets(street, sizeof(street), stdin);
                    street[strcspn(street, "\n")] = 0;

                    printf("Enter City: ");
                    fgets(city, sizeof(city), stdin);
                    city[strcspn(city, "\n")] = 0;

                    printf("Enter State: ");
                    fgets(state, sizeof(state), stdin);
                    state[strcspn(state, "\n")] = 0;

                    printf("Enter Country: ");
                    fgets(country, sizeof(country), stdin);
                    country[strcspn(country, "\n")] = 0;

                    printf("Enter department ID: ");
                    scanf("%d", &departmentID);
                    while (getchar() != '\n')
                        ;

                    printf("\n\nThe following is the new record data:\n");
                    printf("patientSSN - %d, First Name - %s, Middle Name - %s, Last Name - %s, DOB - %s, Gender - %c, Phone Number - %s, Street - %s, City - %s, State - %s, Country - %s, Department ID - %d\n\n", doctorID, fname, mname, lname, DOB, gender, phoneNumber, street, city, state, country, departmentID);
                    break;

                default:
                    break;
                }

                printf("1- Insert another record.\n");
                printf("2- Return to main menu.\n");

                scanf("%d", &insertion);
            }

            break;

        default:
            break;
        }

        system("clear");
    }
}

void appointmentTable()
{
    int ID = 0, MedicalRecordID = 0, DoctorID = 0;
    char date[256];

    int choice = 0;

    while (choice != 5)
    {
        printf("1- Insert new Appointment(s)\n");
        printf("2- Delete Appointment(s)\n");
        printf("3- Select Appointment(s)\n");
        printf("4- Update Appointment(s)\n");
        printf("5- Exit\n");

        scanf("%d", &choice);

        switch (choice)
        {
        case 1:;
            int insertion = 1;
            while (insertion != 2)
            {
                switch (insertion)
                {
                case 1:
                    getchar();
                    printf("Enter Date of Appointment (DD-MM-YYYY): ");
                    fgets(date, sizeof(date), stdin);
                    date[strcspn(date, "\n")] = 0;

                    printf("Enter Appointment ID: ");
                    scanf("%d", &ID);
                    while (getchar() != '\n')
                        ;

                    printf("Enter Doctor ID: ");
                    scanf("%d", &DoctorID);
                    while (getchar() != '\n')
                        ;

                    printf("Enter Medical Record ID: ");
                    scanf("%d", &MedicalRecordID);
                    while (getchar() != '\n')
                        ;

                    printf("\n\nThe following is the new record data:\n");
                    printf("ID - %d, Date - %s, DoctorID - %d, Medical Record ID - %d\n\n", ID, date, DoctorID, MedicalRecordID);
                    break;

                default:
                    break;
                }

                printf("1- Insert another appointment.\n");
                printf("2- Return to main menu.\n");

                scanf("%d", &insertion);
            }

            break;

        default:
            break;
        }

        system("clear");
    }
}

void billTable()
{
    int ID = 0, AppointmentID = 0;
    double price = 0;

    int choice = 0;

    while (choice != 5)
    {
        printf("1- Insert new Bill(s)\n");
        printf("2- Delete Bill(s)\n");
        printf("3- Select Bill(s)\n");
        printf("4- Update Bill(s)\n");
        printf("5- Exit\n");

        scanf("%d", &choice);

        switch (choice)
        {
        case 1:;
            int insertion = 1;
            while (insertion != 2)
            {
                switch (insertion)
                {
                case 1:

                    printf("Enter Bill ID: ");
                    scanf("%d", &ID);

                    printf("Enter Appointment ID: ");
                    scanf("%d", &AppointmentID);

                    printf("Enter Price: ");
                    scanf("%lf", &price);

                    printf("\n\nThe following is the new record data:\n");
                    printf("ID - %d, AppointmentID - %d, Price - %.2lf JOD\n\n", ID, AppointmentID, price);
                    break;

                default:
                    break;
                }

                printf("1- Insert another bill.\n");
                printf("2- Return to main menu.\n");

                scanf("%d", &insertion);
            }

            break;

        default:
            break;
        }

        system("clear");
    }
}

void diagnosisTable()
{
    int MedicalID = 0;
    char diagnosis[256];

    int choice = 0;

    while (choice != 5)
    {
        printf("1- Insert new Diagnosis(s)\n");
        printf("2- Delete Diagnosis(s)\n");
        printf("3- Select Diagnosis(s)\n");
        printf("4- Update Diagnosis(s)\n");
        printf("5- Exit\n");

        scanf("%d", &choice);

        switch (choice)
        {
        case 1:;
            int insertion = 1;
            while (insertion != 2)
            {
                switch (insertion)
                {
                case 1:

                    printf("Enter Medical Record ID: ");
                    scanf("%d", &MedicalID);

                    getchar();

                    printf("Enter Diagnosis: ");
                    fgets(diagnosis, sizeof(diagnosis), stdin);
                    diagnosis[strcspn(diagnosis, "\n")] = 0;

                    printf("\n\nThe following is the new record data:\n");
                    printf("Medical ID - %d, Diagnosis - %s\n\n", MedicalID, diagnosis);
                    break;

                default:
                    break;
                }

                printf("1- Insert another diagnosis.\n");
                printf("2- Return to main menu.\n");

                scanf("%d", &insertion);
            }

            break;

        default:
            break;
        }

        system("clear");
    }
}

void medicalRecordTable()
{
    int ID = 0, PatientSSN = 0;

    int choice = 0;

    while (choice != 5)
    {
        printf("1- Insert new Medical Record(s)\n");
        printf("2- Delete Medical Record(s)\n");
        printf("3- Select Medical Record(s)\n");
        printf("4- Update Medical Record(s)\n");
        printf("5- Exit\n");

        scanf("%d", &choice);

        switch (choice)
        {
        case 1:;
            int insertion = 1;
            while (insertion != 2)
            {
                switch (insertion)
                {
                case 1:

                    printf("Enter Medical Record ID: ");
                    scanf("%d", &ID);

                    printf("Enter Patient SSN: ");
                    scanf("%d", &PatientSSN);

                    printf("\n\nThe following is the new record data:\n");
                    printf("Medical ID - %d, Patient SSN - %d\n\n", ID, PatientSSN);
                    break;

                default:
                    break;
                }

                printf("1- Insert another Medical Record.\n");
                printf("2- Return to main menu.\n");

                scanf("%d", &insertion);
            }

            break;

        default:
            break;
        }

        system("clear");
    }
}

void prescriptionTable()
{
    int MedicalID = 0, AppointmentID = 0;
    char prescription[256];

    int choice = 0;

    while (choice != 5)
    {
        printf("1- Insert new Prescription(s)\n");
        printf("2- Delete Prescription(s)\n");
        printf("3- Select Prescription(s)\n");
        printf("4- Update Prescription(s)\n");
        printf("5- Exit\n");

        scanf("%d", &choice);

        switch (choice)
        {
        case 1:;
            int insertion = 1;
            while (insertion != 2)
            {
                switch (insertion)
                {
                case 1:

                    printf("Enter Medical ID: ");
                    scanf("%d", &MedicalID);

                    printf("Enter Appointment ID: ");
                    scanf("%d", &AppointmentID);

                    getchar();

                    printf("Enter Prescription: ");
                    fgets(prescription, sizeof(prescription), stdin);
                    prescription[strcspn(prescription, "\n")] = 0;

                    printf("\n\nThe following is the new record data:\n");
                    printf("Medical ID - %d, AppointmentID - %d, Prescription - %s\n\n", MedicalID, AppointmentID, prescription);
                    break;

                default:
                    break;
                }

                printf("1- Insert another Prescription.\n");
                printf("2- Return to main menu.\n");

                scanf("%d", &insertion);
            }

            break;

        default:
            break;
        }

        system("clear");
    }
}

void qualificationsTable()
{
    int DoctorID = 0;
    char qualification[256];

    int choice = 0;

    while (choice != 5)
    {
        printf("1- Insert new Qualification(s)\n");
        printf("2- Delete Qualification(s)\n");
        printf("3- Select Qualification(s)\n");
        printf("4- Update Qualification(s)\n");
        printf("5- Exit\n");

        scanf("%d", &choice);

        switch (choice)
        {
        case 1:;
            int insertion = 1;
            while (insertion != 2)
            {
                switch (insertion)
                {
                case 1:

                    printf("Enter Doctor ID: ");
                    scanf("%d", &DoctorID);

                    getchar();

                    printf("Enter Qualification: ");
                    fgets(qualification, sizeof(qualification), stdin);
                    qualification[strcspn(qualification, "\n")] = 0;

                    printf("\n\nThe following is the new record data:\n");
                    printf("Doctor ID - %d, Qualification - %s\n\n", DoctorID, qualification);
                    break;

                default:
                    break;
                }

                printf("1- Insert another Qualification.\n");
                printf("2- Return to main menu.\n");

                scanf("%d", &insertion);
            }

            break;

        default:
            break;
        }

        system("clear");
    }
}

void departmentTable()
{
    int ID = 0;
    char name[256];

    int choice = 0;

    while (choice != 5)
    {
        printf("1- Insert new Department(s)\n");
        printf("2- Delete Department(s)\n");
        printf("3- Select Department(s)\n");
        printf("4- Update Department(s)\n");
        printf("5- Exit\n");

        scanf("%d", &choice);

        switch (choice)
        {
        case 1:;
            int insertion = 1;
            while (insertion != 2)
            {
                switch (insertion)
                {
                case 1:

                    printf("Enter ID: ");
                    scanf("%d", &ID);

                    getchar();

                    printf("Enter Department Name: ");
                    fgets(name, sizeof(name), stdin);
                    name[strcspn(name, "\n")] = 0;

                    printf("\n\nThe following is the new record data:\n");
                    printf("Department ID - %d, Department Name - %s\n\n", ID, name);
                    break;

                default:
                    break;
                }

                printf("1- Insert another Department.\n");
                printf("2- Return to main menu.\n");

                scanf("%d", &insertion);
            }

            break;

        default:
            break;
        }

        system("clear");
    }
}