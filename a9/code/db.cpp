#include <stdlib.h>
#include "db.h"
#include <cstdlib>
#include <string.h>

PGconn* connect_to_database(const char* host, const char* dbname, const char* user, const char* password)
{
    char connection_info[100];
    sprintf(connection_info, "host = %s dbname = %s user = %s password = %s", host, dbname, user, password);
    PGconn* connection = PQconnectdb(connection_info);
    ConnStatusType status = PQstatus(connection);
    if (status != CONNECTION_OK) {
        printf("Couldn't connect to database\n");
        exit(1);
    }
    return connection;
}

void disconnect_from_database(PGconn* connection)
{
	PQfinish(connection);
}

void begin_transaction(PGconn* connection, const char* isolation)
{
		PGresult *res;
	if(strcmp(isolation, "read committed") == 0){
	  res = PQexec(connection, "begin transaction isolation level READ COMMITTED");
	}
	else if(strcmp(isolation, "serializable") == 0)
		res = PQexec(connection, "begin transaction isolation level SERIALIZABLE");

      if (PQresultStatus(res) != PGRES_COMMAND_OK)
        {
          PQclear(res);
        }
      // res has to be cleared after receiving a value
      PQclear(res);
}

int commit_transaction(PGconn* connection)
{
    PGresult* result = PQexec(connection, "commit");
    ExecStatusType status = PQresultStatus(result);
    PQclear(result);
    if (status == PGRES_FATAL_ERROR) {
        return 0;
    } else if (status != PGRES_COMMAND_OK) {
        fprintf(stderr, "Commit failed\n");
        return 0;
    } else {
        return 1;
    }
}

PGresult* query(PGconn* connection, const char* sql, int n_params, const char** params)
{
    PGresult* result = PQexecParams(connection, sql, n_params, NULL, params, NULL, NULL, 0);
    ExecStatusType status = PQresultStatus(result);
    if (status == PGRES_FATAL_ERROR) {
        PQclear(result);
        return NULL;
    } else if (status != PGRES_TUPLES_OK) {
        fprintf(stderr, "query failed with status %d: %s\n", status, sql);
        PQclear(result);
        return NULL;
    } else {
        return result;
    };
}

int update(PGconn* connection, const char* sql, int n_params, const char** params)
{
    PGresult* result = PQexecParams(connection, sql, n_params, NULL, params, NULL, NULL, 0);
    ExecStatusType status = PQresultStatus(result);
    if (status == PGRES_FATAL_ERROR) {
        PQclear(result);
        return 0;
    } else if (status != PGRES_COMMAND_OK) {
        fprintf(stderr, "update failed with status %d: %s\n", status, sql);
        PQclear(result);
        return 0;
    } else {
        int update_count = atoi(PQcmdTuples(result));
        PQclear(result);
        return update_count;
    }
}

const char* field(PGresult* result, int row, int column)
{
    return
        PQgetisnull(result, row, column)
        ? NULL
        : PQgetvalue(result, row, column);
}

void end_query(PGresult* result)
{
    PQclear(result);
}
