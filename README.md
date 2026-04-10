# Storage Chapter -- Learning Data Storage in C

A hands-on teaching project that builds a **bank account system** five times, each
revision introducing one new concept. The program's features stay the same throughout --
create, update, delete, and list accounts. What changes is **how the code is organized**
and **how data is stored**.

By the end, students see how a well-designed abstraction lets you swap from binary files
to JSON to SQLite without rewriting the rest of the program.

---

## The Five Revisions

| Rev | Focus | Storage | Files |
|-----|-------|---------|-------|
| [Rev 1](rev1/) | Starting point -- textbook example | Binary file (`credit.dat`) | 1 file |
| [Rev 2](rev2/) | Clean code, better naming, input validation | Binary file (`accounts.dat`) | 1 file |
| [Rev 3](rev3/) | Multi-file project, storage abstraction | Binary file (`accounts.dat`) | 6 files |
| [Rev 4](rev4/) | Swap storage to JSON (using cJSON library) | JSON text (`accounts.json`) | 7 files + cJSON |
| [Rev 5](rev5/) | Swap storage to SQLite database | SQLite database (`accounts.db`) | 7 files + SQLite |

Each revision has its own `README.md` explaining what changed and why.

---

## What Students Learn at Each Step

### Rev 1 -- The Textbook Example

The starting point. A single-file C program from a textbook that uses `fread`/`fwrite`
with a random-access binary file. It works, but the code is hard to read and extend.

### Rev 2 -- Writing Better C

Same program, rewritten cleanly. Introduces `typedef`, input validation with `fgets`,
meaningful function names, and `perror()` for error messages. Still one file -- the focus
is on code quality, not architecture.

### Rev 3 -- Splitting into Multiple Files

The program is split across `main.c`, `account_service.c`, `account.c`, and
`storage_file.c`. Headers declare the interfaces. The key introduction is
**`storage.h`** -- an abstraction layer that defines *what* storage can do without
saying *how*:

```c
int storage_init(void);
int storage_add(Account *acc);
int storage_update(Account *acc);
int storage_delete(int acctNum);
int storage_get(int acctNum, Account *out);
int storage_get_all(Account *list, int max);
```

The service layer calls these functions. It never touches files directly.

### Rev 4 -- JSON Storage with cJSON

The binary backend is replaced with JSON. Only **one new file** is added
(`storage_cjson.c`) and the compile command is updated. `main.c` and
`account_service.c` are completely untouched -- the storage abstraction pays off.

Students learn to use a third-party C library (cJSON) and see human-readable
data for the first time. See [rev4/getting-started.md](rev4/getting-started.md)
for a cJSON tutorial.

### Rev 5 -- SQLite Database Storage

JSON is replaced with SQLite. Again, only **one new file** (`storage_sqlite.c`)
and a compile command change. `main.c` and `account_service.c` remain identical
to Rev 3.

Students learn SQL basics, prepared statements, and parameter binding. See
[rev5/getting-started.md](rev5/getting-started.md) for a SQLite tutorial.

---

## The Storage Abstraction in Action

This is the central lesson. The same six functions in `storage.h` are implemented
three different ways:

| Function | Rev 3 (Binary) | Rev 4 (JSON) | Rev 5 (SQLite) |
|----------|----------------|--------------|-----------------|
| `storage_init` | Create file with blank records | No-op (file created on write) | `sqlite3_open` + `CREATE TABLE` |
| `storage_add` | `fseek` + `fwrite` | `cJSON_AddItemToArray` | `INSERT INTO` |
| `storage_get` | `fseek` + `fread` | Loop through `cJSON` array | `SELECT WHERE` |
| `storage_update` | `fseek` + `fwrite` | `cJSON_ReplaceItemInArray` | `UPDATE SET WHERE` |
| `storage_delete` | Write blank record | `cJSON_DeleteItemFromArray` | `DELETE WHERE` |
| `storage_get_all` | Read all + skip blanks | Iterate `cJSON` array | `SELECT` loop |

The compile command is the **only** thing that changes between revisions:

