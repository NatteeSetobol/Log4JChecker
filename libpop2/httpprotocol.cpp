#include <stdio.h>
#include "httpprotocol.h"
/*
NOTES(): This requires SSL Lib,, please include in the build.sh with -L <path to SSL lib)  and -I (path to SSL Include files)

Example:

	FILE *wavefile = NULL;
	struct bing_voice_data bingData = {};

	bingData = BingTextToVoice("testing",EN);

	wavefile = fopen("test.wav","w");
	fwrite(bingData.data,1,bingData.length,wavefile);
	fclose(wavefile);


*/
/*
b32 AddToHeader(struct memory_arena *arena, struct http_headers *httpHeaders, s32* key, s32* value)
{
	i32 newKey = 0;
	b32 isFound = false;

	newKey =  AddToHash(httpHeaders->headers,key, value,64, arena);
	for (int keyIndex = 0; keyIndex < httpHeaders->total; ++keyIndex)
	{
		if (httpHeaders->keys[keyIndex] == newKey)
		{
			isFound = true;
			break;
		}
	}

	if (isFound == false)
	{
		httpHeaders->keys[httpHeaders->total] = newKey;
		++httpHeaders->total;
	}

	return isFound;
}
*/

struct header GetHeader(s32* string)
{
	struct tokenizer headerTokenizer = {};
	struct token headerToken = {};
	bool found = false;
	char *headerData = NULL;
	char *temp = NULL;
	struct header result = {};

	headerTokenizer.at = string;

	do 
	{
		char letter = headerTokenizer.at[0];
		switch (letter)
		{
			case ':':
			{
				if (found == false)
				{
					result.key = S32(headerData);

					if (headerData)
					{
						Free(headerData);
						headerData = NULL;
					}

					found = true;
					break;
				}

			} 
			default:
			{
				char tempChar[5] = {};
				tempChar[0] = headerTokenizer.at[0];
				if (headerData == NULL)
				{
					headerData = S32(tempChar);
				} else {
					temp = headerData;
					headerData = S32Cat(headerData,1,tempChar);

					if (temp)
					{
						Free(temp);
						temp=NULL;
					}

				}

				break;
			}
		}

		headerToken = MoveToken(&headerTokenizer);
	} while (  headerTokenizer.at[0] != '\0' );

	result.value= S32(headerData);

	if (headerData)
	{
		Free(headerData);
		headerData=NULL;
	}

	return result;
}


char* HttpsRequest(char* url, struct bucket_list* headers, Method method, char* postData, bool useSSL, int targetPort)
{
	char* result = NULL;
	char* fullHeader = NULL;
	char* hostname = NULL;
	char* temp = NULL;
	struct platform_socket psocket = {};
	int count = 0;
	struct sslinfo sslInfo = {};
	char *httpsString = NULL;
	bool isSSL = false;
	bool connected = false;
	
	if (Strlen(url) > 6)
	{
		httpsString = MidString(url,0,5);

		if (StrCmp(httpsString,"https"))
		{
			isSSL = true;
		}

		if (httpsString)
		{
			Free(httpsString);
			httpsString = NULL;
		}
	}

	if (useSSL)
	{
		if (!InitSSL(&sslInfo, url))
		{
			printf("can't not init SSL\n");
		} else {
			connected = true;
		}
	} else {
		psocket = CreateSocket(url,targetPort);
		connected = psocket.connected;
	}

	if (connected )
	{
		//TODO(): Don't forget to do the POST method 
		if (method == GET)
		{
			fullHeader = S32((char*) "GET");
		} else 
			if (method == OPTIONS)
			{
				fullHeader = S32((char*) "OPTIONS");
			} else 
				if (method == POST)
				{
					fullHeader = S32((char*) "POST");
				}

		temp = fullHeader;

		fullHeader = S32Cat(temp, 5, " ", url, " ", "HTTP/1.0", "\r\n");

			if (temp)
			{
				Free(temp);
					temp = NULL;
			}
		
			if (headers)
			{
				for (int headerIndex = 0; headerIndex < headers->count; headerIndex++)
				{
					char*  index = headers->keys[headerIndex];
					int breakhere = 0;
					char* buck = (char*) GetFromBucket(headers,index); //(bucket*)& headers->buckets[index];
					
						if (buck)
						{
							temp = fullHeader;
							fullHeader = S32Cat(temp, 4, index, ": ", buck, "\r\n");
								
								if (temp)
								{
									Free(temp);
										temp = NULL;
								}
							
						} else {
						}
				}
				
					if (temp)
					{
						Free(temp);
							temp = NULL;
					}
				
					if (method == GET | method == OPTIONS)
					{
						temp = fullHeader;
							fullHeader = S32Cat(temp, 1, "\r\n\r\n");
							
							if (temp)
							{
								Free(temp);
									temp = NULL;
							}
					} else 
						if (method == POST)
						{

							temp = fullHeader;

								if (postData)
								{
									fullHeader = S32Cat(temp, 3, "\r\n", postData, "\r\n\r\n");
								} else {
									fullHeader = S32Cat(temp, 1, "\r\n\r\n");
								}
							
								if (temp)
								{
									Free(temp);
										temp = NULL;
								}
		
						} else {
							printf("Bad Method\n");
						}
			}

		if (useSSL)
		{
			SSL_write(sslInfo.ssl,fullHeader,Strlen(fullHeader)-1);
		} else {
			SendToSocket(&psocket,fullHeader,Strlen(fullHeader));
		}
		//TODO(): Put this loop in a thread so it won't hang  
		do
		{
			char *tempBuffer = (char*) MemoryA(char*,535);

			if (useSSL)
			{
				count = SSL_read(sslInfo.ssl, tempBuffer,512);
			} else {
				count = RecvFromSocket(&psocket, tempBuffer,512);
			}

			//TODO(): Create a queuee and put all the data recived from into a
			//Queue.

			if (result)
			{
				char* tempResult = result;
				result = S32Cat(tempResult, 1, tempBuffer);

				if (tempResult)
				{
					Free(tempResult);
					tempResult = NULL;
				}
			}
			else {
				result = S32(tempBuffer);
			}



			if (tempBuffer)
			{
				Free(tempBuffer);
				tempBuffer = NULL;
			}

		} while (count != 0);


		
			if (fullHeader)
			{
				Free(fullHeader);
					fullHeader = NULL;
			}
		
			if (hostname)
			{
				Free(hostname);
					hostname = NULL;
			}
		
		if (useSSL) FreeSSL(&sslInfo);
	}

#ifndef COMPILER_MSVC
	CloseSocket(&psocket);
#endif
	return result;
}

