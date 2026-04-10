#pragma once

typedef struct {
    int acctNum;
    char firstName[50];
    char lastName[50];
    double balance;
} Account;

// Print one account as a formatted line
void account_print(const Account *acc);
