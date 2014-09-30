#include "system.h"

#include <bcon.h>
#include <mongoc.h>

#include "TestSuite.h"

#include "test-libmongoc.h"
#include "mongoc-tests.h"

#include "debug.h"

static char *gTestUri;


static mongoc_database_t *
get_test_database (mongoc_client_t *client)
{
   return mongoc_client_get_database (client, "test");
}


static mongoc_collection_t *
get_test_collection (mongoc_client_t *client,
                     const char      *prefix)
{
   mongoc_collection_t *ret;
   char *str;

   str = gen_collection_name (prefix);
   ret = mongoc_client_get_collection (client, "test", str);
   bson_free (str);

   return ret;
}


static void
test_insert (void)
{
   mongoc_database_t *database;
   mongoc_collection_t *collection;
   mongoc_client_t *client;
   bson_context_t *context;
   bson_error_t error;
   bool r;
   bson_oid_t oid;
   unsigned i;
   bson_t b;


   client = mongoc_client_new(gTestUri);
   ASSERT (client);

   database = get_test_database (client);
   ASSERT (database);

   collection = get_test_collection (client, "test_insert");
   ASSERT (collection);

   mongoc_collection_drop(collection, &error);

   context = bson_context_new(BSON_CONTEXT_NONE);
   ASSERT (context);

   for (i = 0; i < 10; i++) {
      bson_init(&b);
      bson_oid_init(&oid, context);
      bson_append_oid(&b, "_id", 3, &oid);
      bson_append_utf8(&b, "hello", 5, "/world", 5);
      r = mongoc_collection_insert(collection, MONGOC_INSERT_NONE, &b, NULL,
                                   &error);
      if (!r) {
         MONGOC_WARNING("%s\n", error.message);
      }
      ASSERT (r);
      bson_destroy(&b);
   }

   bson_init (&b);
   BSON_APPEND_INT32 (&b, "$hello", 1);
   r = mongoc_collection_insert (collection, MONGOC_INSERT_NONE, &b, NULL,
                                 &error);
   ASSERT (!r);
   ASSERT (error.domain == MONGOC_ERROR_BSON);
   ASSERT (error.code == MONGOC_ERROR_BSON_INVALID);
   bson_destroy (&b);

   r = mongoc_collection_drop (collection, &error);
   ASSERT (r);

   mongoc_collection_destroy(collection);
   mongoc_database_destroy(database);
   bson_context_destroy(context);
   mongoc_client_destroy(client);
}


static void
test_insert_bulk (void)
{
   mongoc_collection_t *collection;
   mongoc_database_t *database;
   mongoc_client_t *client;
   bson_context_t *context;
   bson_error_t error;
   bool r;
   bson_oid_t oid;
   unsigned i;
   bson_t q;
   bson_t b[10];
   bson_t *bptr[10];
   int64_t count;

   client = mongoc_client_new(gTestUri);
   ASSERT (client);

   database = get_test_database (client);
   ASSERT (database);

   collection = get_test_collection (client, "test_insert_bulk");
   ASSERT (collection);

   mongoc_collection_drop(collection, &error);

   context = bson_context_new(BSON_CONTEXT_NONE);
   ASSERT (context);

   bson_init(&q);
   bson_append_int32(&q, "n", -1, 0);

   for (i = 0; i < 10; i++) {
      bson_init(&b[i]);
      bson_oid_init(&oid, context);
      bson_append_oid(&b[i], "_id", -1, &oid);
      bson_append_int32(&b[i], "n", -1, i % 2);
      bptr[i] = &b[i];
   }

   r = mongoc_collection_insert_bulk (collection, MONGOC_INSERT_NONE,
                                     (const bson_t **)bptr, 10, NULL, &error);

   if (!r) {
      MONGOC_WARNING("%s\n", error.message);
   }
   ASSERT (r);

   count = mongoc_collection_count (collection, MONGOC_QUERY_NONE, &q, 0, 0, NULL, &error);
   ASSERT (count == 5);

   for (i = 8; i < 10; i++) {
      bson_destroy(&b[i]);
      bson_init(&b[i]);
      bson_oid_init(&oid, context);
      bson_append_oid(&b[i], "_id", -1, &oid);
      bson_append_int32(&b[i], "n", -1, i % 2);
      bptr[i] = &b[i];
   }

   r = mongoc_collection_insert_bulk (collection, MONGOC_INSERT_NONE,
                                     (const bson_t **)bptr, 10, NULL, &error);

   ASSERT (!r);
   ASSERT (error.code == 11000);

   count = mongoc_collection_count (collection, MONGOC_QUERY_NONE, &q, 0, 0, NULL, &error);
   ASSERT (count == 5);

   r = mongoc_collection_insert_bulk (collection, MONGOC_INSERT_CONTINUE_ON_ERROR,
                                     (const bson_t **)bptr, 10, NULL, &error);
   ASSERT (!r);
   ASSERT (error.code == 11000);

   count = mongoc_collection_count (collection, MONGOC_QUERY_NONE, &q, 0, 0, NULL, &error);
   ASSERT (count == 6);

   bson_destroy(&q);
   for (i = 0; i < 10; i++) {
      bson_destroy(&b[i]);
   }

   r = mongoc_collection_drop (collection, &error);
   ASSERT (r);

   mongoc_collection_destroy(collection);
   mongoc_database_destroy(database);
   bson_context_destroy(context);
   mongoc_client_destroy(client);
}


