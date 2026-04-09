#!/bin/bash

# Create the data folder (if it doesn't exist)
mkdir -p data

# Compile with JSON storage
gcc -o bank main.c app/account.c app/account_service.c storage/cjson/storage_cjson.c storage/cjson/cJSON.c -lm
