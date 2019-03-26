#ifndef A4_MSG_H
#define A4_MSG_H

#include <sys/param.h>
#include <libpq-fe.h>

/*
 * Add a member to the database. Return the new member's member_id.
 */
int add_member(PGconn* connection, const char* name, const char* birth_date);

/*
 * Add a message to the database, returning the new message's message_id.
 */
int add_message(PGconn* connection, const char* message_date, const char* message_text);

/*
 * Add routing information. message_ids contains the set of all message ids sent by from_member_id, and
 * received by to_member_id. The return value indicates the number of routing rows created, and should equal
 * the number of message_ids. The format of message_ids is a comma-sepated list of integers, contained in
 * curly braces, e.g. {14,19,207}.
 */
int add_routing(PGconn* connection, const char* from_member_id, const char* to_member_id, const char* message_ids);

/*
 * Find the birth date of the member with the given name. If there is a member with that name,
 * then it is placed in the provided buffer, and a non-zero value is returned. Otherwise, the buffer is not
 * modified, and 0 is returned.
 */
int birth_date(PGconn* connection, const char* name, char* buffer);

/*
 * Find the names of senders and receivers of messages sent on the given date. The ith pair of names
 * is placed in buffer[i], in the format SENDER_NAME,RECEIVER_NAME. The output should be sorted, (the comma
 * character ranks after all characters that can appear in a member's name, so you can rely on the ordering
 * provided by the query). Return the number of sender/receiver pairs found. If the number of results exceeds
 * max_results, then print a message to stderr and exit.
 */
int senders_and_receivers(PGconn* connection, const char* date, int max_results, char* buffer[]);

#endif //A4_MSG_H