static void
test_save (void)
{
   mongoc_collection_t *collection;
   mongoc_database_t *database;
   mongoc_client_t *client;
   bson_context_t *context;
   bson_error_t error;
   bool r;
   bson_oid_t oid;
   unsigned i;
   bson_t b;

   client = mongoc_client_new(gTestUri);
   ASSERT (client);

   database = get_test_database (client);
   ASSERT (database);

   collection = get_test_collection (client, "test_save");
   ASSERT (collection);

   mongoc_collection_drop (collection, &error);

   context = bson_context_new(BSON_CONTEXT_NONE);
   ASSERT (context);

   for (i = 0; i < 10; i++) {
      bson_init(&b);
      bson_oid_init(&oid, context);
      bson_append_oid(&b, "_id", 3, &oid);
      bson_append_utf8(&b, "hello", 5, "/world", 5);
      r = mongoc_collection_save(collection, &b, NULL, &error);
      if (!r) {
         MONGOC_WARNING("%s\n", error.message);
      }
      ASSERT (r);
      bson_destroy(&b);
   }

   bson_destroy (&b);

   r = mongoc_collection_drop (collection, &error);
   ASSERT (r);

   mongoc_collection_destroy(collection);
   mongoc_database_destroy(database);
   bson_context_destroy(context);
   mongoc_client_destroy(client);
}


static void
test_regex (void)
{
   mongoc_collection_t *collection;
   mongoc_database_t *database;
   mongoc_write_concern_t *wr;
   mongoc_client_t *client;
   bson_error_t error = { 0 };
   int64_t count;
   bson_t q = BSON_INITIALIZER;
   bson_t *doc;
   bool r;

   client = mongoc_client_new (gTestUri);
   ASSERT (client);

   database = get_test_database (client);
   ASSERT (database);

   collection = get_test_collection (client, "test_regex");
   ASSERT (collection);

   wr = mongoc_write_concern_new ();
   mongoc_write_concern_set_journal (wr, true);

   doc = BCON_NEW ("hello", "/world");
   r = mongoc_collection_insert (collection, MONGOC_INSERT_NONE, doc, wr, &error);
   ASSERT (r);

   BSON_APPEND_REGEX (&q, "hello", "^/wo", "i");

   count = mongoc_collection_count (collection,
                                    MONGOC_QUERY_NONE,
                                    &q,
                                    0,
                                    0,
                                    NULL,
                                    &error);

   ASSERT (count > 0);
   ASSERT (!error.domain);

   r = mongoc_collection_drop (collection, &error);
   ASSERT (r);

   mongoc_write_concern_destroy (wr);

   bson_destroy (&q);
   bson_destroy (doc);
   mongoc_collection_destroy (collection);
   mongoc_database_destroy(database);
   mongoc_client_destroy (client);
}


