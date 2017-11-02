/*
 * redis_demo.cpp
 *
 *  Created on: Dec 20, 2016
 *      Author: lw
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hiredis/hiredis.h>

#if 0

int main(int argc, char **argv) {
	unsigned int j;
	redisContext *c;
	redisReply *reply;
	const char *hostname = (argc > 1) ? argv[1] : "192.168.204.128";
	int port = (argc > 2) ? atoi(argv[2]) : 6379;

	struct timeval timeout = { 10, 500000 }; // 1.5 seconds
	c = redisConnectWithTimeout(hostname, port, timeout);
	if (c == nullptr || c->err) {
		if (c) {
			printf("Connection error: %s\n", c->errstr);
			redisFree(c);
		} else {
			printf("Connection error: can't allocate redis context\n");
		}
		exit(1);
	}

	/* PING server */
	reply = (redisReply *) redisCommand(c, "PING");
	printf("PING: %s\n", reply->str);
	freeReplyObject(reply);

	{
		struct VT {
			int a;
			double b;
			unsigned int c;
			char d;
		};

		VT vt;
		vt.a = 100;
		vt.b = 100.121;
		vt.c = 100;
		vt.d = 'd';
		const char *v[3];
		size_t vlen[3];
		v[0] = "SET";
		vlen[0] = strlen("SET");

		v[1] = "C";
		vlen[1] = strlen("C");

		v[2] = (char*) &vt;
		vlen[2] = sizeof(vt);

		reply = (redisReply *) redisCommandArgv(c,
				sizeof(v) / sizeof(v[0]), v, vlen);
		printf("SET: %s\n", reply->str);
		freeReplyObject(reply);

		reply = (redisReply *) redisCommand(c, "GET C");
		printf("GET foo: %s\n", reply->str);

		VT *pvt = (VT*) reply->str;
		freeReplyObject(reply);
	}

	/* Set a key */
	reply = (redisReply *) redisCommand(c, "SET %s %s", "foo", "hello world");
	printf("SET: %s\n", reply->str);
	freeReplyObject(reply);

	/* Set a key using binary safe API */
	reply = (redisReply *) redisCommand(c, "SET %b %b", "bar", (size_t) 3,
			"hello", (size_t) 5);
	printf("SET (binary API): %s\n", reply->str);
	freeReplyObject(reply);

	/* Try a GET and two INCR */
	reply = (redisReply *) redisCommand(c, "GET foo");
	printf("GET foo: %s\n", reply->str);
	freeReplyObject(reply);

	reply = (redisReply *) redisCommand(c, "INCR counter");
	printf("INCR counter: %lld\n", reply->integer);
	freeReplyObject(reply);

	/* again ... */
	reply = (redisReply *) redisCommand(c, "INCR counter");
	printf("INCR counter: %lld\n", reply->integer);
	freeReplyObject(reply);

	/* Create a list of numbers, from 0 to 9 */
	reply = (redisReply *) redisCommand(c, "DEL mylist");
	freeReplyObject(reply);

	for (j = 0; j < 100; j++) {
		char buf[64];
		snprintf(buf, 64, "%d", j);

		reply = (redisReply *) redisCommand(c, "LPUSH mylist element-%s", buf);
		freeReplyObject(reply);
	}

	/* Let's check what we have inside the list */
	reply = (redisReply *) redisCommand(c, "LRANGE mylist 0 -1");
	if (reply->type == REDIS_REPLY_ARRAY) {
		for (j = 0; j < reply->elements; j++) {
			printf("%u) %s\n", j, reply->element[j]->str);
		}
	}
	freeReplyObject(reply);

	/* Disconnects and frees the context */
	redisFree(c);

	return 0;
}

#endif
