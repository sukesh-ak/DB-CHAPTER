# Rev 5 -- Assignment: Add SQLite Storage

## Starting Point

You have a multi-file program with a storage abstraction. In Rev 3, you implemented
it with binary files. In Rev 4, you swapped in JSON. Now you will do it one more time
with **SQLite** -- a real database engine.

Once again, you will not change `main.c`, `account_service.c`, `account.h`, or
`account.c`. You only write one new file.

## What You Must Produce

Copy your Rev 4 project (or Rev 3 -- the shared files are identical), then add the
SQLite storage files:

```
rev5/
  main.c                                  <-- UNCHANGED
  app/
    account.h                             <-- UNCHANGED
    account.c                             <-- UNCHANGED
    account_service.h                     <-- UNCHANGED
    account_service.c                     <-- UNCHANGED
  storage/
    storage.h                             <-- UNCHANGED
    sqlite/
      storage_sqlite.c                    <-- NEW: you write this
      sqlite3.h                           <-- copy from SQLite download
      sqlite3.c                           <-- copy from SQLite download
  data/
    accounts.db                           <-- created at runtime
```

**You only write one new file:** `storage/sqlite/storage_sqlite.c`

## Getting SQLite

1. Go to https://www.sqlite.org/download.html
2. Under "Source Code", download the **amalgamation** zip file
3. Extract `sqlite3.h` and `sqlite3.c`
4. Place both files in `storage/sqlite/`

Just like cJSON -- two files, no installation needed.

## SQL You Will Need

SQLite uses SQL (Structured Query Language) to work with data. You need five
statements:

**Create the table** (run once at startup):
```sql
CREATE TABLE IF NOT EXISTS accounts (
    acctNum   INTEGER PRIMARY KEY,
    firstName TEXT,
    lastName  TEXT,
    balance   REAL
);
```

**Insert a record:**
```sql
INSERT INTO accounts (acctNum, firstName, lastName, balance)
VALUES (?, ?, ?, ?);
```

**Update a record:**
```sql
UPDATE accounts SET firstName=?, lastName=?, balance=?
WHERE acctNum=?;
```

**Delete a record:**
```sql
DELETE FROM accounts WHERE acctNum=?;
```

**Find one record:**
```sql
SELECT acctNum, firstName, lastName, balance
FROM accounts WHERE acctNum=?;
```

**Get all records:**
```sql
SELECT acctNum, firstName, lastName, balance
FROM accounts ORDER BY acctNum;
```

The `?` placeholders are filled in using "parameter binding" (explained below).
Never paste user values directly into SQL strings.

## SQLite C API Quick Reference

**Opening and closing:**
```
sqlite3_open("path/to/file.db", &db)   -- open (or create) a database file
sqlite3_close(db)                       -- close the database
sqlite3_errmsg(db)                      -- get the last error message as a string
```

**Running simple SQL (no parameters, no results):**
```
sqlite3_exec(db, sql, NULL, NULL, &err) -- execute a SQL statement
sqlite3_free(err)                       -- free the error message if non-NULL
```

Use `sqlite3_exec` for `CREATE TABLE`. For everything else, use prepared
statements (below).

**Prepared statements (for SQL with parameters):**
```
sqlite3_prepare_v2(db, sql, -1, &stmt, NULL)  -- compile SQL into a statement
sqlite3_bind_int(stmt, 1, value)              -- bind an int to parameter 1
sqlite3_bind_double(stmt, 3, value)           -- bind a double to parameter 3
sqlite3_bind_text(stmt, 2, str, -1, SQLITE_STATIC) -- bind a string to parameter 2
sqlite3_step(stmt)                            -- execute the statement
sqlite3_finalize(stmt)                        -- free the statement
```

Parameter numbers start at **1**, not 0.

`sqlite3_step` returns:
- `SQLITE_DONE` -- the statement completed (for INSERT, UPDATE, DELETE)
- `SQLITE_ROW` -- a result row is available (for SELECT)

**Reading results from SELECT:**
```
sqlite3_column_int(stmt, 0)       -- read an integer from column 0
sqlite3_column_double(stmt, 3)    -- read a double from column 3
sqlite3_column_text(stmt, 1)      -- read a string from column 1
```

Column numbers start at **0** (unlike parameter numbers which start at 1).

**Checking if a row was actually affected:**
```
sqlite3_changes(db)               -- returns number of rows changed by last statement
```

