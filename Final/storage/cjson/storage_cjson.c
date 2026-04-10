#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../storage.h"
#include "cJSON.h"

// Path to the JSON file
static const char *JSON_FILE = "data/accounts.json";

// --- Internal helpers ---

// Read entire file into a string (caller must free)
static char *read_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    rewind(f);

    char *buf = malloc(len + 1);
    if (!buf) { fclose(f); return NULL; }

    fread(buf, 1, len, f);
    buf[len] = '\0';
    fclose(f);
    return buf;
}

// Write a string to a file
static int write_file(const char *path, const char *text) {
    FILE *f = fopen(path, "w");
    if (!f) return -1;
    fputs(text, f);
    fclose(f);
    return 0;
}

// Parse the JSON file. Returns empty {"accounts":[]} if file
// doesn't exist yet.
static cJSON *load_json(void) {
    char *text = read_file(JSON_FILE);
    if (!text) {
        cJSON *root = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "accounts",
                              cJSON_CreateArray());
        return root;
    }
    cJSON *root = cJSON_Parse(text);
    free(text);
    if (!root) {
        root = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "accounts",
                              cJSON_CreateArray());
    }
    return root;
}

// Save the root JSON object to the file
static int save_json(cJSON *root) {
    char *text = cJSON_Print(root);
    if (!text) return -1;
    int result = write_file(JSON_FILE, text);
    free(text);
    return result;
}

// Convert an Account struct to a cJSON object
static cJSON *account_to_json(const Account *acc) {
    cJSON *obj = cJSON_CreateObject();
    cJSON_AddNumberToObject(obj, "acctNum",   acc->acctNum);
    cJSON_AddStringToObject(obj, "firstName", acc->firstName);
    cJSON_AddStringToObject(obj, "lastName",  acc->lastName);
    cJSON_AddNumberToObject(obj, "balance",   acc->balance);
    return obj;
}

// Fill an Account struct from a cJSON object
static void json_to_account(const cJSON *obj, Account *acc) {
    acc->acctNum = cJSON_GetObjectItem(obj, "acctNum")->valueint;

    strncpy(acc->firstName,
            cJSON_GetObjectItem(obj, "firstName")->valuestring,
            sizeof(acc->firstName) - 1);
    acc->firstName[sizeof(acc->firstName) - 1] = '\0';

    strncpy(acc->lastName,
            cJSON_GetObjectItem(obj, "lastName")->valuestring,
            sizeof(acc->lastName) - 1);
    acc->lastName[sizeof(acc->lastName) - 1] = '\0';

    acc->balance = cJSON_GetObjectItem(obj, "balance")->valuedouble;
}

// --- Interface implementation ---

int storage_init(void) {
    // Nothing to set up -- the file is created on first write
    return 0;
}

int storage_add(Account *acc) {
    cJSON *root = load_json();
    cJSON *arr  = cJSON_GetObjectItem(root, "accounts");

    // Check if account already exists
    int size = cJSON_GetArraySize(arr);
    for (int i = 0; i < size; i++) {
        cJSON *item = cJSON_GetArrayItem(arr, i);
        if (cJSON_GetObjectItem(item, "acctNum")->valueint
                == acc->acctNum) {
            cJSON_Delete(root);
            return -1;  // already exists
        }
    }

    cJSON_AddItemToArray(arr, account_to_json(acc));
    int result = save_json(root);
    cJSON_Delete(root);
    return result;
}

int storage_update(Account *acc) {
    cJSON *root = load_json();
    cJSON *arr  = cJSON_GetObjectItem(root, "accounts");

    int size = cJSON_GetArraySize(arr);
    for (int i = 0; i < size; i++) {
        cJSON *item = cJSON_GetArrayItem(arr, i);
        if (cJSON_GetObjectItem(item, "acctNum")->valueint
                == acc->acctNum) {
            cJSON_ReplaceItemInArray(arr, i, account_to_json(acc));
            int result = save_json(root);
            cJSON_Delete(root);
            return result;
        }
    }

    cJSON_Delete(root);
    return -1;  // not found
}

int storage_delete(int acctNum) {
    cJSON *root = load_json();
    cJSON *arr  = cJSON_GetObjectItem(root, "accounts");

    int size = cJSON_GetArraySize(arr);
    for (int i = 0; i < size; i++) {
        cJSON *item = cJSON_GetArrayItem(arr, i);
        if (cJSON_GetObjectItem(item, "acctNum")->valueint
                == acctNum) {
            cJSON_DeleteItemFromArray(arr, i);
            int result = save_json(root);
            cJSON_Delete(root);
            return result;
        }
    }

    cJSON_Delete(root);
    return -1;  // not found
}

int storage_get(int acctNum, Account *out) {
    cJSON *root = load_json();
    cJSON *arr  = cJSON_GetObjectItem(root, "accounts");

    int size = cJSON_GetArraySize(arr);
    for (int i = 0; i < size; i++) {
        cJSON *item = cJSON_GetArrayItem(arr, i);
        if (cJSON_GetObjectItem(item, "acctNum")->valueint
                == acctNum) {
            json_to_account(item, out);
            cJSON_Delete(root);
            return 0;  // found
        }
    }

    cJSON_Delete(root);
    return -1;  // not found
}

int storage_get_all(Account *list, int max) {
    cJSON *root = load_json();
    cJSON *arr  = cJSON_GetObjectItem(root, "accounts");

    int size = cJSON_GetArraySize(arr);
    int count = (size < max) ? size : max;

    for (int i = 0; i < count; i++) {
        json_to_account(cJSON_GetArrayItem(arr, i), &list[i]);
    }

    cJSON_Delete(root);
    return count;
}
