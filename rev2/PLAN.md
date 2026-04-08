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
    char firstName[50];
    char lastName[50];
    double balance;
} Account;
```

After this, you write `Account` everywhere instead of `struct clientData`.

**Note:** Rev 1 had `firstName` and `lastName` as separate fields. We keep them
separate here too, which will make the transition to Rev 3 seamless.

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

### 6. Use `fgets` to read names safely

Prompt the user for first name and last name as separate inputs. Use `fgets()`
instead of `scanf("%s", ...)` to read each name:

```c
fgets(acc.firstName, MAX_NAME, stdin);
```

`fgets` is safer than `scanf` for strings because:
- It respects the buffer size you pass (`MAX_NAME`), preventing overflow
- It can read names that contain spaces (e.g., "Mary Jane")

**Gotcha:** `fgets` includes the trailing newline in the string. Strip it with:
```c
acc.firstName[strcspn(acc.firstName, "\n")] = '\0';
```

**Gotcha:** After a `scanf("%d", ...)` call, there is a leftover newline in the
input buffer. Call `getchar()` once before the first `fgets` to consume it,
otherwise `fgets` will read an empty line.

### 7. Delete account

Deleting an account means overwriting its record in the file with a blank `Account`
(all zeroes). Before deleting, check that the account actually exists. If it does
not, print an error.

## Concepts You Will Use

- `typedef struct { ... } Name;` -- giving a struct a short name
- `#define` constants -- `MAX_NAME`, `RECORDS`, `FILE_NAME` so nothing is hardcoded
- `fopen`, `fclose`, `fseek`, `fread`, `fwrite` -- binary file I/O
- `fgets` and `strcspn` -- reading strings safely with buffer size limits
- `getchar()` -- clearing leftover newlines from the input buffer
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
