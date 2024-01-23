#include "bill.h"
#include "appointment.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void loadBillsFromFile(Bill *billIndex[])
{

    FILE *file = fopen("bills.csv", "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return;
    }

    char line[2048];

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

        Bill *newBill = (Bill *)malloc(sizeof(Bill));
        if (newBill == NULL)
        {
            perror("Error allocating memory");
            continue;
        }

        *newBill = (const Bill){0};

        char *token = strtok(line, ",");
        int field = 0;

        while (token != NULL && field < 3)
        {
            switch (field)
            {
            case 0:
                newBill->ID = atoi(token);
                break;

            case 1:
                newBill->appointmentID = atoi(token);
                break;

            case 2:
                newBill->price = atof(token);
                break;
            }
            token = strtok(NULL, ",");
            field++;
        }

        int index = getIndex(newBill->ID);

        if (billIndex[index] == NULL || billIndex[index]->ID > newBill->ID)
        {
            newBill->next = billIndex[index];
            billIndex[index] = newBill;
        }
        else
        {
            Bill *current = billIndex[index];
            while (current->next != NULL && current->next->ID < newBill->ID)
            {
                current = current->next;
            }
            newBill->next = current->next;
            current->next = newBill;
        }
    }

    fclose(file);
}

void saveBillsToFile(Bill *billIndex[])
{
    FILE *file = fopen("bills.csv", "w");
    if (file == NULL)
    {
        perror("Error opening file");
        return;
    }

    fprintf(file, "Bill ID, Appointment ID, Price\n");

    for (int i = 0; i < INDEX_SIZE; i++)
    {
        Bill *current = billIndex[i];
        while (current != NULL)
        {
            fprintf(file, "%d,%d,%lf\n", current->ID, current->appointmentID, current->price);
            current = current->next;
        }
    }

    fclose(file);
}

void freeBillList(Bill *billIndex[])
{
    for (int i = 0; i < INDEX_SIZE; i++)
    {
        Bill *current = billIndex[i];
        while (current != NULL)
        {
            Bill *temp = current;
            current = current->next;
            free(temp);
        }
        billIndex[i] = NULL;
    }
}

void insertBill(Bill newBill)
{

    int continueInsertion = 1;
    while (continueInsertion == 1)
    {
        printf("\n\nAnything with '*' is required.\n\n");

        printf("Enter Bill ID * or 0 to exit: ");
        if (scanf("%d", &newBill.ID) != 1)
        {
            printf("\nBill ID cannot be character.\nTry Again...\n\n");
            while (getchar() != '\n')
                ;
            continue;
        }

        if (newBill.ID == 0)
        {
            break;
        }

        int index = getIndex(newBill.ID);
        Bill *currentBill = billIndex[index];
        int idAlreadyExists = 0;

        while (currentBill != NULL)
        {
            if (currentBill->ID == newBill.ID)
            {
                idAlreadyExists = 1;
                break;
            }
            currentBill = currentBill->next;
        }

        if (idAlreadyExists)
        {
            printf("\nError: A bill with ID %d already exists.\nTry Again...\n\n", newBill.ID);
            continue;
        }

        printf("Enter Appointment ID *: ");
        scanf("%d", &newBill.appointmentID);
        while (getchar() != '\n')
            ;

        int searchIndex = getIndex(newBill.appointmentID);
        Appointment *appointment = appointmentIndex[searchIndex];
        int appointmentExists = 0;

        while (appointment != NULL)
        {
            if (appointment->ID == newBill.appointmentID)
            {
                appointmentExists = 1;
                break;
            }
            appointment = appointment->next;
        }

        if (!appointmentExists)
        {
            printf("\nError: No appointment found with ID %d.\nTry Again...\n\n", newBill.appointmentID);
            continue;
        }

        printf("Enter Bill Price *: ");
        scanf("%lf", &newBill.price);

        Bill *newNode = (Bill *)malloc(sizeof(Bill));
        if (newNode == NULL)
        {
            perror("Error allocating memory");
            continue;
        }
        *newNode = newBill;
        newNode->next = billIndex[index];
        billIndex[index] = newNode;

        printf("\nNew bill added successfully.\n");

        printf("1- Insert another bill.\n");
        printf("2- Return to main menu.\n");
        scanf("%d", &continueInsertion);
        while (getchar() != '\n')
            ;
    }
}

