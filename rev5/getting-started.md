# Getting Started with SQLite in C

SQLite is a database engine contained in a single C file. There is no server to
install -- you just compile `sqlite3.c` alongside your code and your program gets
a full SQL database stored in one file on disk.

This guide walks you through the SQLite C API functions used in Rev 5, with short
standalone examples you can try.

---

## 1. Including SQLite

```c
#include "sqlite3.h"
```

Make sure `sqlite3.h` is in a path your compiler can find, and compile `sqlite3.c`
alongside your code:

```bash
gcc -o myprogram myprogram.c sqlite3.c -lpthread -ldl -lm
```

- `-lpthread -ldl` are system libraries SQLite needs on Linux
- `-lm` links the math library

---

## 2. Core Concept: The Workflow

Every SQLite operation follows the same pattern:

```
Open database  -->  Write SQL  -->  Execute it  -->  Read results  -->  Clean up
```

For simple SQL (no parameters, no results):
```
sqlite3_open  -->  sqlite3_exec  -->  sqlite3_close
```

For SQL with parameters or results (most operations):
```
sqlite3_open  -->  sqlite3_prepare_v2  -->  sqlite3_bind_*  -->  sqlite3_step  -->  sqlite3_column_*  -->  sqlite3_finalize
```

---

## 3. Opening and Closing a Database

### Open

```c
sqlite3 *db = NULL;

int rc = sqlite3_open("data/accounts.db", &db);
if (rc != SQLITE_OK) {
    printf("Cannot open database: %s\n", sqlite3_errmsg(db));
    return -1;
}
```

- `sqlite3_open` creates the file if it does not exist
- Always check the return code against `SQLITE_OK`
- `sqlite3_errmsg(db)` gives a human-readable error message

### Close

```c
sqlite3_close(db);
```

Call this when your program is done with the database.

---

## 4. Creating a Table with `sqlite3_exec`

`sqlite3_exec` is the simplest way to run SQL that has no parameters and returns
no data. It is perfect for `CREATE TABLE`:

```c
const char *sql =
    "CREATE TABLE IF NOT EXISTS accounts ("
    "  acctNum   INTEGER PRIMARY KEY,"
    "  firstName TEXT,"
    "  lastName  TEXT,"
    "  balance   REAL"
    ");";

char *err = NULL;
int rc = sqlite3_exec(db, sql, NULL, NULL, &err);
if (rc != SQLITE_OK) {
    printf("SQL error: %s\n", err);
    sqlite3_free(err);    // must free the error string
}
```

- `IF NOT EXISTS` means it won't fail if the table is already there
- `INTEGER PRIMARY KEY` means `acctNum` is unique and indexed automatically
- `TEXT` is for strings, `REAL` is for floating-point numbers
- If there's an error, SQLite allocates a message -- free it with `sqlite3_free`

---

## 5. Prepared Statements (The Safe Way)

For any SQL that includes user data, use **prepared statements**. This means:
1. Write the SQL with `?` placeholders
2. Compile it with `sqlite3_prepare_v2`
3. Bind actual values to the `?` placeholders
4. Execute with `sqlite3_step`
5. Clean up with `sqlite3_finalize`

**Why not just paste values into the SQL string?** Two reasons:
- **SQL injection** -- a user could type `'; DROP TABLE accounts; --` as their name
- **Special characters** -- names with quotes or apostrophes would break the SQL

### The four-step pattern

```c
// Step 1: SQL with ? placeholders (numbered from 1)
const char *sql = "INSERT INTO accounts (acctNum, firstName, lastName, balance) "
                  "VALUES (?, ?, ?, ?);";

// Step 2: Compile the SQL
sqlite3_stmt *stmt;
sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

// Step 3: Bind values to each ?
sqlite3_bind_int(stmt, 1, 101);                          // ? #1 = acctNum
sqlite3_bind_text(stmt, 2, "Alice", -1, SQLITE_STATIC);  // ? #2 = firstName
sqlite3_bind_text(stmt, 3, "Smith", -1, SQLITE_STATIC);  // ? #3 = lastName
sqlite3_bind_double(stmt, 4, 500.00);                     // ? #4 = balance

// Step 4: Execute
int rc = sqlite3_step(stmt);
if (rc == SQLITE_DONE) {
    printf("Insert successful!\n");
}

// Step 5: Clean up
sqlite3_finalize(stmt);
```

