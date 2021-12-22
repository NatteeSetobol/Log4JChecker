#include "ssl.h"

static int VerifyCerf(X509_STORE_CTX *ctx, void *arg)
{

	return 1;
}
/*
bool InitSSL(memory_arena *arena, struct sslinfo *sslInfo, s32 *url)
{
	bool result = true;

	OpenSSL_add_all_algorithms();
	ERR_load_BIO_strings();
	ERR_load_crypto_strings();
	SSL_load_error_strings();

	sslInfo->certBio = BIO_new(BIO_s_file());
	sslInfo->outBio = BIO_new_fp(stdout,BIO_NOCLOSE);

	if (SSL_library_init() < 0)
	{
		printf("can not init ssl\n");
		result =  false;
	} else {

		sslInfo->method = (SSL_METHOD*) SSLv23_client_method();
		if ( (sslInfo->ctx = SSL_CTX_new(sslInfo->method)) == NULL)
		{
			printf("Unable to create a new SSL context structure.\n");
		}

		char *socketError = NULL;
		SSL_CTX_set_options(sslInfo->ctx,SSL_OP_NO_SSLv2);
		SSL_CTX_load_verify_locations (sslInfo->ctx, "certificate-authorities.pem", NULL);
		SSL_CTX_set_verify (sslInfo->ctx, SSL_VERIFY_PEER, NULL);
		SSL_CTX_set_cert_verify_callback(sslInfo->ctx,VerifyCerf,NULL );
		sslInfo->ssl = SSL_new(sslInfo->ctx);

		sslInfo->realURL = PushString(arena,url, Strlen(url));
		sslInfo->host = StripToHostName(arena,url);
		sslInfo->server  = CreateSocket(sslInfo->host, 443);
		SSL_set_fd(sslInfo->ssl,sslInfo->server.socket);

		//NOTES(): OpenSSL bug fix?.
		SSL_set_tlsext_host_name(sslInfo->ssl, sslInfo->host);

		if (SSL_connect(sslInfo->ssl) != 1)
		{
			printf("can not connect to SSL port\n");
		} else {
			sslInfo->cert = SSL_get_peer_certificate(sslInfo->ssl);

			if (sslInfo->cert == NULL)
			{
				printf("Can not get certification from peer\n");
			} else {
			}
		}
	}

	return result;
}
*/




bool InitSSL(struct sslinfo *sslInfo, s32 *url)
{
	bool result = true;

	OpenSSL_add_all_algorithms();
	ERR_load_BIO_strings();
	ERR_load_crypto_strings();
	SSL_load_error_strings();

	sslInfo->certBio = BIO_new(BIO_s_file());
	sslInfo->outBio = BIO_new_fp(stdout,BIO_NOCLOSE);

	if (SSL_library_init() < 0)
	{
		printf("can not init ssl\n");
		result =  false;
	} else {

		sslInfo->method = (SSL_METHOD*) SSLv23_client_method();
		if ( (sslInfo->ctx = SSL_CTX_new(sslInfo->method)) == NULL)
		{
			printf("Unable to create a new SSL context structure.\n");
		}

		char *socketError = NULL;
		SSL_CTX_set_options(sslInfo->ctx,SSL_OP_NO_SSLv2);
		SSL_CTX_load_verify_locations (sslInfo->ctx, "certificate-authorities.pem", NULL);
		SSL_CTX_set_verify (sslInfo->ctx, SSL_VERIFY_PEER, NULL);
		SSL_CTX_set_cert_verify_callback(sslInfo->ctx,VerifyCerf,NULL );
		sslInfo->ssl = SSL_new(sslInfo->ctx);

		sslInfo->realURL = S32(url);
		sslInfo->host = StripToHostName(url);
		sslInfo->server  = CreateSocket(sslInfo->host, 443);
		SSL_set_fd(sslInfo->ssl,sslInfo->server.socket);

		//NOTES(): OpenSSL bug fix?.
		SSL_set_tlsext_host_name(sslInfo->ssl, sslInfo->host);

		if (SSL_connect(sslInfo->ssl) != 1)
		{
			printf("can not connect to SSL port\n");
		} else {
			sslInfo->cert = SSL_get_peer_certificate(sslInfo->ssl);

			if (sslInfo->cert == NULL)
			{
				printf("Can not get certification from peer\n");
			} else {
			}
		}
	}

	return result;
}

void FreeSSL(struct sslinfo *sslInfo)
{
	BIO_free(sslInfo->certBio);
	BIO_free(sslInfo->outBio);
	X509_free(sslInfo->cert);
	SSL_free(sslInfo->ssl);
	SSL_CTX_free(sslInfo->ctx);

	if (sslInfo->realURL)
	{
		Free(sslInfo->realURL);
		sslInfo->realURL=NULL;
	}
	if (sslInfo->host)
	{
		Free(sslInfo->host );
		sslInfo->host = NULL;
	}
}

void FreeSSL2(struct sslinfo *sslInfo)
{
	BIO_free(sslInfo->certBio);
	BIO_free(sslInfo->outBio);
	X509_free(sslInfo->cert);
	SSL_free(sslInfo->ssl);
	SSL_CTX_free(sslInfo->ctx);

}
