# Rev 5 -- SQLite Database Storage

## Where We Are

Rev 4 proved the storage abstraction works -- we swapped binary files for JSON without
touching the menu or business logic. Now we do it again.

Rev 5 replaces JSON with **SQLite**, a real database engine. Once again, we only swap
**one file** in the compile command. `main.c` and `account_service.c` remain identical.

## What Changed from Rev 4

| Area | Rev 4 | Rev 5 |
|------|-------|-------|
| Data format | JSON text file (`accounts.json`) | SQLite database (`accounts.db`) |
| Storage code | `storage/cjson/storage_cjson.c` | `storage/sqlite/storage_sqlite.c` |
| External library | cJSON | SQLite (`sqlite3.h` + `sqlite3.c`) |
| Data access | Read entire file, scan for record | SQL queries -- direct lookup by primary key |
| Menu, service layer | Unchanged | **Identical** -- still not a single line changed |

## Project Structure

```
rev5/
  main.c                              <-- unchanged from Rev 3 and Rev 4
  app/
    account.h                         <-- unchanged
    account.c                         <-- unchanged
    account_service.h                 <-- unchanged
    account_service.c                 <-- unchanged
  storage/
    storage.h                         <-- unchanged (same 6 function declarations)
    sqlite/
      storage_sqlite.c               <-- NEW: implements storage.h using SQL
      sqlite3.h                       <-- third-party library (from sqlite.org)
      sqlite3.c                       <-- third-party library
  data/
    accounts.db                       <-- runtime database file
  build.sh                            <-- updated compile command
  build.bat                           <-- updated compile command
```

## What You Will Learn

1. **What SQLite is** -- a database that lives in a single file. No server to install,
   no configuration. Just link the library and call functions.
2. **Basic SQL** -- the four statements you need:
   ```sql
   CREATE TABLE IF NOT EXISTS accounts (...);   -- set up the table
   INSERT INTO accounts (...) VALUES (...);     -- add a record
   SELECT * FROM accounts WHERE acctNum = ?;    -- find a record
   UPDATE accounts SET ... WHERE acctNum = ?;   -- change a record
   DELETE FROM accounts WHERE acctNum = ?;      -- remove a record
   ```
3. **Prepared statements** -- using `sqlite3_prepare_v2` / `sqlite3_bind_*` /
   `sqlite3_step` / `sqlite3_finalize` to safely execute SQL with parameters.
   This prevents SQL injection and handles special characters in data.
4. **Reading query results** -- using `sqlite3_column_int`, `sqlite3_column_text`,
   and `sqlite3_column_double` to extract values from result rows.
5. **The full storage abstraction payoff** -- across Rev 3, Rev 4, and Rev 5, the
   files `main.c`, `account_service.c`, `account.c`, and `account.h` are identical.
   Only the storage implementation and compile command changed.

## How the Swap Works

Rev 4 compile command:
```bash
gcc ... storage/cjson/storage_cjson.c storage/cjson/cJSON.c -lm
```

Rev 5 compile command:
```bash
gcc ... storage/sqlite/storage_sqlite.c storage/sqlite/sqlite3.c -lpthread -ldl -lm
```

The `-lpthread -ldl` flags are system libraries that SQLite needs on Linux.

## How to Compile and Run

**Linux / macOS:**
```bash
./build.sh
./bank
```

Or manually:
```bash
mkdir -p data
gcc -o bank main.c app/account.c app/account_service.c storage/sqlite/storage_sqlite.c storage/sqlite/sqlite3.c -lpthread -ldl -lm
./bank
```

**Windows (Developer Command Prompt):**
```cmd
build.bat
bank.exe
```

## Try It

1. Compile and run -- same menu, same behavior as Rev 3 and Rev 4.
2. Create, update, list, and delete accounts -- everything works the same.
3. Install a SQLite viewer (like "DB Browser for SQLite" -- free) and open
   `data/accounts.db`. You will see your data in a proper database table.
4. Compare all three data files side by side:
   - `rev3/data/accounts.dat` -- unreadable binary
   - `rev4/data/accounts.json` -- human-readable text
   - `rev5/data/accounts.db` -- structured database

## Common Errors

| Error | Cause | Fix |
|-------|-------|-----|
| `undefined reference to 'sqlite3_open'` | Forgot `sqlite3.c` in compile command | Add `storage/sqlite/sqlite3.c` |
| `undefined reference to 'pthread_create'` | Missing `-lpthread` flag | Add `-lpthread -ldl` to gcc command |
| `fatal error: sqlite3.h: No such file` | SQLite files not in `storage/sqlite/` | Download the amalgamation from sqlite.org |

## The Big Picture

Look at what happened across all five revisions:

| Rev | What Changed | What Stayed the Same |
|-----|-------------|---------------------|
| 1 | Starting point -- textbook example | -- |
| 2 | Cleaner code, better naming | Same single-file approach |
| 3 | Split into multiple files | Same binary storage, same features |
| 4 | JSON storage backend | `main.c`, `account_service.c` unchanged |
| 5 | SQLite storage backend | `main.c`, `account_service.c` still unchanged |

The lesson: **good abstractions let you change one part of a system without breaking
the rest.** The `storage.h` interface made it possible to swap from binary files to
JSON to SQLite -- three completely different technologies -- while the rest of the
program never noticed.
