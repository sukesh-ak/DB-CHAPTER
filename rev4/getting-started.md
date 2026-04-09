# Getting Started with cJSON

cJSON is a small C library for reading and writing JSON. It is just two files --
`cJSON.h` and `cJSON.c` -- that you copy into your project. No installer, no
package manager.

This guide walks you through the cJSON functions used in Rev 4, with short
standalone examples you can try.

---

## 1. Including cJSON

```c
#include "cJSON.h"
```

That is it. Make sure `cJSON.h` is in a path your compiler can find, and compile
`cJSON.c` alongside your code:

```bash
gcc -o myprogram myprogram.c cJSON.c -lm
```

The `-lm` links the math library, which cJSON uses internally.

---

## 2. Core Concept: The cJSON Tree

cJSON represents JSON as a **tree of `cJSON` nodes** in memory. Every JSON value --
object, array, string, number -- is a `cJSON*` pointer.

```
Root (object)
 +-- "accounts" (array)
      +-- [0] (object)
      |    +-- "acctNum"   (number)
      |    +-- "firstName" (string)
      |    +-- "lastName"  (string)
      |    +-- "balance"   (number)
      +-- [1] (object)
           +-- ...
```

You build this tree to **write** JSON, or you get this tree when you **parse** JSON.

---

## 3. Creating JSON (Struct to JSON)

### Create an empty object

```c
cJSON *obj = cJSON_CreateObject();
```

### Add fields to it

```c
cJSON_AddNumberToObject(obj, "acctNum",   101);
cJSON_AddStringToObject(obj, "firstName", "Alice");
cJSON_AddStringToObject(obj, "lastName",  "Smith");
cJSON_AddNumberToObject(obj, "balance",   500.00);
```

This builds:
```json
{
    "acctNum": 101,
    "firstName": "Alice",
    "lastName": "Smith",
    "balance": 500.0
}
```

### Convert the tree to a JSON string

```c
char *text = cJSON_Print(obj);   // pretty-printed string
printf("%s\n", text);
free(text);                      // you must free the string
cJSON_Delete(obj);               // you must delete the tree
```

`cJSON_Print` allocates memory for the string. **You** are responsible for calling
`free()` on it when you are done.

---

## 4. Parsing JSON (JSON String to Tree)

Given a JSON string (e.g. read from a file):

```c
const char *json_text = "{ \"acctNum\": 101, \"firstName\": \"Alice\" }";

cJSON *obj = cJSON_Parse(json_text);
if (obj == NULL) {
    printf("Parse error!\n");
    return;
}
```

`cJSON_Parse` returns `NULL` if the string is not valid JSON.

---

## 5. Reading Values from a Parsed Tree

### Get a field by name

```c
cJSON *field = cJSON_GetObjectItem(obj, "acctNum");
```

### Read the value

```c
int num       = field->valueint;       // for integers
double dbl    = field->valuedouble;    // for floating-point numbers
const char *s = field->valuestring;    // for strings
```

### Typical pattern (one line)

```c
int acctNum       = cJSON_GetObjectItem(obj, "acctNum")->valueint;
const char *first = cJSON_GetObjectItem(obj, "firstName")->valuestring;
double balance    = cJSON_GetObjectItem(obj, "balance")->valuedouble;
```

---

## 6. Working with Arrays

### Create an array and add items

```c
cJSON *root = cJSON_CreateObject();
cJSON *arr  = cJSON_CreateArray();
cJSON_AddItemToObject(root, "accounts", arr);

// Add an object to the array
cJSON *acc = cJSON_CreateObject();
cJSON_AddNumberToObject(acc, "acctNum", 101);
cJSON_AddStringToObject(acc, "firstName", "Alice");
cJSON_AddItemToArray(arr, acc);
```

### Get array size and iterate

```c
cJSON *arr = cJSON_GetObjectItem(root, "accounts");

int size = cJSON_GetArraySize(arr);
for (int i = 0; i < size; i++) {
    cJSON *item = cJSON_GetArrayItem(arr, i);
    int num = cJSON_GetObjectItem(item, "acctNum")->valueint;
    printf("Account: %d\n", num);
}
```

### Replace an item in an array

```c
cJSON *new_obj = cJSON_CreateObject();
cJSON_AddNumberToObject(new_obj, "acctNum", 101);
cJSON_AddStringToObject(new_obj, "firstName", "Bob");

cJSON_ReplaceItemInArray(arr, index, new_obj);  // replaces item at position
```

### Delete an item from an array

```c
cJSON_DeleteItemFromArray(arr, index);  // removes item at position
```

---

## 7. Memory Rules

cJSON uses `malloc` internally. You must clean up after yourself.

| You called | You must call |
|---|---|
| `cJSON_CreateObject()` or `cJSON_CreateArray()` | `cJSON_Delete(root)` on the **root** node (deletes the whole tree) |
| `cJSON_Print(obj)` | `free(text)` on the returned string |
| `cJSON_Parse(text)` | `cJSON_Delete(root)` when done reading |

**Important:** Only call `cJSON_Delete` on the **root** of the tree. Child nodes
are freed automatically when you delete their parent.

