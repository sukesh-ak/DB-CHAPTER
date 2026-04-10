#include <stdio.h>
#include <string.h>
#include "../storage.h"
#include "sqlite3.h"

static sqlite3 *db = NULL;

int storage_init(void) {
    int rc = sqlite3_open("data/accounts.db", &db);
    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    const char *sql =
        "CREATE TABLE IF NOT EXISTS accounts ("
        "  acctNum   INTEGER PRIMARY KEY,"
        "  firstName TEXT,"
        "  lastName  TEXT,"
        "  balance   REAL"
        ");";

    char *err = NULL;
    rc = sqlite3_exec(db, sql, NULL, NULL, &err);
    if (rc != SQLITE_OK) {
        printf("SQL error: %s\n", err);
        sqlite3_free(err);
        return -1;
    }
    return 0;
}

int storage_add(Account *acc) {
    // First check if account exists
    Account existing;
    if (storage_get(acc->acctNum, &existing) == 0) {
        return -1;  // already exists
    }

    const char *sql =
        "INSERT INTO accounts (acctNum, firstName, lastName, balance) "
        "VALUES (?, ?, ?, ?);";

    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_int(stmt, 1, acc->acctNum);
    sqlite3_bind_text(stmt, 2, acc->firstName, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, acc->lastName, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 4, acc->balance);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE) ? 0 : -1;
}

int storage_update(Account *acc) {
    const char *sql =
        "UPDATE accounts SET firstName=?, lastName=?, balance=? "
        "WHERE acctNum=?;";

    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_text(stmt, 1, acc->firstName, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, acc->lastName, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 3, acc->balance);
    sqlite3_bind_int(stmt, 4, acc->acctNum);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) return -1;

    // sqlite3_changes returns 0 if no row matched the WHERE clause
    return (sqlite3_changes(db) > 0) ? 0 : -1;
}

int storage_delete(int acctNum) {
    const char *sql = "DELETE FROM accounts WHERE acctNum=?;";

    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, acctNum);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) return -1;
    return (sqlite3_changes(db) > 0) ? 0 : -1;
}

int storage_get(int acctNum, Account *out) {
    const char *sql =
        "SELECT acctNum, firstName, lastName, balance "
        "FROM accounts WHERE acctNum=?;";

    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, acctNum);

    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        out->acctNum = sqlite3_column_int(stmt, 0);

        strncpy(out->firstName,
                (const char *)sqlite3_column_text(stmt, 1),
                sizeof(out->firstName) - 1);
        out->firstName[sizeof(out->firstName) - 1] = '\0';

        strncpy(out->lastName,
                (const char *)sqlite3_column_text(stmt, 2),
                sizeof(out->lastName) - 1);
        out->lastName[sizeof(out->lastName) - 1] = '\0';

        out->balance = sqlite3_column_double(stmt, 3);
        sqlite3_finalize(stmt);
        return 0;  // found
    }

    sqlite3_finalize(stmt);
    return -1;  // not found
}

int storage_get_all(Account *list, int max) {
    const char *sql =
        "SELECT acctNum, firstName, lastName, balance "
        "FROM accounts ORDER BY acctNum;";

    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && count < max) {
        list[count].acctNum = sqlite3_column_int(stmt, 0);

        strncpy(list[count].firstName,
                (const char *)sqlite3_column_text(stmt, 1),
                sizeof(list[count].firstName) - 1);
        list[count].firstName[sizeof(list[count].firstName) - 1] = '\0';

        strncpy(list[count].lastName,
                (const char *)sqlite3_column_text(stmt, 2),
                sizeof(list[count].lastName) - 1);
        list[count].lastName[sizeof(list[count].lastName) - 1] = '\0';

        list[count].balance = sqlite3_column_double(stmt, 3);
        count++;
    }

    sqlite3_finalize(stmt);
    return count;
}
