#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_NAME "accounts.dat"
#define MAX_NAME 50
#define RECORDS 100 // Maximum number of accounts


// Structure for account record
typedef struct
{
    int accountNumber;       // Unique account number
    char firstName[MAX_NAME]; // Account holder first name
    char lastName[MAX_NAME];  // Account holder last name
    double balance;          // Account balance
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

    Account blank = {0, "", "", 0.0};
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
    getchar(); // clear newline left by scanf
    printf("Enter first name: ");
    fgets(acc.firstName, MAX_NAME, stdin);
    acc.firstName[strcspn(acc.firstName, "\n")] = '\0';
    printf("Enter last name: ");
    fgets(acc.lastName, MAX_NAME, stdin);
    acc.lastName[strcspn(acc.lastName, "\n")] = '\0';
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
    Account blank = {0, "", "", 0.0};
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
        printf("\nAccount Number: %d\nName: %s %s\nBalance: %.2f\n",
               acc.accountNumber, acc.firstName, acc.lastName, acc.balance);
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
    printf("\n%-10s %-16s %-16s %-10s\n", "Acc No", "First Name", "Last Name", "Balance");
    printf("--------------------------------------------------------------\n");

    while (fread(&acc, sizeof(Account), 1, fp) == 1)
    {
        if (acc.accountNumber != 0)
        {
            printf("%-10d %-16s %-16s %-10.2f\n", acc.accountNumber, acc.firstName, acc.lastName, acc.balance);
        }
    }
    fclose(fp);
}

// Calculate file position for a given account number
long getFilePosition(int accountNumber)
{
    return (accountNumber - 1) * sizeof(Account);
}
