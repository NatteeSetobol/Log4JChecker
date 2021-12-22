#ifndef __HTTP_PROTOCOL_H__
#define  __HTTP_PROTOCOL_H__

#include "socket.h"
#include "required/intrinsic.h"
#include "required/nix.h"
#include "required/memory.h"
#include "required/platform.h"
#include "token.h"
#include "bucket.h"
#if USE_SSL
#include "ssl.h"
#endif

enum http_token_type 
{
	HTTP_NONE,
	HTTP_LINEBREAK,
	HTTP_WLINEBREAK,
	HTTP_DATA,
	HTTP_TOKEN_COUNT,
};



struct header
{
	s32 *key;
	s32 *value;
};

struct http_header
{
	struct bucket_list headers;
	s32 *body;
};

struct http_download_info
{
	us32 *data;
	int length;
};

struct http_headers
{
	i32 keys[256];
	struct hash headers[256];
	i32 total;
};

http_download_info HttpSkipHeader(void *data,int length);
http_download_info  HttpRequestDownload(char* url, struct bucket_list* headers, Method method, char* postData);
char* HttpRequest(char* url, struct bucket_list* headers, Method method, char* postData,int targetPort);
enum http_token_type GetHeaderToken(struct tokenizer *tokenizer);
struct http_header HttpParseHeader(char *data);
void *FreeHttpHeader(struct http_header *header);
b32 AddToHeader(struct memory_arena *arena, struct http_headers *httpHeaders, s32* key, s32* value);
struct http_download_info HTTPSRequest(memory_arena *arena,s32* url, http_headers *httpHeaders, Method method, s32 *postData, int targetPort);
#endif
