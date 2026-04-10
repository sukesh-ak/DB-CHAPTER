@echo off

REM Create the data folder (if it doesn't exist)
if not exist data mkdir data

REM Compile with SQLite storage
cl /Fe:bank.exe main.c app\account.c app\account_service.c storage\sqlite\storage_sqlite.c storage\sqlite\sqlite3.c
