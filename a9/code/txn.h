#ifndef A4_TXN_H
#define A4_TXN_H

#include <libpq-fe.h>

extern const int BALANCE_FAILED;
extern const int AUDIT_FAILED;

/*
 * Return the balance of the given account. If the balance cannot be read, due to a concurrency conflict,
 * return BALANCE_FAILED.
 */
int read_balance(PGconn* connection, int account_id);

/*
 * Transfer the given amount from the account identified by from_id, to the account identified by
 * to_id. Amount is expected to not exceed the from_id account's balance. Return a non-zero value on
 * success. Zero indicates that the update failed due to a concurrency conflict.
 */
int transfer(PGconn* connection, int from_id, int to_id, int amount);

/*
 * Return the sum of balances. In case of a concurrency conflict, AUDIT_FAILED is returned.
 */
int audit(PGconn* connection);

#endif //A4_TXN_H