Use this after UPDATE and DELETE to check whether the account existed.

## Step-by-Step Guidance

### Step 1: Set up the database connection

Declare a file-level `static sqlite3 *db = NULL;` variable. All functions will
use this shared connection.

### Step 2: Implement `storage_init`

1. Call `sqlite3_open("data/accounts.db", &db)`
2. Check the return code. If it is not `SQLITE_OK`, print the error and return -1.
3. Run the `CREATE TABLE IF NOT EXISTS` statement using `sqlite3_exec`.
4. Return 0 on success.

### Step 3: Implement `storage_get`

This is the simplest SELECT. Implement it early because `storage_add` needs it
to check for duplicates.

1. Prepare the SELECT statement with a WHERE clause
2. Bind the account number to parameter 1
3. Call `sqlite3_step`
4. If it returns `SQLITE_ROW`, read the columns into the output `Account` struct
   using `sqlite3_column_*` functions. Use `strncpy` for strings (cast
   `sqlite3_column_text` to `const char *`). Return 0.
5. If no row, return -1.
6. Always call `sqlite3_finalize` before returning.

### Step 4: Implement `storage_add`

1. First call `storage_get` to check if the account already exists. If it does,
   return -1.
2. Prepare the INSERT statement
3. Bind all four parameters (acctNum, firstName, lastName, balance)
4. Step and finalize
5. Return 0 if `sqlite3_step` returned `SQLITE_DONE`

### Step 5: Implement `storage_update`

1. Prepare the UPDATE statement
2. Bind parameters (note: the WHERE parameter `acctNum` is the **last** `?` in
   the SQL, so it gets the highest parameter number)
3. Step and finalize
4. Check `sqlite3_changes(db)` -- if it is 0, the account did not exist, return -1

### Step 6: Implement `storage_delete`

1. Prepare the DELETE statement
2. Bind the account number
3. Step and finalize
4. Check `sqlite3_changes(db)` -- if it is 0, return -1

### Step 7: Implement `storage_get_all`

1. Prepare the SELECT (no WHERE clause, with ORDER BY)
2. Loop: while `sqlite3_step` returns `SQLITE_ROW` and count < max, read each
   row into `list[count]` and increment count
3. Finalize and return count

## How to Compile

**Linux / macOS:**
```bash
mkdir -p data
gcc -o bank main.c app/account.c app/account_service.c storage/sqlite/storage_sqlite.c storage/sqlite/sqlite3.c -lpthread -ldl -lm
```

**Windows (Developer Command Prompt):**
```cmd
if not exist data mkdir data
cl /Fe:bank.exe main.c app\account.c app\account_service.c storage\sqlite\storage_sqlite.c storage\sqlite\sqlite3.c
```

The `-lpthread -ldl` flags are system libraries that SQLite needs on Linux.
Windows does not need them.

Notice: compared to Rev 4, you replaced the cJSON files with the SQLite files.
Everything else in the compile command is the same.

## How to Test

1. Compile and run. The menu and behavior must be identical to Rev 3 and Rev 4.
2. Create, update, list, and delete accounts.
3. Download "DB Browser for SQLite" (free) and open `data/accounts.db`. You
   should see your data in a proper database table.
4. Compare all three data formats:
   - Rev 3: `data/accounts.dat` -- binary, unreadable
   - Rev 4: `data/accounts.json` -- text, human-readable
   - Rev 5: `data/accounts.db` -- database, queryable

## Rules

1. **Do not modify** `main.c`, `account_service.c`, `account.h`, `account.c`,
   or `storage.h`. They must be identical to Rev 3 and Rev 4.
2. Your `storage_sqlite.c` must implement **exactly** the six functions declared
   in `storage.h`.
3. **Never paste user data directly into SQL strings.** Always use `?` parameters
   and `sqlite3_bind_*`. This prevents SQL injection.
4. Always call `sqlite3_finalize` on every prepared statement, even if an error
   occurred.

## Reflection

When you are done, look at the files across Rev 3, Rev 4, and Rev 5:

- `main.c` -- identical in all three
- `account_service.c` -- identical in all three
- `storage.h` -- identical in all three
- Only the storage implementation file changed

This is the value of a **storage abstraction**. You swapped from binary files to
JSON to SQLite -- three completely different technologies -- and the rest of the
program never noticed.
