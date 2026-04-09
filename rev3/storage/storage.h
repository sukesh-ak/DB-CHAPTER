#pragma once

#include "../app/account.h"

#define MAX_ACCOUNTS 100

int storage_init(void);
int storage_add(Account *acc);
int storage_update(Account *acc);
int storage_delete(int acctNum);
int storage_get(int acctNum, Account *out);
int storage_get_all(Account *list, int max);
