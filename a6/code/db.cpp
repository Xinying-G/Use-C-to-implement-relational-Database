#include <cstdlib>
#include "db.h"
#include <stdio.h>
#include <string.h>

static void oops(PGconn *cnxn, const char* problem)
{
    fprintf(stderr, "%s: %s", problem, PQerrorMessage(cnxn));
    PQfinish(cnxn);
    exit(1);
}

PGconn* connect_to_database(const char* host, const char* dbname, const char* user, const char* password)
{
    
    PGconn *cnxn;
    //char *cnxn_info = "host = postgres.cs.tufts.edu dbname = xguo07 user = xguo07 password = Gxygxy950827";
      char cnxn_info[250];
      strcpy(cnxn_info,"host = ");
      strcat(cnxn_info,host);
      strcat(cnxn_info," ");
      strcat(cnxn_info,"dbname = ");
      strcat(cnxn_info,dbname);
      strcat(cnxn_info," ");
      strcat(cnxn_info,"user = ");
      strcat(cnxn_info,user);
      strcat(cnxn_info," ");
      strcat(cnxn_info,"password = ");
      strcat(cnxn_info,password);




      cnxn = PQconnectdb(cnxn_info);
      if (PQstatus(cnxn) != CONNECTION_OK) {
        oops(cnxn, "connection to database failed");}
    
    return cnxn;
}

void disconnect_from_database(PGconn* connection)
{
	PQfinish(connection);
}

void begin_transaction(PGconn* connection)
{
	  PGresult *res;
	  res = PQexec(connection, "begin");
      if (PQresultStatus(res) != PGRES_COMMAND_OK)
        {
          PQclear(res);
          oops(connection, "begin failed");
        }
      // res has to be cleared after receiving a value
      PQclear(res);
}

int commit_transaction(PGconn* connection)
{
          PGresult *res;
      res = PQexec(connection, "commit");
      if (PQresultStatus(res) != PGRES_COMMAND_OK)
        {
          PQclear(res);
          return 0;
          // oops(cnxn, "commit failed");
        }
      PQclear(res);
      return 1;

      // if (PQresultStatus(res) == PGRES_COMMAND_OK)
      // 	return 1;
      // else 
      // 	return 0;
}

PGresult* query(PGconn* connection, const char* sql, int n_params, const char** params)
{
      char message[80];
      strcpy(message,"select");
      for(int i = 0; i < n_params - 1; i++)
      {
      	strcpy(message,params[i]);
      	strcpy(message,", ");
      }

      strcpy(message,params[n_params - 1]);
      strcpy(message,"from ");
      strcpy(message,sql);

      PGresult *res;
      res = PQexec(connection, message);

    if (PQresultStatus(res) == PGRES_FATAL_ERROR)
    {
        PQclear(res);
        return NULL;
    }
    else if (PQresultStatus(res) == PGRES_TUPLES_OK)
    {
    	return res;
    }
    else{
      PQclear(res);
      return NULL;
    }


}

int update(PGconn* connection, const char* sql, int n_params, const char** params)
{
        PGresult *res;
        res = PQexecParams(connection,
                           sql,
                           n_params, // Number of inputs
                           NULL, // Postgres will deduce input types
                           params, // Parameters as strings
                           NULL, // Parameter lengths, not needed for text params
                           NULL, // Parameter formats, not needed for text params
                           0); // Obtain results in text format
        if (PQresultStatus(res) == PGRES_COMMAND_OK)
        {
            
            int tuples = 0;
            if(PQcmdTuples(res)){
             tuples= atoi(PQcmdTuples(res));
            }
            // printf("this is status %s\n",PQcmdTuples(res));
            PQclear(res);
            return tuples;
        }
        else if(PQresultStatus(res) == PGRES_FATAL_ERROR)
        {
        	PQclear(res);
        	return -1;
        } else
        {   fprintf(stderr, "%s: %s", "update failed", PQerrorMessage(connection));
        	PQclear(res);
        	return - 2;
        }

}

const char* field(PGresult* result, int row, int column)
{
    if (PQgetisnull(result, row, column)) {
        return NULL;
    } else {
        char* field = PQgetvalue(result, row, column);
        return field;
    }
}

void end_query(PGresult* result)
{
	PQclear(result);
}
