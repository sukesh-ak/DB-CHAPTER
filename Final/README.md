# Final -- All Three Storage Backends

This is the finished version of the bank account project. It contains **all three
storage backends** (binary file, JSON, SQLite) in a single codebase, selectable at
compile time by passing the appropriate storage implementation file to gcc.

## Project Structure

```
Final/
  main.c                              <-- entry point (menu)
  app/
    account.h                         <-- Account struct
    account.c                         <-- print helper
    account_service.h                 <-- service interface
    account_service.c                 <-- business logic
  storage/
    storage.h                         <-- common storage interface (6 functions)
    file/
      storage_file.c                  <-- binary file backend (fread/fwrite)
    cjson/
      storage_cjson.c                 <-- JSON backend
      cJSON.h, cJSON.c               <-- cJSON library
    sqlite/
      storage_sqlite.c               <-- SQLite backend
      sqlite3.h, sqlite3.c           <-- SQLite library
  data/                               <-- runtime data files
  build.sh                            <-- Linux/macOS build script
  build.bat                           <-- Windows build script
```

## How It Works

Each storage backend (`storage_file.c`, `storage_cjson.c`, `storage_sqlite.c`)
implements the same 6 functions declared in `storage.h`. To select a backend, you
simply pass the corresponding `.c` file to gcc. All three compile commands share the
same `main.c` and `account_service.c` -- only the storage implementation file and
linked libraries differ.

## How to Build

### Build all three at once

**Linux / macOS:**
```bash
./build.sh
```

**Windows (Developer Command Prompt):**
```cmd
build.bat
```

### Build one at a time

```bash
mkdir -p data

# Binary file backend
gcc -o bank_file main.c app/account.c app/account_service.c storage/file/storage_file.c

# JSON backend
gcc -o bank_json main.c app/account.c app/account_service.c storage/cjson/storage_cjson.c storage/cjson/cJSON.c -lm

# SQLite backend
gcc -o bank_sqlite main.c app/account.c app/account_service.c storage/sqlite/storage_sqlite.c storage/sqlite/sqlite3.c -lpthread -ldl -lm
```

## How to Run

```bash
./bank_file      # uses data/accounts.dat  (binary)
./bank_json      # uses data/accounts.json (human-readable)
./bank_sqlite    # uses data/accounts.db   (database)
```

All three executables behave identically -- same menu, same features. The difference
is what appears in the `data/` folder.

## The Three Backends Compared

| | Binary File | JSON | SQLite |
|---|---|---|---|
| **Data file** | `accounts.dat` | `accounts.json` | `accounts.db` |
| **Readable?** | No (binary) | Yes (text editor) | With a DB viewer |
| **Library** | None (standard C) | cJSON | SQLite |
| **How it stores** | Fixed-size records at calculated offsets | Parse entire file, modify tree, rewrite file | SQL queries with indexed lookup |
| **Implementation** | `storage_file.c` | `storage_cjson.c` | `storage_sqlite.c` |

## What This Demonstrates

1. **Storage abstraction** -- `storage.h` defines 6 functions. Three completely
   different implementations exist behind that interface. The application code
   (`main.c`, `account_service.c`) never changed across any of them.

2. **Same interface, different trade-offs** -- binary files are fast but unreadable,
   JSON is readable but must rewrite the whole file on every change, SQLite gives
   indexed lookups but adds a larger library dependency.

## Try It

1. Build all three: `./build.sh`
2. Run `./bank_file`, create some accounts, then try to open `data/accounts.dat`
   in a text editor -- it is unreadable binary.
3. Run `./bank_json`, create some accounts, then open `data/accounts.json` --
   you can read and even hand-edit the data.
4. Run `./bank_sqlite`, create some accounts, then open `data/accounts.db` with
   a SQLite viewer (like "DB Browser for SQLite") -- you see a proper database table.
5. Notice that the menu and behavior are identical across all three.
