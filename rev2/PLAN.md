# Rev 2 -- Assignment: Rewrite the Program Cleanly

## Starting Point

You have `rev1/trans.c` -- a working bank account program from the textbook. It uses
`struct clientData`, has cryptic function names like `newRecord` and `textFile`, does
no input validation, and crashes if the data file does not exist.

Your job is to **rewrite this program from scratch in a single file** (`trans.c`) that
does the same thing, but better.

## What You Must Produce

```
rev2/
  trans.c          <-- your rewritten program (single file)
```

## Requirements

### 1. Use `typedef` for the struct

Replace the old-style `struct clientData` with a `typedef`. Your struct should look
like this:

```c
typedef struct
{
    int accountNumber;
    char name[30];
    double balance;
} Account;
```

After this, you write `Account` everywhere instead of `struct clientData`.

### 2. Use meaningful function names

Your program must have these functions:

| Function | Purpose |
|----------|---------|
| `initializeFile()` | Create the data file with blank records if it does not exist |
| `createAccount()` | Add a new account |
| `updateAccount()` | Deposit to or withdraw from an existing account |
| `deleteAccount()` | Remove an existing account (overwrite with a blank record) |
| `viewAccount()` | Display a single account's details |
| `listAccounts()` | Display all accounts in a formatted table |
| `getFilePosition(int accountNumber)` | Return the byte offset for a given account number |

### 3. The menu must include all operations

```
=== Bank Account System ===
1. Create Account
2. Update Account Balance
3. Delete Account
4. View Account
5. List All Accounts
0. Exit
```

Use `0` for exit (not `5` like in Rev 1).

### 4. Validate user input

- Check `scanf` return values. If `scanf` fails (user types a letter instead of a
  number), print an error message instead of crashing.
- Reject account numbers outside the valid range (1 to 100).
- Use `perror()` when file operations fail, so the user sees the actual system error.

### 5. Auto-create the data file

Rev 1 crashes if `credit.dat` does not exist. Your program must check whether the
file exists on startup. If it does not, create it by writing 100 blank `Account`
records.

Use `"accounts.dat"` as the filename (not `"credit.dat"`).

### 6. Handle string input with spaces

Rev 1 uses `scanf("%s", ...)` for names, which breaks on spaces. Use `fgets()` to
read the account holder name so that names like "Alice Smith" work correctly.
Remember to strip the trailing newline that `fgets` leaves behind.

**Hint:** `strcspn` can find the position of the newline character.

### 7. Delete account

Deleting an account means overwriting its record in the file with a blank `Account`
(all zeroes). Before deleting, check that the account actually exists. If it does
not, print an error.

## Concepts You Will Use

- `typedef struct { ... } Name;` -- giving a struct a short name
- `fopen`, `fclose`, `fseek`, `fread`, `fwrite` -- binary file I/O
- `fgets` and `strcspn` -- reading strings safely
- `perror` -- printing system error messages
- Return value checking -- `scanf` returns the number of items successfully read

## How to Test

1. Compile: `gcc -o trans trans.c`
2. Run: `./trans`
3. Create two or three accounts.
4. List all accounts -- verify the table looks correct.
5. View a single account by number.
6. Update one account's balance.
7. Delete one account, then list again to confirm it is gone.
8. Try invalid inputs: letters instead of numbers, account number 0, account
   number 999. The program should print errors, not crash.
9. Exit and re-run the program. Your accounts should still be there (they are
   saved in `accounts.dat`).

## What NOT to Do

- Do not split into multiple files yet. That is Rev 3.
- Do not use any external libraries. This is plain C with `stdio.h`, `stdlib.h`,
  and `string.h`.
