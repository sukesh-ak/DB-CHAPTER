#include <stdio.h>
#include <string.h>
#include "account.h"
#include "account_service.h"
#include "../storage/storage.h"

int account_service_init(void) {
    return storage_init();
}

void account_service_cleanup(void) {
    // nothing to clean up for now
}

void account_service_list(void) {
    Account accounts[MAX_ACCOUNTS];
    int count = storage_get_all(accounts, MAX_ACCOUNTS);

    if (count == 0) {
        puts("No accounts found.");
        return;
    }

    printf("%-6s%-16s%-11s%10s\n",
           "Acct", "Last Name", "First Name", "Balance");
    for (int i = 0; i < count; i++) {
        account_print(&accounts[i]);
    }
}

void account_service_create(void) {
    Account acc = {0, "", "", 0.0};
    int num;

    printf("Enter new account number (1 - 100): ");
    scanf("%d", &num);

    // Check if account already exists
    Account existing;
    if (storage_get(num, &existing) == 0) {
        printf("Account #%d already exists.\n", num);
        return;
    }

    printf("Enter lastname firstname balance\n? ");
    scanf("%49s%49s%lf", acc.lastName, acc.firstName, &acc.balance);
    acc.acctNum = num;

    if (storage_add(&acc) == 0) {
        puts("Account created.");
    } else {
        puts("Error creating account.");
    }
}

void account_service_update(void) {
    int num;
    double transaction;
    Account acc;

    printf("Enter account to update (1 - 100): ");
    scanf("%d", &num);

    if (storage_get(num, &acc) != 0) {
        printf("Account #%d has no information.\n", num);
        return;
    }

    account_print(&acc);
    printf("\nEnter charge (+) or payment (-): ");
    scanf("%lf", &transaction);
    acc.balance += transaction;

    account_print(&acc);
    storage_update(&acc);
}

void account_service_delete(void) {
    int num;

    printf("Enter account number to delete (1 - 100): ");
    scanf("%d", &num);

    if (storage_delete(num) != 0) {
        printf("Account %d does not exist.\n", num);
    } else {
        puts("Account deleted.");
    }
}
