@echo off

REM Create the data folder (if it doesn't exist)
if not exist data mkdir data

REM --- Binary file version ---
cl /Fe:bank_file.exe main.c app\account.c app\account_service.c storage\file\storage_file.c

REM --- JSON version ---
cl /Fe:bank_json.exe main.c app\account.c app\account_service.c storage\cjson\storage_cjson.c storage\cjson\cJSON.c

REM --- SQLite version ---
cl /Fe:bank_sqlite.exe main.c app\account.c app\account_service.c storage\sqlite\storage_sqlite.c storage\sqlite\sqlite3.c
