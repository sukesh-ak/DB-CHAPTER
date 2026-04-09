#include <stdio.h>
#include <string.h>
#include "storage.h"

// Path to the binary data file
static const char *DATA_FILE = "data/accounts.dat";

// Helper: calculate file position for an account number
static long file_position(int acctNum) {
    return (acctNum - 1) * sizeof(Account);
}

int storage_init(void) {
    // If file already exists, nothing to do
    FILE *f = fopen(DATA_FILE, "rb");
    if (f) {
        fclose(f);
        return 0;
    }

    // Create file with empty records
    f = fopen(DATA_FILE, "wb");
    if (!f) return -1;

    Account blank = {0, "", "", 0.0};
    for (int i = 0; i < MAX_ACCOUNTS; i++) {
        fwrite(&blank, sizeof(Account), 1, f);
    }
    fclose(f);
    return 0;
}

int storage_add(Account *acc) {
    FILE *f = fopen(DATA_FILE, "rb+");
    if (!f) return -1;

    // Check if account already exists
    Account existing;
    fseek(f, file_position(acc->acctNum), SEEK_SET);
    fread(&existing, sizeof(Account), 1, f);

    if (existing.acctNum != 0) {
        fclose(f);
        return -1;  // already exists
    }

    // Write the new account
    fseek(f, file_position(acc->acctNum), SEEK_SET);
    fwrite(acc, sizeof(Account), 1, f);
    fclose(f);
    return 0;
}

int storage_update(Account *acc) {
    FILE *f = fopen(DATA_FILE, "rb+");
    if (!f) return -1;

    // Check if account exists
    Account existing;
    fseek(f, file_position(acc->acctNum), SEEK_SET);
    fread(&existing, sizeof(Account), 1, f);

    if (existing.acctNum == 0) {
        fclose(f);
        return -1;  // not found
    }

    // Overwrite with updated account
    fseek(f, file_position(acc->acctNum), SEEK_SET);
    fwrite(acc, sizeof(Account), 1, f);
    fclose(f);
    return 0;
}

int storage_delete(int acctNum) {
    FILE *f = fopen(DATA_FILE, "rb+");
    if (!f) return -1;

    // Check if account exists
    Account existing;
    fseek(f, file_position(acctNum), SEEK_SET);
    fread(&existing, sizeof(Account), 1, f);

    if (existing.acctNum == 0) {
        fclose(f);
        return -1;  // not found
    }

    // Overwrite with blank record
    Account blank = {0, "", "", 0.0};
    fseek(f, file_position(acctNum), SEEK_SET);
    fwrite(&blank, sizeof(Account), 1, f);
    fclose(f);
    return 0;
}

int storage_get(int acctNum, Account *out) {
    FILE *f = fopen(DATA_FILE, "rb");
    if (!f) return -1;

    fseek(f, file_position(acctNum), SEEK_SET);
    fread(out, sizeof(Account), 1, f);
    fclose(f);

    if (out->acctNum == 0) {
        return -1;  // not found
    }
    return 0;
}

int storage_get_all(Account *list, int max) {
    FILE *f = fopen(DATA_FILE, "rb");
    if (!f) return 0;

    Account acc;
    int count = 0;

    while (fread(&acc, sizeof(Account), 1, f) == 1 && count < max) {
        if (acc.acctNum != 0) {
            list[count] = acc;
            count++;
        }
    }

    fclose(f);
    return count;
}
