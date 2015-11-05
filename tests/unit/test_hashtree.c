#include "hashtree.h"
#include "testutils.h"
#include "utils.h"

#define HTREE_TESTCASE(testfunc, ...) { testcase_begin("%s(%s)", #testfunc, #__VA_ARGS__); testfunc(__VA_ARGS__); testcase_end(); }

void
test_htree_new(void)
{
  HNode *root = htree_new("com.google.gmail", ".");

  assert_true(root != NULL, "htree_new returned NULL");
  assert_guint(htree_get_size(root, "com"), 0, "htree_get_size failed");
  assert_gint(hnode_get_size(root), 3, "hnode_size failed");
  assert_gint(htree_get_size(root, "google"), 2, "htree_get_size failed");
  assert_gint(htree_get_size(root, "google.gmail"), 1, "htree_get_size failed");

  htree_free(root);
}

void
test_htree_insert(void)
{
  HNode *root = htree_new("com", ".");
  htree_insert(root, "google.gmail");
  HNode *android = htree_insert(root, "google.android");

  assert_true(android != NULL, "htree_insert failed");
  assert_string(hnode_get_key(android), "android", "hnode_insert returned an invalid node");

  htree_free(root);
}

void
test_htree_get_top_root(void)
{
  HNode *root = htree_new("com.google.gmail", ".");
  htree_insert(root, "google.drive.docs");
  htree_insert(root, "google.drive.pres");
  htree_insert(root, "google.drive.xls");
  htree_insert(root, "google.drive.docs.odt");
  htree_insert(root, "google.drive.docs.doc");

  HNode *txt = htree_insert(root, "google.drive.docs.txt");
  assert_true(txt != root, "htree_insert returned a bad node");
  HNode *txt_root = htree_get_top_root(txt);
  assert_true(txt_root == root, "htree_get_top_root returned something but not the top root");

  htree_free(root);
}

void
test_htree_get_set_value(void)
{
  HNode *root = htree_new("com.google.gmail", ".");
  HNode *google = htree_find(root, "google");
  htree_insert(google, "drive.docs");
  htree_set_value(google, "drive", "DRIVE");

  assert_string((gchar *)htree_get_value(root, "google.drive"), "DRIVE", "htree_get_value returned a bad value");

  htree_free(root);
}

void
test_htree_find(void)
{
  HNode *root = htree_new("com.google.gmail", ".");

  HNode *google = htree_find(root, "google");

  assert_true(google != NULL, "find_tree didn't find the node");
  assert_string(hnode_get_key(google), "google", "htree_find found a different node");

  HNode *drive = htree_insert(google, "drive");

  assert_string(hnode_get_key(hnode_get_parent(drive)), "google", "parent of the node is not match to the required");


  assert_true(htree_find(root, "com") == NULL, "found the node itself as child-node");
  assert_true(htree_find(root, "xxx") == NULL, "found a non-existent path as a child");

  htree_free(root);
}

void
test_htree_find_longest_match(void)
{
  HNode *root = htree_new("com.google.gmail", ".");
  gchar *remaining = NULL;
  HNode *google = htree_find(root, "google");

  HNode *node = htree_find_longest_match(root, "google.drive", &remaining);

  assert_true(node == google, "not found longest match");
  assert_true(remaining != NULL, "invalid longest match");
  assert_string(remaining, "drive", "invalid longest match");
  g_free(remaining);

  remaining = NULL;
  node = htree_find_longest_match(root, "google", &remaining);
  assert_true(node == google, "invalid longest match");
  assert_true(remaining == NULL, "invalid longest_match");

  assert_true(htree_find_longest_match(root, "xxx", &remaining) == NULL, "invalid longest match");
  assert_string(remaining, "xxx", "invalid longest match");
  g_free(remaining);

  assert_true(htree_find_longest_match(root, "xxx.yyy", &remaining) == NULL, "invalid longest match");
  assert_string(remaining, "xxx.yyy", "invalid longest match");
  g_free(remaining);

  remaining = NULL;
  assert_true(htree_find_longest_match(root, "google.drive.docs", &remaining) == google, "invalid longest match");
  assert_string(remaining, "drive.docs", "invalid longest match");
  g_free(remaining);

  remaining = NULL;

  HNode *gmail = htree_find_longest_match(root, "google.gmail", &remaining);
  assert_true(remaining == NULL, "invalid longest match");
  assert_true(gmail != NULL, "invalid longest match");
  assert_string(hnode_get_key(gmail), "gmail", "invalid longest match");

  htree_free(root);
}

