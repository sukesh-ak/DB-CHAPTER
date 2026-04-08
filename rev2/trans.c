#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_NAME "accounts.dat"
#define MAX_NAME 30
#define RECORDS 100 // Maximum number of accounts

// Structure for account record
typedef struct
{
    int accountNumber;   // Unique account number
    char name[MAX_NAME]; // Account holder name
    double balance;      // Account balance
} Account;

// Function prototypes
void initializeFile();
void createAccount();
void updateAccount();
void deleteAccount();
void viewAccount();
void listAccounts();
long getFilePosition(int accountNumber);

int main()
{
    int choice;

    // Ensure file exists
    FILE *fp = fopen(FILE_NAME, "rb");
    if (!fp)
    {
        initializeFile();
    }
    else
    {
        fclose(fp);
    }

    do
    {
        printf("\n=== Bank Account System ===\n");
        printf("1. Create Account\n");
        printf("2. Update Account Balance\n");
        printf("3. Delete Account\n");
        printf("4. View Account\n");
        printf("5. List All Accounts\n");
        printf("0. Exit\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input. Exiting.\n");
            break;
        }

        switch (choice)
        {
        case 1:
            createAccount();
            break;
        case 2:
            updateAccount();
            break;
        case 3:
            deleteAccount();
            break;
        case 4:
            viewAccount();
            break;
        case 5:
            listAccounts();
            break;
        case 0:
            printf("Goodbye!\n");
            break;
        default:
            printf("Invalid choice.\n");
        }
    } while (choice != 0);

    return 0;
}

// Initialize file with empty records
void initializeFile()
{
    FILE *fp = fopen(FILE_NAME, "wb");
    if (!fp)
    {
        perror("Error creating file");
        exit(EXIT_FAILURE);
    }

    Account blank = {0, "", 0.0};
    for (int i = 0; i < RECORDS; i++)
    {
        fwrite(&blank, sizeof(Account), 1, fp);
    }
    fclose(fp);
}

// Create a new account
void createAccount()
{
    int accNum;
    Account acc;

    printf("Enter new account number (1-%d): ", RECORDS);
    if (scanf("%d", &accNum) != 1 || accNum < 1 || accNum > RECORDS)
    {
        printf("Invalid account number.\n");
        return;
    }

    FILE *fp = fopen(FILE_NAME, "rb+");
    if (!fp)
    {
        perror("Error opening file");
        return;
    }

    fseek(fp, getFilePosition(accNum), SEEK_SET);
    fread(&acc, sizeof(Account), 1, fp);

    if (acc.accountNumber != 0)
    {
        printf("Account already exists.\n");
        fclose(fp);
        return;
    }

    acc.accountNumber = accNum;
    printf("Enter account holder name: ");
    getchar(); // clear newline
    fgets(acc.name, MAX_NAME, stdin);
    acc.name[strcspn(acc.name, "\n")] = '\0'; // remove newline
    printf("Enter initial balance: ");
    if (scanf("%lf", &acc.balance) != 1)
    {
        printf("Invalid balance.\n");
        fclose(fp);
        return;
    }

    fseek(fp, getFilePosition(accNum), SEEK_SET);
    fwrite(&acc, sizeof(Account), 1, fp);
    fclose(fp);

    printf("Account created successfully.\n");
}

// Update account balance
void updateAccount()
{
    int accNum;
    double amount;
    Account acc;

    printf("Enter account number to update: ");
    if (scanf("%d", &accNum) != 1 || accNum < 1 || accNum > RECORDS)
    {
        printf("Invalid account number.\n");
        return;
    }

    FILE *fp = fopen(FILE_NAME, "rb+");
    if (!fp)
    {
        perror("Error opening file");
        return;
    }

    fseek(fp, getFilePosition(accNum), SEEK_SET);
    fread(&acc, sizeof(Account), 1, fp);

    if (acc.accountNumber == 0)
    {
        printf("Account not found.\n");
        fclose(fp);
        return;
    }

    printf("Enter amount to deposit (+) or withdraw (-): ");
    if (scanf("%lf", &amount) != 1)
    {
        printf("Invalid amount.\n");
        fclose(fp);
        return;
    }

    acc.balance += amount;
    fseek(fp, getFilePosition(accNum), SEEK_SET);
    fwrite(&acc, sizeof(Account), 1, fp);
    fclose(fp);

    printf("Account updated successfully.\n");
}

// Delete an existing account
void deleteAccount()
{
    int accNum;
    Account acc;

    printf("Enter account number to delete (1-%d): ", RECORDS);
    if (scanf("%d", &accNum) != 1 || accNum < 1 || accNum > RECORDS)
    {
        printf("Invalid account number.\n");
        return;
    }

    FILE *fp = fopen(FILE_NAME, "rb+");
    if (!fp)
    {
        perror("Error opening file");
        return;
    }

    fseek(fp, getFilePosition(accNum), SEEK_SET);
    fread(&acc, sizeof(Account), 1, fp);

    if (acc.accountNumber == 0)
    {
        printf("Account %d does not exist.\n", accNum);
        fclose(fp);
        return;
    }

    // Overwrite with blank record
    Account blank = {0, "", 0.0};
    fseek(fp, getFilePosition(accNum), SEEK_SET);
    fwrite(&blank, sizeof(Account), 1, fp);
    fclose(fp);

    printf("Account deleted successfully.\n");
}

// View a single account
void viewAccount()
{
    int accNum;
    Account acc;

    printf("Enter account number to view: ");
    if (scanf("%d", &accNum) != 1 || accNum < 1 || accNum > RECORDS)
    {
        printf("Invalid account number.\n");
        return;
    }

    FILE *fp = fopen(FILE_NAME, "rb");
    if (!fp)
    {
        perror("Error opening file");
        return;
    }

    fseek(fp, getFilePosition(accNum), SEEK_SET);
    fread(&acc, sizeof(Account), 1, fp);
    fclose(fp);

    if (acc.accountNumber == 0)
    {
        printf("Account not found.\n");
    }
    else
    {
        printf("\nAccount Number: %d\nName: %s\nBalance: %.2f\n",
               acc.accountNumber, acc.name, acc.balance);
    }
}

// List all accounts
void listAccounts()
{
    FILE *fp = fopen(FILE_NAME, "rb");
    if (!fp)
    {
        perror("Error opening file");
        return;
    }

    Account acc;
    printf("\n%-10s %-30s %-10s\n", "Acc No", "Name", "Balance");
    printf("----------------------------------------------------------\n");

    while (fread(&acc, sizeof(Account), 1, fp) == 1)
    {
        if (acc.accountNumber != 0)
        {
            printf("%-10d %-30s %-10.2f\n", acc.accountNumber, acc.name, acc.balance);
        }
    }
    fclose(fp);
}

// Calculate file position for a given account number
long getFilePosition(int accountNumber)
{
    return (accountNumber - 1) * sizeof(Account);
}