static void
test_update (void)
{
   mongoc_collection_t *collection;
   mongoc_database_t *database;
   mongoc_client_t *client;
   bson_context_t *context;
   bson_error_t error;
   bool r;
   bson_oid_t oid;
   unsigned i;
   bson_t b;
   bson_t q;
   bson_t u;
   bson_t set;

   client = mongoc_client_new(gTestUri);
   ASSERT (client);

   database = get_test_database (client);
   ASSERT (database);

   collection = get_test_collection (client, "test_update");
   ASSERT (collection);

   context = bson_context_new(BSON_CONTEXT_NONE);
   ASSERT (context);

   for (i = 0; i < 10; i++) {
      bson_init(&b);
      bson_oid_init(&oid, context);
      bson_append_oid(&b, "_id", 3, &oid);
      bson_append_utf8(&b, "utf8", 4, "utf8 string", 11);
      bson_append_int32(&b, "int32", 5, 1234);
      bson_append_int64(&b, "int64", 5, 12345678);
      bson_append_bool(&b, "bool", 4, 1);

      r = mongoc_collection_insert(collection, MONGOC_INSERT_NONE, &b, NULL, &error);
      if (!r) {
         MONGOC_WARNING("%s\n", error.message);
      }
      ASSERT (r);

      bson_init(&q);
      bson_append_oid(&q, "_id", 3, &oid);

      bson_init(&u);
      bson_append_document_begin(&u, "$set", 4, &set);
      bson_append_utf8(&set, "utf8", 4, "updated", 7);
      bson_append_document_end(&u, &set);

      r = mongoc_collection_update(collection, MONGOC_UPDATE_NONE, &q, &u, NULL, &error);
      if (!r) {
         MONGOC_WARNING("%s\n", error.message);
      }
      ASSERT (r);

      bson_destroy(&b);
      bson_destroy(&q);
      bson_destroy(&u);
   }

   bson_init(&q);
   bson_init(&u);
   BSON_APPEND_INT32 (&u, "abcd", 1);
   BSON_APPEND_INT32 (&u, "$hi", 1);
   r = mongoc_collection_update(collection, MONGOC_UPDATE_NONE, &q, &u, NULL, &error);
   ASSERT (!r);
   ASSERT (error.domain == MONGOC_ERROR_BSON);
   ASSERT (error.code == MONGOC_ERROR_BSON_INVALID);
   bson_destroy(&q);
   bson_destroy(&u);

   bson_init(&q);
   bson_init(&u);
   BSON_APPEND_INT32 (&u, "a.b.c.d", 1);
   r = mongoc_collection_update(collection, MONGOC_UPDATE_NONE, &q, &u, NULL, &error);
   ASSERT (!r);
   ASSERT (error.domain == MONGOC_ERROR_BSON);
   ASSERT (error.code == MONGOC_ERROR_BSON_INVALID);
   bson_destroy(&q);
   bson_destroy(&u);

   r = mongoc_collection_drop (collection, &error);
   ASSERT (r);

   mongoc_collection_destroy(collection);
   mongoc_database_destroy(database);
   bson_context_destroy(context);
   mongoc_client_destroy(client);
}


static void
test_delete (void)
{
   mongoc_collection_t *collection;
   mongoc_database_t *database;
   mongoc_client_t *client;
   bson_context_t *context;
   bson_error_t error;
   bool r;
   bson_oid_t oid;
   bson_t b;
   int i;

   client = mongoc_client_new(gTestUri);
   ASSERT (client);

   database = get_test_database (client);
   ASSERT (database);

   collection = get_test_collection (client, "test_delete");
   ASSERT (collection);

   context = bson_context_new(BSON_CONTEXT_NONE);
   ASSERT (context);

   for (i = 0; i < 100; i++) {
      bson_init(&b);
      bson_oid_init(&oid, context);
      bson_append_oid(&b, "_id", 3, &oid);
      bson_append_utf8(&b, "hello", 5, "world", 5);
      r = mongoc_collection_insert(collection, MONGOC_INSERT_NONE, &b, NULL,
                                   &error);
      if (!r) {
         MONGOC_WARNING("%s\n", error.message);
      }
      ASSERT (r);
      bson_destroy(&b);

      bson_init(&b);
      bson_append_oid(&b, "_id", 3, &oid);
      r = mongoc_collection_delete(collection, MONGOC_DELETE_NONE, &b, NULL,
                                   &error);
      if (!r) {
         MONGOC_WARNING("%s\n", error.message);
      }
      ASSERT (r);
      bson_destroy(&b);
   }

   r = mongoc_collection_drop (collection, &error);
   ASSERT (r);

   mongoc_collection_destroy(collection);
   mongoc_database_destroy(database);
   bson_context_destroy(context);
   mongoc_client_destroy(client);
}

static void
test_index (void)
{
   mongoc_collection_t *collection;
   mongoc_database_t *database;
   mongoc_client_t *client;
   mongoc_index_opt_t opt;
   bson_error_t error;
   bool r;
   bson_t keys;

   mongoc_index_opt_init(&opt);

   client = mongoc_client_new(gTestUri);
   ASSERT (client);

   database = get_test_database (client);
   ASSERT (database);

   collection = get_test_collection (client, "test_index");
   ASSERT (collection);

   bson_init(&keys);
   bson_append_int32(&keys, "hello", -1, 1);
   r = mongoc_collection_ensure_index(collection, &keys, &opt, &error);
   ASSERT (r);

   r = mongoc_collection_ensure_index(collection, &keys, &opt, &error);
   ASSERT (r);

   r = mongoc_collection_drop_index(collection, "hello_1", &error);
   ASSERT (r);

   bson_destroy(&keys);

   r = mongoc_collection_drop (collection, &error);
   ASSERT (r);

   mongoc_collection_destroy(collection);
   mongoc_database_destroy(database);
   mongoc_client_destroy(client);
}