```bash
# Rev 3 -- binary
gcc -o bank main.c app/account.c app/account_service.c storage/storage_file.c

# Rev 4 -- JSON
gcc -o bank main.c app/account.c app/account_service.c storage/cjson/storage_cjson.c storage/cjson/cJSON.c -lm

# Rev 5 -- SQLite
gcc -o bank main.c app/account.c app/account_service.c storage/sqlite/storage_sqlite.c storage/sqlite/sqlite3.c -lpthread -ldl -lm
```

---

## Final -- All Three Backends in One Project

The [Final/](Final/) folder is the finished product. It contains the same `main.c`,
`account_service.c`, and `storage.h` from Rev 3-5, but with **all three storage
backends side by side**. To select a backend, you pass the corresponding
implementation file to gcc:

```bash
gcc -o bank_file   ... storage/file/storage_file.c
gcc -o bank_json   ... storage/cjson/storage_cjson.c storage/cjson/cJSON.c -lm
gcc -o bank_sqlite ... storage/sqlite/storage_sqlite.c storage/sqlite/sqlite3.c -lpthread -ldl -lm
```

Build all three with one script:

```bash
cd Final
./build.sh
```

| Executable | Storage implementation | Data file |
|---|---|---|
| `bank_file` | `storage_file.c` | `data/accounts.dat` |
| `bank_json` | `storage_cjson.c` | `data/accounts.json` |
| `bank_sqlite` | `storage_sqlite.c` | `data/accounts.db` |

All three behave identically. The difference is what you see when you open the
`data/` folder -- unreadable binary, human-readable JSON, or a structured database.

See [Final/README.md](Final/README.md) for full details.

---

## Project Structure

```
storage-chapter/
  rev1/
    trans.c                          <-- original textbook code
  rev2/
    trans.c                          <-- cleaned up single file
  rev3/
    main.c                           <-- menu (unchanged in Rev 4, 5)
    app/
      account.h                      <-- Account struct (unchanged in Rev 4, 5)
      account.c                      <-- print helper (unchanged in Rev 4, 5)
      account_service.h              <-- service interface (unchanged in Rev 4, 5)
      account_service.c              <-- business logic (unchanged in Rev 4, 5)
    storage/
      storage.h                      <-- storage interface (unchanged in Rev 4, 5)
      storage_file.c                 <-- binary file implementation
  rev4/
    ...same app/ files...
    storage/
      cjson/
        storage_cjson.c              <-- JSON implementation
        cJSON.h, cJSON.c             <-- third-party library
    getting-started.md               <-- cJSON tutorial for students
  rev5/
    ...same app/ files...
    storage/
      sqlite/
        storage_sqlite.c             <-- SQLite implementation
        sqlite3.h, sqlite3.c         <-- third-party library
    getting-started.md               <-- SQLite tutorial for students
  Final/
    main.c                           <-- same shared entry point
    app/                             <-- same shared application code
    storage/
      storage.h                      <-- same shared interface
      file/
        storage_file.c               <-- binary file backend
      cjson/
        storage_cjson.c, cJSON.*     <-- JSON backend
      sqlite/
        storage_sqlite.c, sqlite3.*  <-- SQLite backend
    build.sh / build.bat             <-- builds all three
```

---

## How to Use This Repository

**For self-study:** Start at Rev 1, read the README in each revision, and work through
them in order. The progression is designed so each step introduces exactly one new idea.

**For teaching:** Each revision can be a separate class session or assignment:
1. Rev 1-2: C fundamentals, code quality
2. Rev 3: Multi-file projects, header files, abstraction
3. Rev 4: Third-party libraries, JSON, cJSON API
4. Rev 5: Databases, SQL, SQLite C API

**To build any revision:**
```bash
cd rev3    # or rev4, rev5
./build.sh
./bank
```

**To build the Final version (all three backends):**
```bash
cd Final
./build.sh
./bank_file      # or bank_json, bank_sqlite
```

---

## Prerequisites

- A C compiler (`gcc` on Linux/macOS, MSVC on Windows)
- Basic C knowledge (structs, pointers, file I/O)
- No external installations needed -- cJSON and SQLite are included as source files