enum http_token_type GetHeaderToken(struct tokenizer *tokenizer)
{
	enum  http_token_type result;

	switch(tokenizer->at[0])
	{
		case '\0': { result = HTTP_NONE; break; }
		case '\n': { result = HTTP_LINEBREAK ; break; }
		case '\r': { result = HTTP_WLINEBREAK ; break; }
		default: {   result = HTTP_DATA; break; }

	}
	return result;
}


struct http_header HttpParseHeader(char *data)
{

	struct http_header result = {};
	struct tokenizer headerTokenizer = {};
	struct token headerToken = {};
	bool found = false;
	char *headerData = NULL;
	char *temp = NULL;

	headerTokenizer.at = data;

	InitBucket(&result.headers,256,0);
	do
	{
		headerToken.type = GetHeaderToken(&headerTokenizer);
		switch(headerToken.type)
		{
			case HTTP_WLINEBREAK:
			case HTTP_LINEBREAK:
			{
				if (headerData)
				{

					struct tokenizer nextHeaderTokenizer = {};
					struct token nextHeaderToken = {};

					nextHeaderTokenizer.at = headerTokenizer.at;

					nextHeaderToken = MoveToken(&nextHeaderTokenizer);
					nextHeaderToken.type = GetHeaderToken(&nextHeaderTokenizer);

					if (headerToken.type ==  HTTP_WLINEBREAK)
					{
						//NOTES(): For windows systems we need to check if there's another
						//return if there is then we need to advance one
						switch(nextHeaderToken.type)
						{
							case HTTP_LINEBREAK:
							{
								headerToken = MoveToken(&headerTokenizer);
								break;
							}
						}

					}

					nextHeaderToken = MoveToken(&nextHeaderTokenizer);
					nextHeaderToken.type = GetHeaderToken(&nextHeaderTokenizer);

					if ((nextHeaderToken.type ==  HTTP_LINEBREAK) ||
							(nextHeaderToken.type ==  HTTP_WLINEBREAK))
					{
						found = true;
						char *theRest = MidString(data,headerTokenizer.count+3,Strlen(data) );
						
						result.body = theRest;
						//break;
						//NOTES(): this is for debuging
						//found=true; 
					//	printf("tesint\n");
					} else {

						struct header headerKeyValue =  GetHeader(headerData);
						if (headerKeyValue.key && headerKeyValue.value)
						{
							//printf("key: |%s|\n",headerKeyValue.key);
							//printf("value: |%s|\n",headerKeyValue.value);

							AddToBucket(&result.headers,headerKeyValue.key,(i8*) headerKeyValue.value);
						} else {

							if (headerKeyValue.key)
							{
								Free(headerKeyValue.key);
								headerKeyValue.key=NULL;
							}

							if (headerKeyValue.value)
							{
								Free(headerKeyValue.value);
								headerKeyValue.value=NULL;
							}
						
						}
					}


					Free(headerData);
					headerData=NULL;
				}
				break;
			}
			default:
			{
				char tempChar[5] = {};
				tempChar[0] = headerTokenizer.at[0];

				if (headerData == NULL)
				{
					headerData = S32(tempChar);
				} else {
					temp = headerData;
					headerData = S32Cat(headerData,1,tempChar);

					if (temp)
					{
						Free(temp);
						temp=NULL;
					}

				}
				break;
			}
		}

		headerToken = MoveToken(&headerTokenizer);
		headerToken.type = GetHeaderToken(&headerTokenizer);
	
	} while (headerToken.type != HTTP_NONE && found == false);
	

