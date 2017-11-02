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

#if 0

#include <hiredis/hiredis.h>
#include <hiredis/async.h>
#include <hiredis/adapters/libuv.h>

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
	printf("argv[%s]: %s\n", (char*) privdata, reply->str);

	/* Disconnect after receiving the reply to GET */
	redisAsyncDisconnect(c);
}

static void connectCallback(const redisAsyncContext *c, int status) {
	if (status != REDIS_OK) {
		printf("Error: %s\n", c->errstr);
		return;
	}
	printf("Connected...\n");
}

static void disconnectCallback(const redisAsyncContext *c, int status) {
	if (status != REDIS_OK) {
		printf("Error: %s\n", c->errstr);
		return;
	}
	printf("Disconnected...\n");
}

int main(int argc, char **argv) {
	signal(SIGPIPE, SIG_IGN);
	uv_loop_t* loop = uv_default_loop();

	redisAsyncContext *c = redisAsyncConnect("192.168.137.130", 6379);
	if (c->err) {
		/* Let *c leak for now... */
		printf("Error: %s\n", c->errstr);
		return 1;
	}

	redisLibuvAttach(c, loop);

	redisAsyncSetConnectCallback(c, connectCallback);
	redisAsyncSetDisconnectCallback(c, disconnectCallback);

	std::string k = "name";
	std::string v = "liwei";

	redisAsyncCommand(c, setCallback, (char*) "SET", "SET key %b", k.c_str(),
			v.c_str());

	redisAsyncCommand(c, getCallback, (char*) "GET", "GET %s", k.c_str());

	uv_run(loop, UV_RUN_DEFAULT);

	return 0;

}

#endif