---

## 6. Bind Functions

Each `?` placeholder gets a value through a bind function. The `?` positions start
at **1**, not 0.

| Function | Use for | Example |
|---|---|---|
| `sqlite3_bind_int(stmt, pos, value)` | Integers | `sqlite3_bind_int(stmt, 1, 101)` |
| `sqlite3_bind_double(stmt, pos, value)` | Decimals | `sqlite3_bind_double(stmt, 4, 500.00)` |
| `sqlite3_bind_text(stmt, pos, str, len, flag)` | Strings | `sqlite3_bind_text(stmt, 2, "Alice", -1, SQLITE_STATIC)` |

For `sqlite3_bind_text`:
- Pass `-1` for `len` to let SQLite measure the string length automatically
- Pass `SQLITE_STATIC` for `flag` when the string won't change before `sqlite3_step` runs
  (this is the common case)

---

## 7. INSERT -- Adding a Row

```c
const char *sql =
    "INSERT INTO accounts (acctNum, firstName, lastName, balance) "
    "VALUES (?, ?, ?, ?);";

sqlite3_stmt *stmt;
sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

sqlite3_bind_int(stmt, 1, acc->acctNum);
sqlite3_bind_text(stmt, 2, acc->firstName, -1, SQLITE_STATIC);
sqlite3_bind_text(stmt, 3, acc->lastName, -1, SQLITE_STATIC);
sqlite3_bind_double(stmt, 4, acc->balance);

int rc = sqlite3_step(stmt);
sqlite3_finalize(stmt);
// rc == SQLITE_DONE means success
```

---

## 8. SELECT -- Reading Rows

### Select one row

```c
const char *sql =
    "SELECT acctNum, firstName, lastName, balance "
    "FROM accounts WHERE acctNum=?;";

sqlite3_stmt *stmt;
sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
sqlite3_bind_int(stmt, 1, 101);  // looking for account 101

int rc = sqlite3_step(stmt);
if (rc == SQLITE_ROW) {
    // A row was found -- read column values (columns start at 0)
    int acctNum       = sqlite3_column_int(stmt, 0);
    const char *first = (const char *)sqlite3_column_text(stmt, 1);
    const char *last  = (const char *)sqlite3_column_text(stmt, 2);
    double balance    = sqlite3_column_double(stmt, 3);

    printf("Found: %d %s %s %.2f\n", acctNum, first, last, balance);
} else {
    printf("Account not found.\n");
}

sqlite3_finalize(stmt);
```

- `sqlite3_step` returns `SQLITE_ROW` if there is a result row
- Column positions start at **0** (unlike bind positions which start at 1)
- `sqlite3_column_text` returns `const unsigned char*`, so cast it to `const char*`

### Select multiple rows (loop)

```c
const char *sql =
    "SELECT acctNum, firstName, lastName, balance "
    "FROM accounts ORDER BY acctNum;";

sqlite3_stmt *stmt;
sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

while (sqlite3_step(stmt) == SQLITE_ROW) {
    int acctNum       = sqlite3_column_int(stmt, 0);
    const char *first = (const char *)sqlite3_column_text(stmt, 1);
    const char *last  = (const char *)sqlite3_column_text(stmt, 2);
    double balance    = sqlite3_column_double(stmt, 3);

    printf("%d: %s %s, Balance: %.2f\n", acctNum, first, last, balance);
}

sqlite3_finalize(stmt);
```

