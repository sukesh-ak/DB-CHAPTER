# Rev 3 -- Multi-File Project with Storage Abstraction

## Where We Are

Rev 2 gave us clean, readable code -- but it is still one big file. In a real project,
you do not put the menu, the business logic, and the file I/O all in the same place.

Rev 3 takes the **exact same program** and splits it across multiple files. The behavior
is identical -- same menu, same features, same binary file format. The difference is
in how the code is organized.

## What Changed from Rev 2

| Area | Rev 2 | Rev 3 |
|------|-------|-------|
| Files | 1 file (`trans.c`) | 6 files across 3 folders |
| Structure | Everything mixed together | Menu, logic, and storage separated |
| Data format | Binary (`accounts.dat`) | Still binary -- same format, different code organization |
| Compilation | `gcc trans.c` | `gcc main.c app/account.c app/account_service.c storage/storage_file.c` |

## Project Structure

```
rev3/
  main.c                     <-- entry point: menu and user interaction
  app/
    account.h                <-- Account struct definition
    account.c                <-- Account helper (print formatting)
    account_service.h        <-- service function declarations
    account_service.c        <-- business logic (create, update, delete, list)
  storage/
    storage.h                <-- storage interface (declarations only)
    storage_file.c           <-- storage implementation using binary file I/O
  data/
    accounts.dat             <-- runtime data file (created automatically)
  build.sh                   <-- Linux/macOS build script
  build.bat                  <-- Windows build script
```

## What You Will Learn

1. **Header files (`.h`)** -- declaring structs and function signatures that other
   files can use. A header is a "table of contents" for a `.c` file.
2. **Include guards (`#pragma once`)** -- preventing a header from being included
   twice, which would cause "redefinition" errors.
3. **Separation of concerns** -- menu code does not touch files; storage code does
   not print menus. Each file has one job.
4. **The storage abstraction** -- `storage.h` declares *what* storage can do (add,
   get, update, delete) without saying *how*. The implementation lives in
   `storage_file.c`. This is the key idea that makes Rev 4 and Rev 5 possible.
5. **Multi-file compilation** -- telling `gcc` about all your `.c` files so the
   linker can connect function calls to their implementations.

## The Key File: `storage/storage.h`

This header declares six functions:

```c
int storage_init(void);
int storage_add(Account *acc);
int storage_update(Account *acc);
int storage_delete(int acctNum);
int storage_get(int acctNum, Account *out);
int storage_get_all(Account *list, int max);
```

`account_service.c` calls these functions. It does not know (or care) whether data
is stored in a binary file, a JSON file, or a database. It only knows the interface.

Right now, `storage_file.c` provides the implementation using `fread`/`fwrite`.
In Rev 4, we will swap in a completely different implementation -- without touching
`main.c` or `account_service.c` at all.

## How to Compile and Run

**Linux / macOS:**
```bash
./build.sh
./bank
```

Or manually:
```bash
mkdir -p data
gcc -o bank main.c app/account.c app/account_service.c storage/storage_file.c
./bank
```

**Windows (Developer Command Prompt):**
```cmd
build.bat
bank.exe
```

## Common Errors

| Error | Cause | Fix |
|-------|-------|-----|
| `undefined reference to 'storage_init'` | Forgot `storage_file.c` in compile command | Add it to the `gcc` command |
| `fatal error: account.h: No such file` | Compiling from the wrong directory | Run `gcc` from the `rev3/` folder |
| `multiple definition of 'Account'` | Header included twice without include guard | Use `#pragma once` at the top of each `.h` file |

## Try It

1. Compile and run -- everything should work exactly like Rev 2.
2. Open `storage/storage.h` and read the six function declarations. This is the
   contract that any storage backend must fulfill.
3. Open `storage/storage_file.c` and see how each function is implemented with
   `fseek`, `fread`, `fwrite`.
4. Open `app/account_service.c` and notice it calls `storage_add()`, `storage_get()`,
   etc. -- it never opens a file directly.

## What Is Next

The binary file works, but you cannot open `accounts.dat` in a text editor and read
it. In Rev 4, we replace the storage backend with JSON -- human-readable data -- by
swapping in a single new file.
