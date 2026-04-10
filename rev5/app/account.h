#pragma once

#define MAX_NAME 50

typedef struct {
    int acctNum;
    char firstName[MAX_NAME];
    char lastName[MAX_NAME];
    double balance;
} Account;

// Print one account as a formatted line
void account_print(const Account *acc);