static void
test_count (void)
{
   mongoc_collection_t *collection;
   mongoc_client_t *client;
   bson_error_t error;
   int64_t count;
   bson_t b;

   client = mongoc_client_new(gTestUri);
   ASSERT (client);

   collection = mongoc_client_get_collection(client, "test", "test");
   ASSERT (collection);

   bson_init(&b);
   count = mongoc_collection_count(collection, MONGOC_QUERY_NONE, &b,
                                   0, 0, NULL, &error);
   bson_destroy(&b);

   if (count == -1) {
      MONGOC_WARNING("%s\n", error.message);
   }
   ASSERT (count != -1);

   mongoc_collection_destroy(collection);
   mongoc_client_destroy(client);
}


static void
test_drop (void)
{
   mongoc_collection_t *collection;
   mongoc_database_t *database;
   mongoc_client_t *client;
   bson_error_t error;
   bson_t *doc;
   bool r;

   client = mongoc_client_new(gTestUri);
   ASSERT (client);

   database = get_test_database (client);
   ASSERT (database);

   collection = get_test_collection (client, "test_drop");
   ASSERT (collection);

   doc = BCON_NEW("hello", "world");
   r = mongoc_collection_insert(collection, MONGOC_INSERT_NONE, doc, NULL, &error);
   bson_destroy (doc);
   ASSERT (r);

   r = mongoc_collection_drop(collection, &error);
   ASSERT (r == true);

   r = mongoc_collection_drop(collection, &error);
   ASSERT (r == false);

   mongoc_collection_destroy(collection);
   mongoc_database_destroy(database);
   mongoc_client_destroy(client);
}


static void
test_aggregate (void)
{
   mongoc_collection_t *collection;
   mongoc_database_t *database;
   mongoc_client_t *client;
   mongoc_cursor_t *cursor;
   const bson_t *doc;
   bson_error_t error;
   bool r;
   bson_t b;
   bson_t match;
   bson_t pipeline;
   bson_iter_t iter;

   bson_init(&b);
   bson_append_utf8(&b, "hello", -1, "world", -1);

   bson_init(&match);
   bson_append_document(&match, "$match", -1, &b);

   bson_init(&pipeline);
   bson_append_document(&pipeline, "0", -1, &match);

   client = mongoc_client_new(gTestUri);
   ASSERT (client);

   database = get_test_database (client);
   ASSERT (database);

   collection = get_test_collection (client, "test_aggregate");
   ASSERT (collection);

   mongoc_collection_drop(collection, &error);

   r = mongoc_collection_insert(collection, MONGOC_INSERT_NONE, &b, NULL, &error);
   ASSERT (r);

   cursor = mongoc_collection_aggregate(collection, MONGOC_QUERY_NONE, &pipeline, NULL);
   ASSERT (cursor);

   /*
    * This can fail if we are connecting to a pre-2.5.x MongoDB instance.
    */
   r = mongoc_cursor_next(cursor, &doc);
   if (mongoc_cursor_error(cursor, &error)) {
      MONGOC_WARNING("%s", error.message);
   }

   ASSERT (r);
   ASSERT (doc);

   ASSERT (bson_iter_init_find (&iter, doc, "hello") &&
           BSON_ITER_HOLDS_UTF8 (&iter));

   r = mongoc_cursor_next(cursor, &doc);
   if (mongoc_cursor_error(cursor, &error)) {
      MONGOC_WARNING("%s", error.message);
   }
   ASSERT (!r);
   ASSERT (!doc);

   r = mongoc_collection_drop(collection, &error);
   ASSERT (r);

   mongoc_cursor_destroy(cursor);
   mongoc_collection_destroy(collection);
   mongoc_database_destroy(database);
   mongoc_client_destroy(client);
   bson_destroy(&b);
   bson_destroy(&pipeline);
   bson_destroy(&match);
}


static void
cleanup_globals (void)
{
   bson_free (gTestUri);
}


void
test_collection_install (TestSuite *suite)
{
   gTestUri = bson_strdup_printf("mongodb://%s/", MONGOC_TEST_HOST);

   TestSuite_Add (suite, "/Collection/insert_bulk", test_insert_bulk);
   TestSuite_Add (suite, "/Collection/insert", test_insert);
   TestSuite_Add (suite, "/Collection/save", test_save);
   TestSuite_Add (suite, "/Collection/index", test_index);
   TestSuite_Add (suite, "/Collection/regex", test_regex);
   TestSuite_Add (suite, "/Collection/update", test_update);
   TestSuite_Add (suite, "/Collection/delete", test_delete);
   TestSuite_Add (suite, "/Collection/count", test_count);
   TestSuite_Add (suite, "/Collection/drop", test_drop);
   TestSuite_Add (suite, "/Collection/aggregate", test_aggregate);

   atexit (cleanup_globals);
}
