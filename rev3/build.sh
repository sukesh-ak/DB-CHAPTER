#!/bin/bash

# Create the data folder (if it doesn't exist)
mkdir -p data

# Compile with binary file storage
gcc -o bank main.c app/account.c app/account_service.c storage/storage_file.c