Each call to `sqlite3_step` advances to the next row. When there are no more rows,
it returns `SQLITE_DONE` and the loop exits.

---

## 9. UPDATE -- Changing a Row

```c
const char *sql =
    "UPDATE accounts SET firstName=?, lastName=?, balance=? "
    "WHERE acctNum=?;";

sqlite3_stmt *stmt;
sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

sqlite3_bind_text(stmt, 1, "Bob", -1, SQLITE_STATIC);
sqlite3_bind_text(stmt, 2, "Jones", -1, SQLITE_STATIC);
sqlite3_bind_double(stmt, 3, 750.00);
sqlite3_bind_int(stmt, 4, 101);  // WHERE acctNum = 101

int rc = sqlite3_step(stmt);
sqlite3_finalize(stmt);

if (rc == SQLITE_DONE && sqlite3_changes(db) > 0) {
    printf("Updated!\n");
} else {
    printf("Account not found.\n");
}
```

`sqlite3_changes(db)` returns how many rows were affected. If it is 0, the
`WHERE` clause did not match anything.

---

## 10. DELETE -- Removing a Row

```c
const char *sql = "DELETE FROM accounts WHERE acctNum=?;";

sqlite3_stmt *stmt;
sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
sqlite3_bind_int(stmt, 1, 101);

int rc = sqlite3_step(stmt);
sqlite3_finalize(stmt);

if (rc == SQLITE_DONE && sqlite3_changes(db) > 0) {
    printf("Deleted!\n");
} else {
    printf("Account not found.\n");
}
```

---

## 11. Return Codes

| Constant | Value | Meaning |
|---|---|---|
| `SQLITE_OK` | 0 | Operation succeeded (for open, exec, prepare) |
| `SQLITE_ROW` | 100 | `sqlite3_step` found a row of results |
| `SQLITE_DONE` | 101 | `sqlite3_step` finished executing (no more rows) |

---

## 12. Column Functions

When `sqlite3_step` returns `SQLITE_ROW`, read values with these functions.
Column positions start at **0**.

| Function | Returns | Use for |
|---|---|---|
| `sqlite3_column_int(stmt, col)` | `int` | Integer columns |
| `sqlite3_column_double(stmt, col)` | `double` | Real/decimal columns |
| `sqlite3_column_text(stmt, col)` | `const unsigned char*` | Text columns (cast to `const char*`) |

---

## 13. Complete Example: Save and Load an Account

```c
#include <stdio.h>
#include <string.h>
#include "sqlite3.h"

int main(void) {
    sqlite3 *db;

    // Open (creates file if needed)
    if (sqlite3_open("test.db", &db) != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    // Create table
    char *err = NULL;
    sqlite3_exec(db,
        "CREATE TABLE IF NOT EXISTS accounts ("
        "  acctNum INTEGER PRIMARY KEY,"
        "  firstName TEXT,"
        "  lastName TEXT,"
        "  balance REAL);",
        NULL, NULL, &err);
    if (err) { printf("Error: %s\n", err); sqlite3_free(err); return 1; }

    // Insert an account
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db,
        "INSERT INTO accounts VALUES (?, ?, ?, ?);",
        -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, 101);
    sqlite3_bind_text(stmt, 2, "Alice", -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, "Smith", -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 4, 500.00);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    printf("Inserted account 101.\n");

    // Read it back
    sqlite3_prepare_v2(db,
        "SELECT acctNum, firstName, lastName, balance "
        "FROM accounts WHERE acctNum=?;",
        -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, 101);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        printf("Found: %d %s %s %.2f\n",
            sqlite3_column_int(stmt, 0),
            (const char *)sqlite3_column_text(stmt, 1),
            (const char *)sqlite3_column_text(stmt, 2),
            sqlite3_column_double(stmt, 3));
    }
    sqlite3_finalize(stmt);

    // Clean up
    sqlite3_close(db);
    return 0;
}
```

