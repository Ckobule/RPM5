#include "system.h"

#include <mongoc.h>

#include "TestSuite.h"

#include "debug.h"


static void
test_extract_subject (void)
{
   char *subject;

   subject = _mongoc_ssl_extract_subject (BINARY_DIR"/../certificates/client.pem");
   ASSERT (0 == strcmp (subject, "CN=client,OU=kerneluser,O=10Gen,L=New York City,ST=New York,C=US"));
   bson_free (subject);
}


void
test_x509_install (TestSuite *suite)
{
   TestSuite_Add (suite, "/SSL/extract_subject", test_extract_subject);
}