static void
_append_keys_to_list(HNode *node, gpointer arg)
{
  GList **list = (GList **) arg;
  *list = g_list_append(*list, g_strdup(hnode_get_key(node)));
}

void
test_htree_foreach(void)
{
  HNode *root = htree_new("com.google.google.gmail", ".");
  GList *list = NULL;

  htree_foreach(root, _append_keys_to_list, &list);

  assert_gint(g_list_length(list), 4, "number of nodes are not matched");
  assert_true(g_list_find_custom(list, "com", (GCompareFunc)strcmp) != NULL, "node not found");
  assert_true(g_list_find_custom(list, "google", (GCompareFunc)strcmp) != NULL, "node not found");
  assert_true(g_list_find_custom(list, "gmail", (GCompareFunc)strcmp) != NULL, "node not found");

  g_list_free_full(list, g_free);

  htree_free(root);
}

void
test_htree_get_height(void)
{
  HNode *root = NULL;

  assert_gint(htree_get_height(root, NULL), 0, "returned a non-zero size for a NULL-tree");

  root = htree_new("com.google.gmail", ".");

  htree_insert(root, "google.drive");
  htree_insert(root, "com.google.android");
  htree_insert(root, "google.drive.docs");

  assert_guint(htree_get_height(root, "google.drive.xls"), 0, "invalid tree height");
  assert_guint(htree_get_height(root, "google.drive"), 1, "invalid tree height");
  assert_guint(htree_get_height(root, "google"), 2, "invalid tree height");

  htree_free(root);
}

void
test_htree_unlink(void)
{
  HNode *root = htree_new("com.google.android", ".");
  htree_insert(root, "oracle.java");

  HNode *oracle = htree_unlink(root, "oracle");

  assert_gint(hnode_get_size(root), 3, "invalid tree size after unlink in the original tree");
  assert_gint(hnode_get_size(oracle), 2, "invalid tree size after unlink in the detacht subtree");

  htree_free(root);
  htree_free(oracle);
}

void
test_htree_join(void)
{
  HNode *root = htree_new("com.google.gmail", ".");
  HNode *subtree = htree_new("oracle.java.swing", ".");
  HNode *java_in_subtree = htree_find(subtree, "java");

  HNode *joined = htree_join(root, subtree);

  assert_true(joined == root, "htree_join create a wrong tree");
  assert_gint(hnode_get_size(root), 6, "invalid tree size after join");
  assert_true(htree_get_top_root(subtree) == root, "invalid root node after join");

  HNode *java_in_root = htree_find(root, "oracle.java");
  assert_true(java_in_subtree == java_in_root, "the joined tree didn't contain the attached subtree");
  assert_true(htree_get_top_root(java_in_root) == root, "invalid root node for a node located in the attached subtree");

  assert_true(htree_join(root, NULL) == root, "htree_join failed when tried to attach a NULL-tree");
  assert_gint(hnode_get_size(root), 6, "size of the original tree changed after attaching a NULL-tree");
  assert_true(htree_join(NULL, root) == NULL, "attaching a tree to NULL failed");

  htree_free(root);
}

void
test_htree_fqdn(void)
{
  const gchar *path_got;

  HNode *root = htree_new("com.google.gmail", ".");
  path_got = hnode_get_fqdn (root);
  assert_string(path_got, "", "invalid path");
  g_free(path_got);

  gchar *remaining = NULL;
  HNode *leaf = htree_find_longest_match(root, "google.gmail", &remaining);
  assert_true(leaf != NULL, "htree_find_longest_match() failed");
  path_got = hnode_get_fqdn (leaf);
  assert_string(path_got, "google.gmail", "invalid path");
  g_free(path_got);

  htree_free(root);
}

int main(int argc, char **argv)
{
  HTREE_TESTCASE(test_htree_new);
  HTREE_TESTCASE(test_htree_insert);
  HTREE_TESTCASE(test_htree_get_top_root);
  HTREE_TESTCASE(test_htree_get_set_value);
  HTREE_TESTCASE(test_htree_find);
  HTREE_TESTCASE(test_htree_find_longest_match);
  HTREE_TESTCASE(test_htree_foreach);
  HTREE_TESTCASE(test_htree_get_height);
  HTREE_TESTCASE(test_htree_unlink);
  HTREE_TESTCASE(test_htree_join);
  HTREE_TESTCASE(test_htree_fqdn);

  return 0;
}
