#ifndef BILL_H
#define BILL_H
#include "appointment.h"

typedef struct Bill
{
    int ID, appointmentID;
    double price;

    struct Bill *next;

} Bill;

Bill *billIndex[100];

void loadBillsFromFile(Bill *billIndex[]);
void saveBillsToFile(Bill *billIndex[]);
void freeBillList(Bill *billIndex[]);
void billTable(Bill *billIndex[], Appointment *appointmentIndex[]);

#endif