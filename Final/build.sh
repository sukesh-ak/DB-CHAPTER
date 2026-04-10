#!/bin/bash

# Create the data folder (if it doesn't exist)
mkdir -p data

# --- Binary file version ---
gcc -o bank_file main.c app/account.c app/account_service.c storage/file/storage_file.c

# --- JSON version ---
gcc -o bank_json main.c app/account.c app/account_service.c storage/cjson/storage_cjson.c storage/cjson/cJSON.c -lm

# --- SQLite version ---
gcc -o bank_sqlite main.c app/account.c app/account_service.c storage/sqlite/storage_sqlite.c storage/sqlite/sqlite3.c -lpthread -ldl -lm
