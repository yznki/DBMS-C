#include "utils.h"
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

int getIndex(int ID)
{
    return ID % INDEX_SIZE;
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