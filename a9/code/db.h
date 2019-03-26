#ifndef A4_DB_H
#define A4_DB_H

#include <libpq-fe.h>

/*
 * Connect to the database described by the arguments, and return the connection. If the status
 * after connecting is anything other than CONNECTION_OK, print an error message to stderr and exit(1).
 */
PGconn* connect_to_database(const char* host, const char* dbname, const char* user, const char* password);

/*
 * Disconnect from the database. Following this call, the connection is unusable.
 */
void disconnect_from_database(PGconn* connection);

/*
 * Begin a transaction at the given isolation level. This is implemented by
 * issuing the command "begin isolation level X", where X is the value of the isolation argument.
 * If the status after issuing the BEGIN command is anything other that PGRES_COMMAND_OK,
 * print an error message to stderr and exit(1).
 */
void begin_transaction(PGconn* connection, const char* isolation);

/*
 * Commit the currently open transaction. Return 1 if the status is PGRES_COMMAND_OK, indicating
 * success. For any other status, return 0.
 */
int commit_transaction(PGconn* connection);

/*
 * Run the query. The query has n_params parameters, whose values are in the params array.
 * Query execution results in a status of type ExecStatusType. The handling of status is
 * as follows:
 * - PGRES_FATAL_ERROR (indicates a concurrency conflict): Clear the PGresult and return NULL.
 * - PGRES_TUPLES_OK: Return the PGresult (DO NOT clear it).
 * - Anything else: Print an error message on stderr, clear the PGresult, and return NULL.
 */
PGresult* query(PGconn* connection, const char* sql, int n_params, const char* params[]);

/*
 * Run the update. The update has n_params parameters, whose values are in the params array.
 * Execution of the update results in a status of type ExecStatusType. The handling of status is
 * as follows:
 * - PGRES_FATAL_ERROR (indicates a concurrency conflict): Clear the PGresult and return -1.
 * - PGRES_COMMAND_OK: Clear the PGresult and return the update count (which can be obtained from PQcmdTuples()).
 * - Anything else: Print an error message on stderr, clear the PGresult, and return -2.
 */
int update(PGconn* connection, const char* sql, int n_params, const char* params[]);

/*
 * Return the value from the given row and column of the result. If PQgetisnull indicates that the field
 * is NULL then return NULL. Otherwise return the field value obtained by PQgetvalue.
 */
const char* field(PGresult* result, int row, int column);

/*
 * All rows of interest have been retrieved from the current query execution. End query
 * execution by calling PGclear on result.
 */
void end_query(PGresult* result);

#endif //A4_DB_H