void deleteBill()
{

    int billID, deletion = 1;
    while (deletion != 2)
    {
        printf("Enter ID of Bill to delete or 0 to exit: ");
        scanf("%d", &billID);

        if (billID == 0)
        {
            break;
        }

        int index = getIndex(billID);
        Bill *current = billIndex[index];
        Bill *prev = NULL;

        while (current != NULL)
        {
            if (current->ID == billID)
            {
                if (prev == NULL)
                {
                    billIndex[index] = current->next;
                }
                else
                {
                    prev->next = current->next;
                }
                free(current);
                printf("\n\nBill with ID %d deleted.\n\n", billID);
                break;
            }
            prev = current;
            current = current->next;
        }

        if (current == NULL)
        {
            printf("\n\nBill with ID %d not found.\n\n", billID);
        }

        printf("1- Delete another bill.\n");
        printf("2- Return to main menu.\n");
        scanf("%d", &deletion);
    }
}

void selectBills()
{
    printf("%-15s%-20s%-15s\n", "Bill ID", "Appointment ID", "Price");
    for (int i = 0; i < INDEX_SIZE; i++)
    {
        Bill *current = billIndex[i];
        while (current != NULL)
        {
            printf("%-15d%-20dJOD %-15.2f\n",
                   current->ID, current->appointmentID, current->price);
            current = current->next;
        }
    }
}

void searchBill()
{

    int filterChoice, searchID;
    printf("According to which attribute?\n");
    printf("1- Bill ID\n");
    printf("2- Appointment ID\n");

    scanf("%d", &filterChoice);
    printf("\n\n\n");

    int found = 0;

    switch (filterChoice)
    {
    case 1:
        printf("Enter Bill ID: ");
        scanf("%d", &searchID);
        printf("%-15s%-20s%-15s\n", "Bill ID", "Appointment ID", "Price");
        for (int i = 0; i < INDEX_SIZE; i++)
        {
            Bill *current = billIndex[i];
            while (current != NULL)
            {
                if (current->ID == searchID)
                {
                    printf("%-15d%-20dJOD %-15.2f\n",
                           current->ID, current->appointmentID, current->price);
                    found = 1;
                }
                current = current->next;
            }
        }
        if (!found)
        {
            printf("\nNo bills found with Bill ID %d.\n", searchID);
        }
        break;
    case 2:
        printf("Enter Appointment ID: ");
        scanf("%d", &searchID);
        printf("%-15s%-20s%-15s\n", "Bill ID", "Appointment ID", "Price");
        for (int i = 0; i < INDEX_SIZE; i++)
        {
            Bill *current = billIndex[i];
            while (current != NULL)
            {
                if (current->appointmentID == searchID)
                {
                    printf("%-15d%-20dJOD %-15.2f\n",
                           current->ID, current->appointmentID, current->price);
                    found = 1;
                }
                current = current->next;
            }
        }
        if (!found)
        {
            printf("\nNo bills found for Appointment ID %d.\n", searchID);
        }
        break;
    default:
        printf("Invalid choice.\n");
        break;
    }
}

void updateBill()
{
    int billIDToUpdate;
    printf("Enter Bill ID to update or 0 to exit: ");
    scanf("%d", &billIDToUpdate);

    if (billIDToUpdate == 0)
    {
        return;
    }

    int index = getIndex(billIDToUpdate);
    Bill *current = billIndex[index];

    while (current != NULL && current->ID != billIDToUpdate)
    {
        current = current->next;
    }

    if (current == NULL)
    {
        printf("No record found for Bill ID %d.\n", billIDToUpdate);
    }
    else
    {
        printf("Current Price: %.2f\n", current->price);
        printf("Enter new Price: ");
        scanf("%lf", &current->price);
        printf("Price updated to %.2f for Bill ID %d.\n", current->price, billIDToUpdate);
    }

    printf("\n\nEnter any key to continue\n\n");
    getchar();
    getchar();
}

void billTable(Bill *billIndex[], Appointment *appointmentIndex[])
{
    Bill newBill;
    int choice = 0;

    while (choice != 5)
    {
        system("clear");
        printf("1- Insert new Bill(s)\n");
        printf("2- Delete Bill(s)\n");
        printf("3- Select Bill(s)\n");
        printf("4- Update Bill(s)\n");
        printf("5- Exit\n");

        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            insertBill(newBill);
            break;

        case 2:
            deleteBill();
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
                selectBills();
            }
            else if (selection == 2)
            {
                searchBill();
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
            updateBill();
            break;
        
        default:
            break;
        }
    }
}