	return result;
}


http_download_info HttpSkipHeader(void *data,int length)
{
	http_download_info result = {};
	struct tokenizer headerTokenizer = {};
	struct token headerToken = {};
	bool found = false;
	char *headerData = NULL;
	char *temp = NULL;

	headerTokenizer.at =(char*)  data;

	do
	{
		headerToken.type = GetHeaderToken(&headerTokenizer);
		switch(headerToken.type)
		{
			case HTTP_WLINEBREAK:
			case HTTP_LINEBREAK:
			{
				if (headerData)
				{

					struct tokenizer nextHeaderTokenizer = {};
					struct token nextHeaderToken = {};

					nextHeaderTokenizer.at = headerTokenizer.at;

					nextHeaderToken = MoveToken(&nextHeaderTokenizer);
					nextHeaderToken.type = GetHeaderToken(&nextHeaderTokenizer);

					if (headerToken.type ==  HTTP_WLINEBREAK)
					{
						//NOTES(): For windows systems we need to check if there's another
						//return if there is then we need to advance one
						switch(nextHeaderToken.type)
						{
							case HTTP_LINEBREAK:
							{
								headerToken = MoveToken(&headerTokenizer);
								break;
							}
						}

					}

					nextHeaderToken = MoveToken(&nextHeaderTokenizer);
					nextHeaderToken.type = GetHeaderToken(&nextHeaderTokenizer);

					if ((nextHeaderToken.type ==  HTTP_LINEBREAK) ||
							(nextHeaderToken.type ==  HTTP_WLINEBREAK))
					{
						found = true;
						//char *theRest = MidString(data,headerTokenizer.count+3,Strlen(data) );
						
						result.data = (us32*)MemoryRaw(length);
						result.length = length - headerTokenizer.count+3;
						memcpy(result.data, ( ((ui8*) data + headerTokenizer.count+3)),length - headerTokenizer.count+3);

						//break;
						//NOTES(): this is for debuging
						//found=true; 
					//	printf("tesint\n");
					} else {

					}


					Free(headerData);
					headerData=NULL;
				}
				break;
			}
			default:
			{
				char tempChar[5] = {};
				tempChar[0] = headerTokenizer.at[0];

				if (headerData == NULL)
				{
					headerData = S32(tempChar);
				} else {
					temp = headerData;
					headerData = S32Cat(headerData,1,tempChar);

					if (temp)
					{
						Free(temp);
						temp=NULL;
					}

				}
				break;
			}
		}

		headerToken = MoveToken(&headerTokenizer);
		headerToken.type = GetHeaderToken(&headerTokenizer);
	
	} while (headerToken.type != HTTP_NONE && found == false);
	

	return result;
}


s32* HttpRequest(char* url, struct bucket_list* headers, Method method, char* postData, int targetPort)
{
	s32 *result=NULL;
	struct marray *findHttp = NULL;

	findHttp = FindString("https://", url);

	if (findHttp->count == 1)
	{
		int *httpPos = NULL;

		httpPos = GetFromMArray(findHttp,0);

		if (*httpPos == 0)
		{
			result = HttpsRequest(url,headers,method,postData, true, targetPort);
			printf("http was found\n");
		}
	} else {
		int *httpPos = NULL;

		FreeMArray(findHttp);

		if (findHttp)
		{
			Free(findHttp);
			findHttp = NULL;
		}

		findHttp = FindString("http://", url);

		if (findHttp->count == 1)
		{
			httpPos = GetFromMArray(findHttp,0);
			if (*httpPos == 0)
			{
				result = HttpsRequest(url,headers,method,postData, false, targetPort);
			} 
		}
	}

	FreeMArray(findHttp);

	if (findHttp)
	{
		Free(findHttp);
		findHttp=NULL;
	}

	return result;
}

void *FreeHttpHeader(struct http_header *header)
{
	FreeAllBuckets(&header->headers);

	if (header->body)
	{
		Free(header->body);
		header->body = NULL;
	}

	return NULL;
}

