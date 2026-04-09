@echo off

REM Create the data folder (if it doesn't exist)
if not exist data mkdir data

REM Compile with JSON storage
cl /Fe:bank.exe main.c app\account.c app\account_service.c storage\cjson\storage_cjson.c storage\cjson\cJSON.c
