#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <wait.h>
#include "db.h"
#include "txn.h"

//----------------------------------------------------------------------------------------------------------------------

// Database setup

static const char* DDL[] = {
    "drop table if exists account cascade",
    "create table account("
        "account_id serial primary key, "
        "balance int not null)"
};

static void setup_database(const char* host, const char* dbname, const char* user, const char* password)
{
    PGconn* connection = connect_to_database(host, dbname, user, password);
    begin_transaction(connection, "serializable");
    int n = sizeof(DDL) / sizeof(const char*);
    for (int i = 0; i < n; i++) {
        update(connection, DDL[i], 0, NULL);
    }
    commit_transaction(connection);
    disconnect_from_database(connection);
}

//----------------------------------------------------------------------------------------------------------------------

// Exercising transactions

void run_transfers(const char* isolation,
                   const char* host,
                   const char* dbname,
                   const char* user,
                   const char* password,
                   int accounts,
                   int transactions_per_user,
                   int expected_sum_balances)
{
    const int PRIME = 5722697;
    srand((unsigned int) getpid() * PRIME);
    PGconn* connection = connect_to_database(host, dbname, user, password);
    int failed_audit = 0;
    int bad_audit = 0;
    int failed_transfer = 0;
    int failed_balance = 0;
    int bad_balance = 0;
    int failed_commit = 0;
    for (int t = 0; t < transactions_per_user; t++) {
        if (isolation) {
            begin_transaction(connection, isolation);
        }
        // Get two different accounts to operate on
        int source_id = 1 + rand() % accounts;
        int target_id;
        do {
            target_id = 1 + rand() % accounts;
        } while (source_id == target_id);
        // Generate a transfer amount > 0
        int balance = read_balance(connection, source_id);
        // printf("what happened %d\n",balance);
        if (balance == BALANCE_FAILED) {
            failed_balance++;
        } else if (balance < 0) {
            bad_balance++;
        } else {
            int amount = 1 + rand() % 100;
            if (amount > balance) {
                amount = balance;
            }
            // Do the transfer
            int transfer_ok = transfer(connection, source_id, target_id, amount);
            if (!transfer_ok) {
                failed_transfer++;
            }
        }
        if (isolation) {
            int ok = commit_transaction(connection);
            if (!ok) {
                failed_commit++;
            }
        }
        // Check balances
        if (isolation) {
            begin_transaction(connection, isolation);
        }
        int sum_balances = audit(connection);
        if (isolation) {
            int ok = commit_transaction(connection);
            if (!ok) {
                failed_commit++;
            }
        }
        if (sum_balances == AUDIT_FAILED) {
            failed_audit++;
        } else if (sum_balances != expected_sum_balances) {
            bad_audit++;
        }
    }
    disconnect_from_database(connection);
    printf(
        "    process %d -- bad balance: %d, failed balance: %d, failed transfer: %d, bad audit: %d, failed audit: %d, failed commit: %d\n",
        getpid(),
        bad_balance,
        failed_balance,
        failed_transfer,
        bad_audit,
        failed_audit,
        failed_commit);
}

static void initialize_accounts(PGconn* connection, int accounts)
{
    for (int a = 0; a < accounts; a++) {
        update(connection, "insert into account(balance) values(1000)", 0, NULL);
    }
}

static void spawn_workers(const char* isolation,
                          int n_processes,
                          const char* host,
                          const char* dbname,
                          const char* user,
                          const char* password,
                          int accounts,
                          int transactions_per_user)
{
    printf("BEGIN %s ------------------------------------------------------------------------------\n",
           isolation ? isolation : "auto-commit");
    // Get the expected sum of balances
    int expected_sum_balances;
    {
        PGconn* connection = connect_to_database(host, dbname, user, password);
        initialize_accounts(connection, accounts);
        expected_sum_balances = audit(connection);
        disconnect_from_database(connection);
    }
    // Spawn worker processes
    pid_t children[n_processes];
    int parent = 1;
    for (int p = 0; p < n_processes; p++) {
        pid_t child_pid = fork();
        if (child_pid == 0) {
            parent = 0;
            run_transfers(isolation, host, dbname, user, password, accounts, transactions_per_user, expected_sum_balances);
            break;
        } else {
            children[p] = child_pid;
        }
    }
    // Wait for the workers to complete
    if (parent) {
        for (int p = 0; p < n_processes; p++) {
            int status = 0;
            waitpid(children[p], &status, 0);
        }
        printf("END %s\n", isolation ? isolation : "auto-commit");
    }
}

static void test_transactions(const char* host,
                              const char* dbname,
                              const char* user,
                              const char* password,
                              const char* isolation)
{
    const int ACCOUNTS = 10;
    const int CONCURRENT_USERS = 10;
    const int TRANSACTIONS_PER_USER = 1000;
    spawn_workers(isolation, CONCURRENT_USERS, host, dbname, user, password, ACCOUNTS, TRANSACTIONS_PER_USER);
}

//----------------------------------------------------------------------------------------------------------------------

// Main

void usage()
{
    const char* USAGE =
            "a9 TXN_TEST HOST DBNAME USER PASSWORD\n"
            "TXN_TEST:\n"
            "    ac: test transactions without explicit transaction boundaries (i.e., auto-commit)\n"
            "    rc: test transactions with READ COMMITTED transactions\n"
            "    ser: test transactions with SERIALIZABLE transactions\n";
    fprintf(stderr, "%s\n", USAGE);
    exit(1);
}

int main(int argc, const char** argv)
{
    if (argc != 6) {
        usage();
    }
    int a = 1;
    const char* test = argv[a++];
    const char* host = argv[a++];
    const char* dbname = argv[a++];
    const char* user = argv[a++];
    const char* password = argv[a++];
    setup_database(host, dbname, user, password);
    if (!strcmp(test, "ac")) {
        test_transactions(host, dbname, user, password, NULL);
    } else if (!strcmp(test, "rc")) {
        test_transactions(host, dbname, user, password, "read committed");
    } else if (!strcmp(test, "ser")) {
        test_transactions(host, dbname, user, password, "serializable");
    } else {
        usage();
    }
}
