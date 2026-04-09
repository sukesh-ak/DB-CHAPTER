# Rev 4 -- JSON Storage with cJSON

## Where We Are

Rev 3 split our program into multiple files and introduced the storage abstraction.
Now we put that abstraction to work.

Rev 4 replaces the binary file backend with **JSON** -- a human-readable text format.
The key point: we only add **one new implementation file** (`storage_cjson.c`) and
swap it into the compile command. `main.c` and `account_service.c` are untouched.

## What Changed from Rev 3

| Area | Rev 3 | Rev 4 |
|------|-------|-------|
| Data format | Binary (`accounts.dat`) | JSON (`accounts.json`) |
| Storage code | `storage/storage_file.c` | `storage/cjson/storage_cjson.c` |
| External library | None | cJSON (`cJSON.h` + `cJSON.c`) |
| Menu, service layer | Unchanged | **Identical** -- not a single line changed |

This is the payoff of the storage abstraction: swap the storage layer, keep everything
else.

## Project Structure

```
rev4/
  main.c                          <-- unchanged from Rev 3
  app/
    account.h                     <-- unchanged
    account.c                     <-- unchanged
    account_service.h             <-- unchanged
    account_service.c             <-- unchanged
  storage/
    storage.h                     <-- unchanged (same 6 function declarations)
    cjson/
      storage_cjson.c             <-- NEW: implements storage.h using JSON
      cJSON.h                     <-- third-party library (copy into project)
      cJSON.c                     <-- third-party library
  data/
    accounts.json                 <-- runtime data file (human-readable!)
  build.sh                        <-- updated compile command
  build.bat                       <-- updated compile command
```

## What You Will Learn

1. **JSON format** -- a simple text-based way to represent structured data.
   Your accounts look like this on disk:
   ```json
   {
       "accounts": [
           {
               "acctNum": 1,
               "firstName": "Alice",
               "lastName": "Smith",
               "balance": 500.00
           }
       ]
   }
   ```
2. **Using a third-party C library** -- cJSON is just two files (`cJSON.h` and
   `cJSON.c`) that you copy into your project. No package manager needed.
3. **cJSON API basics** -- creating JSON objects, adding fields, parsing JSON text,
   reading values, and the critical rule: every `cJSON_Create*` needs a matching
   `cJSON_Delete`, every `cJSON_Print` needs a matching `free`.
4. **Dynamic memory** -- JSON storage uses `malloc`/`free` because JSON files can
   be any size (unlike the fixed-size binary file from Rev 3).
5. **The storage swap in practice** -- the compile command is the only thing that
   changes. Compare `build.sh` in Rev 3 vs Rev 4.

## How the Swap Works

Rev 3 compile command:
```bash
gcc -o bank main.c app/account.c app/account_service.c storage/storage_file.c
```

Rev 4 compile command:
```bash
gcc -o bank main.c app/account.c app/account_service.c storage/cjson/storage_cjson.c storage/cjson/cJSON.c -lm
```

Same program. Different storage. Just swap which `.c` files you compile.

## How to Compile and Run

**Linux / macOS:**
```bash
./build.sh
./bank
```

Or manually:
```bash
mkdir -p data
gcc -o bank main.c app/account.c app/account_service.c storage/cjson/storage_cjson.c storage/cjson/cJSON.c -lm
./bank
```

**Windows (Developer Command Prompt):**
```cmd
build.bat
bank.exe
```

## Try It

1. Compile and run -- the menu and behavior are identical to Rev 3.
2. Create a couple of accounts.
3. Open `data/accounts.json` in any text editor -- you can **read your data**.
   Compare this to Rev 3's `accounts.dat`, which was unreadable binary.
4. Update an account, then check the JSON file again -- watch it change.
5. Delete an account and verify it disappears from the JSON file.
6. Try **manually editing** `accounts.json` in a text editor (change a balance),
   then list accounts in the program -- it picks up your changes.

## Common Errors

| Error | Cause | Fix |
|-------|-------|-----|
| `undefined reference to 'cJSON_Parse'` | Forgot `cJSON.c` in compile command | Add `storage/cjson/cJSON.c` |
| `cannot open data/accounts.json` | `data/` directory missing | Run `mkdir data` |
| `segmentation fault` on read | JSON file is corrupt or empty | Delete `data/accounts.json` and start fresh |

## What Is Next

JSON is great for readability, but it is not ideal for large datasets -- you have to
read and rewrite the entire file on every operation. In Rev 5, we switch to SQLite,
a real database engine, by swapping in one more file.
