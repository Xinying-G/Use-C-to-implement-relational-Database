#include <stdlib.h>
#include <stdio.h>
#include <libpq-fe.h>
#include <sys/param.h>
#include "txn.h"
#include "db.h"

const int BALANCE_FAILED = INT_MIN;
const int AUDIT_FAILED = INT_MIN;

static int update_balance(PGconn* connection, int account_id, int amount)
{
    // Run SQL to increment balance by amount.
    static const char* UPDATE_BALANCE = ""
    "update account\n"
    "set balance = balance + $1\n"
    "where account_id = $2\n";

    char c_account_id[10],c_amount[10];
    sprintf(c_account_id,"%d",account_id);
    sprintf(c_amount,"%d",amount);
    const char* params[] = {c_amount,c_account_id};
    int ok =  update(connection, UPDATE_BALANCE, 2, params);
    return ok;

}

int read_balance(PGconn* connection, int account_id)
{
    // Run SQL to read balance of account.
    static const char* READ_BALANCE = ""
    "select balance\n"
    "from account\n"
    "where account_id = $1\n";

    int balance;
    char c_account_id[10];
    sprintf(c_account_id,"%d",account_id);
    const char* params[] = {c_account_id};
    PGresult* result = query(connection, READ_BALANCE, 1, params);
    if(PQresultStatus(result) == PGRES_FATAL_ERROR)
        return BALANCE_FAILED;
    else if (PQntuples(result) == 1) {
        balance = atoi(field(result, 0, 0));
    }
    end_query(result);
    return balance;
}

int transfer(PGconn* connection, int from_id, int to_id, int amount)
{
    // Update smaller accont id first to avoid deadlock.
    int ok = 1;
    if (from_id < to_id) {
        ok = ok && update_balance(connection, from_id, -amount);
        ok = ok && update_balance(connection, to_id, amount);
    } else {
        ok = ok && update_balance(connection, to_id, amount);
        ok = ok && update_balance(connection, from_id, -amount);
    }
    return ok;
}

int audit(PGconn* connection)
{
    // Run SQL to compute sum of balances.
    static const char* AUDIT = ""
    "select sum(balance)\n"
    "from account\n";

    int balance;
    PGresult* result = query(connection, AUDIT, 0, NULL);
    if(PQresultStatus(result) == PGRES_FATAL_ERROR)
        return AUDIT_FAILED;
    if (PQntuples(result) == 1) {
        balance = atoi(field(result, 0, 0));
    }
    end_query(result);
    return balance;
}
