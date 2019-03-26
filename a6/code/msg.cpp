#include <cstdlib>
#include <cstdio>
#include <libpq-fe.h>
#include <cstring>
#include "msg.h"
#include "db.h"

static const char* INSERT_MEMBER =
    "insert into member(name, birth_date) values($1, $2)";

static const char* INSERT_MESSAGE =
    "insert into message(message_date, message_text) values($1, $2)";

static const char* INSERT_ROUTING =
    "select store_routing($1, $2, $3)";

static const char* PK_VALUE =
    "select lastval()";

static const char* BIRTH_DATE_QUERY =
    "select birth_date from member where name = $1";

static const char* SENDER_AND_RECEIVER_QUERY = ""
    "select distinct f.name, t.name\n"
    "from member f join routing r on f.member_id = r.from_member_id\n"
    "              join member t on r.to_member_id = t.member_id\n"
    "              join message m on r.message_id = m.message_id\n"
    "where m.message_date = $1\n"
    "order by f.name, t.name";


// API

int add_member(PGconn* connection, const char* name, const char* birth_date)
{
    // Run INSERT_MEMBER
    // Run PK_VALUE
    // Return PK_VALUE output (the member_id just inserted)
    const char* params[] = {name, birth_date};
    PGresult *res;
    res = PQexecParams(connection,
                           INSERT_MEMBER,
                           2, // Number of inputs
                           NULL, // Postgres will deduce input types
                           params, // Parameters as strings
                           NULL, // Parameter lengths, not needed for text params
                           NULL, // Parameter formats, not needed for text params
                           0); // Obtain results in text format
        if (PQresultStatus(res) != PGRES_COMMAND_OK)
        {
            PQclear(res);
            return 0;
            // oops(connection, "insert failed");
        }
        // printf("Rows inserted: %s\n", PQcmdTuples(res));

        PGresult *res_pk = PQexec(connection,PK_VALUE);
        char* pk_value = PQgetvalue(res_pk, 0, 0);
        int pk_int = atoi(pk_value);
        PQclear(res);
        PQclear(res_pk);
    return pk_int;
}

int add_message(PGconn* connection, const char* message_date, const char* message_text)
{
    // Run INSERT_MESSAGE
    // Run PK_VALUE
    // Return PK_VALUE output (the message_id just inserted)
    PGresult *res;
    const char* params[] = {message_date, message_text};
    res = PQexecParams(connection,
                           INSERT_MESSAGE,
                           2, // Number of inputs
                           NULL, // Postgres will deduce input types
                           params, // Parameters as strings
                           NULL, // Parameter lengths, not needed for text params
                           NULL, // Parameter formats, not needed for text params
                           0); // Obtain results in text format
        if (PQresultStatus(res) != PGRES_COMMAND_OK)
        {
            PQclear(res);
            return 0;
            // oops(connection, "insert failed");
        }
        // printf("Rows inserted: %s\n", PQcmdTuples(res));

        PGresult *res_pk = PQexec(connection,PK_VALUE);
        char* pk_value = PQgetvalue(res_pk, 0, 0);
        int pk_int = atoi(pk_value);
        PQclear(res);
        PQclear(res_pk);
    return pk_int;
}

int add_routing(PGconn* connection, const char* from_member_id, const char* to_member_id, const char* message_ids)
{
    // Run INSERT_ROUTING
    // Return the value returned by the insert_routing stored procedure
    const char* params[] = {from_member_id, to_member_id,message_ids};
    PGresult *res;
    res = PQexecParams(connection,
                           INSERT_ROUTING,
                           3, // Number of inputs
                           NULL, // Postgres will deduce input types
                           params, // Parameters as strings
                           NULL, // Parameter lengths, not needed for text params
                           NULL, // Parameter formats, not needed for text params
                           0); // Obtain results in text format

        if (PQresultStatus(res) != PGRES_TUPLES_OK)
        {
            // fprintf(stderr, "Add routing failed: %s", PQresultErrorMessage(res));
            fprintf(stderr, "Add routing failed: %s", PQresultErrorMessage(res));            
            PQclear(res);
            // printf("Add Routing row failed: %s\n", PQcmdTuples(res));
            // oops(connection, "insert failed");
        }

    char* num = PQgetvalue(res, 0, 0);
    int num_i = atoi(num);
    PQclear(res);

    return num_i;
}

int birth_date(PGconn* connection, const char* name, char* buffer)
{
    // Run BIRTH_DATE_QUERY
    const char* params[] = {name};
    PGresult *res;
    res = PQexecParams(connection,
                       BIRTH_DATE_QUERY,
                       1,       /* one param */
                       NULL,    /* let the backend deduce param type */
                       params,
                       NULL,    /* don't need param lengths since text */
                       NULL,    /* default to all text params */
                       0);      /* ask for binary results */
                         
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {        
        PQclear(res);
        return 0;
    }

    if (PQntuples(res) == 0)
    {
        PQclear(res);
        return 0;
    }

    strcpy(buffer,PQgetvalue(res, 0, 0));
    PQclear(res);
    return 1;
}

int senders_and_receivers(PGconn* connection, const char* date, int max_results, char* buffer[])
{
    // Run SENDER_AND_RECEIVER_QUERY
    const char* params[] = {date};
    PGresult *res;
    res = PQexecParams(connection,
                           SENDER_AND_RECEIVER_QUERY,
                           1, // Number of inputs
                           NULL, // Postgres will deduce input types
                           params, // Parameters as strings
                           NULL, // Parameter lengths, not needed for text params
                           NULL, // Parameter formats, not needed for text params
                           0); // Obtain results in text format
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        PQclear(res);
        fprintf(stderr, "11number of pairs exceeded the max: \n");
    }

    int pairs = PQntuples(res);
      if (pairs > max_results) {
        fprintf(stderr, "number of pairs exceeded the max: %d\n", pairs);
        exit(0);
      }

        for (int r = 0; r < PQntuples(res); r++) {
        for (int c = 0; c < PQnfields(res); c++) {
          if (PQgetisnull(res, r, c)) {
            //printf("NULL\t");
          } else {
              char* field = PQgetvalue(res, r, c);
              if(c != PQnfields(res) - 1)
              {
                  strcpy(buffer[r],field);
                  strcat(buffer[r],",");
              }
              else
                strcat(buffer[r],field);
          }
        }
      }      
        PQclear(res);
    return pairs;
}
