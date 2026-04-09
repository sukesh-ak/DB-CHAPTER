# Rev 3 -- Assignment: Split Into Multiple Files

## Starting Point

You have a working single-file program from Rev 2. It is clean and readable, but
everything -- the menu, the logic, and the file I/O -- lives in one file. As programs
grow, this becomes unmanageable.

Your job is to **split the program into multiple files** and introduce a **storage
abstraction layer** that will make Rev 4 and Rev 5 possible.

## What You Must Produce

```
rev3/
  main.c                        <-- entry point: menu loop only
  app/
    account.h                   <-- Account struct definition + helper declaration
    account.c                   <-- Account helper function implementation
    account_service.h           <-- service function declarations
    account_service.c           <-- business logic (calls storage functions)
  storage/
    storage.h                   <-- storage interface (declarations only, no code)
    storage_file.c              <-- storage implementation using binary file I/O
  data/                         <-- runtime data directory (created at build time)
```

## Step-by-Step Guidance

### Step 1: Create `app/account.h`

This header file defines the `Account` struct that every other file will use.

It must contain:
- An include guard (`#pragma once` or `#ifndef`/`#define`/`#endif`)
- `#define MAX_NAME 50` -- the buffer size constant, same as Rev 2
- The `Account` typedef with these fields:
  - `int acctNum`
  - `char firstName[MAX_NAME]`
  - `char lastName[MAX_NAME]`
  - `double balance`
- A declaration for `account_print(const Account *acc)` -- a helper that prints
  one account as a formatted line

**Note:** The struct fields match Rev 2 (`firstName`, `lastName`), but we rename
`accountNumber` to `acctNum` for brevity. The `MAX_NAME` constant lives here
because any file that includes `account.h` may need it (e.g., for `fgets` calls).

### Step 2: Create `app/account.c`

This file implements `account_print`. It should:
- Include `stdio.h` and `"account.h"`
- Print one account in a formatted line (account number, last name, first name,
  balance)

### Step 3: Create `storage/storage.h` -- THE KEY FILE

This is the most important file in the project. It declares **what** storage can
do, without implementing **how**.

It must contain:
- An include guard
- `#include "../app/account.h"`
- `#define MAX_ACCOUNTS 100`
- These six function declarations:

```
int storage_init(void);                       -- set up the storage backend
int storage_add(Account *acc);                -- add a new account (fail if exists)
int storage_update(Account *acc);             -- update an existing account
int storage_delete(int acctNum);              -- delete an account by number
int storage_get(int acctNum, Account *out);   -- retrieve one account
int storage_get_all(Account *list, int max);  -- retrieve all accounts
```

**Return value convention:**
- Return `0` on success, `-1` on failure
- `storage_get_all` returns the count of accounts found

**Do not write any function bodies in this file.** Only declarations.

### Step 4: Create `storage/storage_file.c`

This file implements all six functions from `storage.h` using binary file I/O
(`fopen`, `fseek`, `fread`, `fwrite`).

It must include `"storage.h"` (not `"../app/account.h"` -- that comes through
`storage.h` already).

Implementation hints:
- Use a static file path like `"data/accounts.dat"`
- Write a small helper `file_position(int acctNum)` that returns
  `(acctNum - 1) * sizeof(Account)` -- the byte offset for direct access
- `storage_init`: check if the file exists; if not, create it with 100 blank
  records
- `storage_add`: seek to the account's position, read it, check if
  `acctNum == 0` (empty slot), then write the new account
- `storage_delete`: overwrite the record with a blank `Account`
- `storage_get_all`: read all records, collect the ones where `acctNum != 0`

### Step 5: Create `app/account_service.h`

Declare the functions that `main.c` will call:

```
int  account_service_init(void);
void account_service_cleanup(void);
void account_service_list(void);
void account_service_create(void);
void account_service_update(void);
void account_service_delete(void);
```

### Step 6: Create `app/account_service.c`

This is the bridge between the menu and the storage layer. It handles **user
interaction** -- reading input, printing messages -- and calls `storage_*`
functions to do the actual data work.

It must include:
- `"account.h"`, `"account_service.h"`, and `"../storage/storage.h"`

Each function should:
- `account_service_init`: call `storage_init()` and return its result
- `account_service_create`: ask for account number, check if it already exists
  (using `storage_get`), ask for first name, last name, and balance, call
  `storage_add`
- `account_service_update`: ask for account number, fetch it with `storage_get`,
  display it, ask for a transaction amount, update the balance, call
  `storage_update`
- `account_service_delete`: ask for account number, call `storage_delete`,
  print success or error
- `account_service_list`: call `storage_get_all`, then loop and print each
  account using `account_print`

**Input handling:** Use `fgets(buf, MAX_NAME, stdin)` for reading names, just
like in Rev 2. Remember to strip the trailing newline with `strcspn` and to
call `getchar()` after `scanf("%d", ...)` to clear the leftover newline before
the first `fgets`.

**Important rule:** This file must never call `fopen`, `fread`, `fwrite`, or any
file I/O function directly. All data access goes through `storage_*` functions.

### Step 7: Create `main.c`

The entry point. It must:
- Include `"app/account_service.h"`
- Call `account_service_init()` on startup (exit if it fails)
- Show a menu with these options:
  ```
  1 - List all accounts
  2 - Update an account
  3 - Add a new account
  4 - Delete an account
  5 - Exit
  ```
- Loop until the user picks 5
- Call `account_service_cleanup()` before exiting

**Important rule:** `main.c` must not include `storage.h` or know anything about
how data is stored. It only talks to the service layer.

## How to Compile

You must compile **all** `.c` files together. Never compile `.h` files.

**Linux / macOS:**
```bash
mkdir -p data
gcc -o bank main.c app/account.c app/account_service.c storage/storage_file.c
```

**Windows (Developer Command Prompt):**
```cmd
if not exist data mkdir data
cl /Fe:bank.exe main.c app\account.c app\account_service.c storage\storage_file.c
```

## How to Test

1. The program must behave identically to Rev 2: create, update, delete, view,
   and list accounts.
2. Verify that `data/accounts.dat` is created automatically on first run.
3. Check that deleting an account works and the account disappears from the list.

## Common Errors and What They Mean

| Error | What went wrong |
|-------|----------------|
| `undefined reference to 'storage_init'` | You forgot to include `storage_file.c` in the compile command |
| `undefined reference to 'account_print'` | You forgot to include `account.c` in the compile command |
| `fatal error: account.h: No such file` | You are compiling from the wrong directory -- run `gcc` from `rev3/` |
| `redefinition of 'Account'` | Missing include guard in `account.h` |

## Design Rules to Follow

1. **`main.c` only knows about `account_service.h`** -- it never includes
   `storage.h` or `account.h` directly.
2. **`account_service.c` only knows about `storage.h`** -- it never calls
   `fopen`/`fread`/`fwrite` directly.
3. **`storage_file.c` only knows about `storage.h`** -- it never calls `printf`
   to interact with the user.
4. **No function bodies in `.h` files** -- headers contain only declarations.
