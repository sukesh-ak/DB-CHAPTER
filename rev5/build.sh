#!/bin/bash

# Create the data folder (if it doesn't exist)
mkdir -p data

# Compile with SQLite storage
gcc -o bank main.c app/account.c app/account_service.c storage/sqlite/storage_sqlite.c storage/sqlite/sqlite3.c -lpthread -ldl -lm
