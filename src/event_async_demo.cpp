//============================================================================
// Name        : redis_demo.cpp
// Author      : lw
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <string>

#if 1

#include <hiredis/hiredis.h>
#include <hiredis/async.h>
#include <hiredis/adapters/libevent.h>

static void setCallback(redisAsyncContext *c, void *r, void *privdata) {
	redisReply *reply = (redisReply *) r;

	if (reply == nullptr)
		return;

	if (!(reply->type == REDIS_REPLY_STATUS && strcasecmp(reply->str, "OK") == 0)) {

		printf("failed to execute command[%s]\n", (char*) privdata);
		freeReplyObject(reply);
		redisAsyncFree(c);
		return;
	}

	printf("argv [%s]: %s\n", (char*) privdata, reply->str);
}

static void getCallback(redisAsyncContext *c, void *r, void *privdata) {
	redisReply *reply = (redisReply *) r;
	if (reply == nullptr)
		return;

	printf("argv [%s]: %s\n", (char*) privdata, reply->str);

	/* Disconnect after receiving the reply to GET */
	redisAsyncDisconnect(c);
}

static void connectCallback(const redisAsyncContext *c, int status) {
	if (status != REDIS_OK) {
		printf("error: %s\n", c->errstr);
		return;
	}

	printf("connected...\n");
}

static void disconnectCallback(const redisAsyncContext *c, int status) {
	if (status != REDIS_OK) {
		printf("Error: %s\n", c->errstr);
		return;
	}

	printf("disconnected...\n");
}

int main(int argc, char **argv) {
	signal(SIGPIPE, SIG_IGN);

	struct event_base *base = event_base_new();

	redisAsyncContext *c = redisAsyncConnect("192.168.204.128", 6379);
	if (c->err) {
		/* Let *c leak for now... */
		printf("error: %s\n", c->errstr);
		return 1;
	}

	int r = redisLibeventAttach(c, base);

	redisAsyncSetConnectCallback(c, connectCallback);
	redisAsyncSetDisconnectCallback(c, disconnectCallback);

	std::string k = "name";
	std::string v = "liwei";

	redisAsyncCommand(c, setCallback, (void*) "set", "SET %s %s", k.c_str(),
			v.c_str());

	redisAsyncCommand(c, getCallback, (void*) "get", (char*) "GET %s",
			k.c_str());

	r = event_base_dispatch(base);

	printf("r = %d\n", r);

	return 0;
}

#endif
