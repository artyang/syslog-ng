#include "rename.h"

static gboolean
__check_parameters(gchar *persist_filename, int argc)
{
  if (!old_key || !new_key)
    {
      fprintf(stderr,"Old and new key are required options\n");
      return FALSE;
    }

  if (argc < 2)
    {
      fprintf(stderr, "Persist file is a required parameter\n");
      return FALSE;
    }

  if (!g_file_test(persist_filename, G_FILE_TEST_IS_REGULAR | G_FILE_TEST_EXISTS))
    {
      fprintf(stderr, "Persist file doesn't exist; file = %s\n", persist_filename);
      return FALSE;
    }

  return TRUE;
}

static inline gint
__rename_entry(PersistTool *self, gchar *value)
{
  if (!persist_state_rename_entry(self->state, old_key, new_key))
    {
      return 1;
    }

  persist_state_alloc_string(self->state, new_key, value, strlen(value));
  
  if (!persist_state_commit(self->state))
    {
      return 1;
    }

  return 0;
}

gint
rename_main(int argc, char *argv[])
{
  PersistTool *self;
  gchar *value;
  gint result = 1;
  gchar *persist_filename = argv[1];

  if (!__check_parameters(persist_filename, argc))
    return result;

  self = persist_tool_new(persist_filename, persist_mode_edit);
  if (!self)
    return result;

  value = persist_state_lookup_string(self->state, old_key, NULL, NULL);
  if (value)
    {
      result = __rename_entry(self, value);
    }
  else
    {
      fprintf(stderr, "Key:'%s' does not exist\n", old_key);
    }

  persist_tool_free(self);
  return result;
}