---

## 8. Complete Example: Save and Load an Account

This is a simplified version of what `storage_cjson.c` does in Rev 4.

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

// Save one account to a JSON file
void save_account(const char *filename, int acctNum,
                  const char *first, const char *last,
                  double balance) {
    // Build the JSON tree
    cJSON *root = cJSON_CreateObject();
    cJSON *arr  = cJSON_CreateArray();
    cJSON_AddItemToObject(root, "accounts", arr);

    cJSON *acc = cJSON_CreateObject();
    cJSON_AddNumberToObject(acc, "acctNum",   acctNum);
    cJSON_AddStringToObject(acc, "firstName", first);
    cJSON_AddStringToObject(acc, "lastName",  last);
    cJSON_AddNumberToObject(acc, "balance",   balance);
    cJSON_AddItemToArray(arr, acc);

    // Convert to string and write to file
    char *text = cJSON_Print(root);
    FILE *f = fopen(filename, "w");
    fputs(text, f);
    fclose(f);

    // Clean up
    free(text);
    cJSON_Delete(root);

    printf("Saved to %s\n", filename);
}

// Load and print all accounts from a JSON file
void load_accounts(const char *filename) {
    // Read file into a string
    FILE *f = fopen(filename, "r");
    if (!f) { printf("File not found.\n"); return; }

    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    rewind(f);

    char *text = malloc(len + 1);
    fread(text, 1, len, f);
    text[len] = '\0';
    fclose(f);

    // Parse JSON
    cJSON *root = cJSON_Parse(text);
    free(text);  // done with the raw string

    if (!root) { printf("JSON parse error.\n"); return; }

    // Read the array
    cJSON *arr = cJSON_GetObjectItem(root, "accounts");
    int size = cJSON_GetArraySize(arr);

    for (int i = 0; i < size; i++) {
        cJSON *item = cJSON_GetArrayItem(arr, i);
        printf("Account %d: %s %s, Balance: %.2f\n",
            cJSON_GetObjectItem(item, "acctNum")->valueint,
            cJSON_GetObjectItem(item, "firstName")->valuestring,
            cJSON_GetObjectItem(item, "lastName")->valuestring,
            cJSON_GetObjectItem(item, "balance")->valuedouble);
    }

    // Clean up
    cJSON_Delete(root);
}

int main(void) {
    save_account("test.json", 101, "Alice", "Smith", 500.00);
    load_accounts("test.json");
    return 0;
}
```

Compile and run:
```bash
gcc -o test_cjson test_cjson.c cJSON.c -lm
./test_cjson
```

Output:
```
Saved to test.json
Account 101: Alice Smith, Balance: 500.00
```

---

## 9. Quick Reference

| Function | What it does |
|---|---|
| `cJSON_CreateObject()` | Create an empty `{}` |
| `cJSON_CreateArray()` | Create an empty `[]` |
| `cJSON_AddNumberToObject(obj, key, num)` | Add `"key": 123` to an object |
| `cJSON_AddStringToObject(obj, key, str)` | Add `"key": "text"` to an object |
| `cJSON_AddItemToObject(obj, key, child)` | Add a child node to an object |
| `cJSON_AddItemToArray(arr, item)` | Append an item to an array |
| `cJSON_Parse(string)` | Parse a JSON string into a tree |
| `cJSON_Print(node)` | Convert a tree to a pretty JSON string |
| `cJSON_GetObjectItem(obj, key)` | Get a child node by key name |
| `cJSON_GetArraySize(arr)` | Get the number of items in an array |
| `cJSON_GetArrayItem(arr, index)` | Get an item by position |
| `cJSON_ReplaceItemInArray(arr, i, new)` | Replace item at position `i` |
| `cJSON_DeleteItemFromArray(arr, i)` | Remove item at position `i` |
| `cJSON_Delete(root)` | Free the entire tree |

---

## 10. Common Mistakes

**Forgetting to free memory:**
```c
// BAD -- memory leak
char *text = cJSON_Print(obj);
printf("%s\n", text);
// forgot free(text)!

// GOOD
char *text = cJSON_Print(obj);
printf("%s\n", text);
free(text);
```

**Deleting a child instead of the root:**
```c
// BAD -- will cause crashes
cJSON *arr = cJSON_GetObjectItem(root, "accounts");
cJSON_Delete(arr);  // don't do this!

// GOOD -- delete the root, children are freed automatically
cJSON_Delete(root);
```

**Not checking for NULL after parse:**
```c
// BAD -- will crash if JSON is invalid
cJSON *root = cJSON_Parse(text);
cJSON *arr = cJSON_GetObjectItem(root, "accounts");  // crash if root is NULL

// GOOD
cJSON *root = cJSON_Parse(text);
if (root == NULL) {
    printf("Invalid JSON!\n");
    return;
}
```

---

## Next Step

Open [storage/cjson/storage_cjson.c](storage/cjson/storage_cjson.c) and read
through it. You now know every cJSON function it uses. Try to follow how each
storage operation (add, update, delete, get) works in terms of the cJSON calls
described above.