Compile and run:
```bash
gcc -o test_sqlite test_sqlite.c sqlite3.c -lpthread -ldl -lm
./test_sqlite
```

Output:
```
Inserted account 101.
Found: 101 Alice Smith 500.00
```

---

## 14. Quick Reference

| Function | What it does |
|---|---|
| `sqlite3_open(path, &db)` | Open (or create) a database file |
| `sqlite3_close(db)` | Close the database |
| `sqlite3_exec(db, sql, NULL, NULL, &err)` | Run simple SQL (no params, no results) |
| `sqlite3_prepare_v2(db, sql, -1, &stmt, NULL)` | Compile SQL into a prepared statement |
| `sqlite3_bind_int(stmt, pos, value)` | Bind an integer to `?` at position `pos` |
| `sqlite3_bind_double(stmt, pos, value)` | Bind a double to `?` at position `pos` |
| `sqlite3_bind_text(stmt, pos, str, -1, SQLITE_STATIC)` | Bind a string to `?` at position `pos` |
| `sqlite3_step(stmt)` | Execute the statement (returns ROW or DONE) |
| `sqlite3_column_int(stmt, col)` | Read integer from column `col` of current row |
| `sqlite3_column_double(stmt, col)` | Read double from column `col` of current row |
| `sqlite3_column_text(stmt, col)` | Read string from column `col` of current row |
| `sqlite3_finalize(stmt)` | Free the prepared statement |
| `sqlite3_changes(db)` | Number of rows affected by last INSERT/UPDATE/DELETE |
| `sqlite3_errmsg(db)` | Get human-readable error message |
| `sqlite3_free(ptr)` | Free memory allocated by SQLite (e.g. error strings) |

---

## 15. Common Mistakes

**Forgetting to finalize:**
```c
// BAD -- memory leak
sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
sqlite3_step(stmt);
// forgot sqlite3_finalize(stmt)!

// GOOD
sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
sqlite3_step(stmt);
sqlite3_finalize(stmt);
```

**Bind positions start at 1, column positions start at 0:**
```c
// Binding: positions 1, 2, 3, 4
sqlite3_bind_int(stmt, 1, acctNum);       // first ?  = position 1
sqlite3_bind_text(stmt, 2, name, -1, SQLITE_STATIC);  // second ? = position 2

// Reading columns: positions 0, 1, 2, 3
int acctNum    = sqlite3_column_int(stmt, 0);    // first column  = position 0
const char *fn = (const char *)sqlite3_column_text(stmt, 1);  // second column = position 1
```

**Building SQL by pasting strings (SQL injection risk):**
```c
// BAD -- vulnerable to SQL injection
char sql[256];
sprintf(sql, "INSERT INTO accounts VALUES (%d, '%s', '%s', %f);",
        acctNum, firstName, lastName, balance);
sqlite3_exec(db, sql, NULL, NULL, NULL);

// GOOD -- use prepared statements with ? placeholders
const char *sql = "INSERT INTO accounts VALUES (?, ?, ?, ?);";
sqlite3_stmt *stmt;
sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
sqlite3_bind_int(stmt, 1, acctNum);
sqlite3_bind_text(stmt, 2, firstName, -1, SQLITE_STATIC);
// ... etc
```

**Not checking if a row was found:**
```c
// BAD -- will read garbage if no row matches
sqlite3_step(stmt);
int num = sqlite3_column_int(stmt, 0);

// GOOD
if (sqlite3_step(stmt) == SQLITE_ROW) {
    int num = sqlite3_column_int(stmt, 0);
} else {
    printf("Not found.\n");
}
```

---

## Next Step

Open [storage/sqlite/storage_sqlite.c](storage/sqlite/storage_sqlite.c) and read
through it. You now know every SQLite function it uses. Notice how each storage
operation (init, add, update, delete, get, get_all) follows the same
prepare-bind-step-finalize pattern described above.
