#include <stdio.h>
#include "account.h"

void account_print(const Account *acc) {
    printf("%-6d%-16s%-11s%10.2f\n",
           acc->acctNum, acc->lastName,
           acc->firstName, acc->balance);
}
