# Rev 4 -- Assignment: Add JSON Storage

## Starting Point

You have a multi-file program from Rev 3 with a clean storage abstraction.
`storage.h` declares six functions; `storage_file.c` implements them using binary
file I/O. The rest of the code (`main.c`, `account_service.c`) only knows about
the interface, not the implementation.

Your job is to **write a new storage implementation** that saves data as
human-readable JSON instead of binary. You will not change `main.c`,
`account_service.c`, `account.h`, or `account.c` at all.

## What You Must Produce

Copy your Rev 3 project, then add the JSON storage files:

```
rev4/
  main.c                              <-- UNCHANGED from Rev 3
  app/
    account.h                         <-- UNCHANGED
    account.c                         <-- UNCHANGED
    account_service.h                 <-- UNCHANGED
    account_service.c                 <-- UNCHANGED
  storage/
    storage.h                         <-- UNCHANGED
    cjson/
      storage_cjson.c                 <-- NEW: you write this
      cJSON.h                         <-- copy from cJSON library
      cJSON.c                         <-- copy from cJSON library
  data/
    accounts.json                     <-- created at runtime
```

**You only write one new file:** `storage/cjson/storage_cjson.c`

## Getting the cJSON Library

cJSON is a lightweight JSON library for C. It is just two files.

1. Go to https://github.com/DaveGamble/cJSON
2. Download `cJSON.h` and `cJSON.c` (click the file, click "Raw", save)
3. Place both files in `storage/cjson/`

No installation. No package manager. Just two files copied into your project.

## The JSON File Format

Your `data/accounts.json` should look like this:

```json
{
    "accounts": [
        {
            "acctNum": 1,
            "firstName": "Bob",
            "lastName": "Jones",
            "balance": 1234.56
        },
        {
            "acctNum": 5,
            "firstName": "Alice",
            "lastName": "Smith",
            "balance": 500.00
        }
    ]
}
```

The root object has one key (`"accounts"`) whose value is an array of account
objects. Each account object has four fields matching the `Account` struct.

## cJSON Quick Reference

You will need these cJSON functions. Read the `cJSON.h` header for full details.

**Creating JSON:**
```
cJSON_CreateObject()                          -- create an empty {}
cJSON_CreateArray()                           -- create an empty []
cJSON_AddNumberToObject(obj, "key", number)   -- add a number field
cJSON_AddStringToObject(obj, "key", string)   -- add a string field
cJSON_AddItemToObject(obj, "key", child)       -- add a child object/array
cJSON_AddItemToArray(arr, item)               -- append an item to an array
```

**Reading JSON:**
```
cJSON_Parse(text)                             -- parse a JSON string into objects
cJSON_GetObjectItem(obj, "key")               -- get a field by name
cJSON_GetArraySize(arr)                       -- get number of items in array
cJSON_GetArrayItem(arr, index)                -- get item at index
item->valueint                                -- read an integer value
item->valuedouble                             -- read a double value
item->valuestring                             -- read a string value
```

**Modifying JSON:**
```
cJSON_ReplaceItemInArray(arr, index, newItem) -- replace an array element
cJSON_DeleteItemFromArray(arr, index)         -- remove an array element
```

**Output and cleanup:**
```
cJSON_Print(obj)                              -- convert to formatted string (caller must free)
cJSON_Delete(obj)                             -- free a cJSON tree
```

**Critical memory rule:**
- Every `cJSON_CreateObject` / `cJSON_CreateArray` / `cJSON_Parse` must have a
  matching `cJSON_Delete`
- Every `cJSON_Print` must have a matching `free`
- Forgetting these causes memory leaks

## Step-by-Step Guidance

### Step 1: Write helper functions

Before implementing the six storage functions, write these internal helpers in
`storage_cjson.c` (mark them `static` so they are private to this file):

1. **`read_file(path)`** -- read an entire file into a `malloc`'d string. Return
   `NULL` if the file does not exist. The caller must `free` the result.
   - Use `fseek(f, 0, SEEK_END)` + `ftell(f)` to find the file size
   - `malloc(size + 1)`, `fread`, null-terminate

2. **`write_file(path, text)`** -- write a string to a file, overwriting any
   existing content.

3. **`load_json()`** -- call `read_file`, then `cJSON_Parse`. If the file does
   not exist or is invalid, return a fresh `{"accounts": []}` object.

4. **`save_json(root)`** -- call `cJSON_Print`, then `write_file`, then `free`
   the printed string.

5. **`account_to_json(acc)`** -- convert an `Account` struct to a cJSON object
   with four fields.

6. **`json_to_account(obj, acc)`** -- fill an `Account` struct from a cJSON
   object. Use `strncpy` for strings (not `strcpy`) and null-terminate.

### Step 2: Implement the six storage functions

Each function follows the same pattern:
1. Call `load_json()` to get the root object
2. Get the `"accounts"` array from the root
3. Do the work (search, add, modify, or remove)
4. If data changed, call `save_json(root)` to write back
5. Call `cJSON_Delete(root)` to free memory
6. Return `0` for success, `-1` for failure

**`storage_init`** -- nothing to do (the file is created on first write). Return 0.

**`storage_add`** -- load, scan the array to check the account does not already
exist, append a new JSON object, save.

**`storage_update`** -- load, find the matching account by `acctNum`, replace it
with a new JSON object, save.

**`storage_delete`** -- load, find the matching account, remove it from the array
with `cJSON_DeleteItemFromArray`, save.

**`storage_get`** -- load, find the matching account, fill the output `Account`
struct using `json_to_account`, return 0 if found.

**`storage_get_all`** -- load, loop through the array, fill the output list using
`json_to_account`, return the count.

## How to Compile

**Linux / macOS:**
```bash
mkdir -p data
gcc -o bank main.c app/account.c app/account_service.c storage/cjson/storage_cjson.c storage/cjson/cJSON.c -lm
```

**Windows (Developer Command Prompt):**
```cmd
if not exist data mkdir data
cl /Fe:bank.exe main.c app\account.c app\account_service.c storage\cjson\storage_cjson.c storage\cjson\cJSON.c
```

Notice: compared to Rev 3, you replaced `storage/storage_file.c` with
`storage/cjson/storage_cjson.c storage/cjson/cJSON.c`. That is the only change.

## How to Test

1. Compile and run. The menu and behavior must be identical to Rev 3.
2. Create a couple of accounts.
3. Open `data/accounts.json` in a text editor. You should see your data in
   plain text. This was impossible with the binary file in Rev 3.
4. Update an account, check the JSON file again.
5. Delete an account, check the JSON file again.
6. List all accounts and verify the output matches the JSON file.
7. Try manually editing `accounts.json` in a text editor (change a balance or
   name), then run the program and list accounts. It should show your changes.

## Rules

1. **Do not modify** `main.c`, `account_service.c`, `account.h`, `account.c`,
   or `storage.h`. The whole point is that these files remain unchanged.
2. Your `storage_cjson.c` must implement **exactly** the six functions declared
   in `storage.h`, with the same signatures and return value conventions.
3. Always free cJSON memory. Run through each function and verify every
   `load_json()` call has a matching `cJSON_Delete`.
