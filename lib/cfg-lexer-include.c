/*
 * Copyright (c) 2002-2016 Balabit
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As an additional exemption you are allowed to compile & link against the
 * OpenSSL libraries as published by the OpenSSL project. See the file
 * COPYING for details.
 *
 */

#include "cfg-lexer.h"
#include "cfg-lex.h"
#include "messages.h"
#include "misc.h"

#include <sys/stat.h>

#if HAVE_GLOB_H
#include <glob.h>
#endif

#ifdef _WIN32
#include <windows.h>
#endif

static gboolean
cfg_lexer_include_file_simple(CfgLexer *self, const gchar *filename)
{
  CfgIncludeLevel *level;
  struct stat st;

  if (stat(filename, &st) < 0)
    {
      return FALSE;
    }

  self->include_depth++;
  level = &self->include_stack[self->include_depth];
  level->include_type = CFGI_FILE;
  if (S_ISDIR(st.st_mode))
    {
      GDir *dir;
      GError *error = NULL;
      const gchar *entry;

      dir = g_dir_open(filename, 0, &error);
      if (!dir)
        {
          msg_error("Error opening directory for reading",
                evt_tag_str("filename", filename),
                evt_tag_str("error", error->message),
                NULL);
          goto drop_level;
        }
      while ((entry = g_dir_read_name(dir)))
        {
          const gchar *p;
          if (entry[0] == '.')
            {
              msg_debug("Skipping include file, it cannot begin with .",
                        evt_tag_str("filename", entry),
                        NULL);
              continue;
            }
          for (p = entry; *p; p++)
            {
              if (!((*p >= 'a' && *p <= 'z') ||
                   (*p >= 'A' && *p <= 'Z') ||
                   (*p >= '0' && *p <= '9') ||
                   (*p == '_') || (*p == '-') || (*p == '.')))
                {
                  msg_debug("Skipping include file, does not match pattern [\\-_a-zA-Z0-9]+",
                            evt_tag_str("filename", entry),
                            NULL);
                  p = NULL;
                  break;
                }
            }
          if (p)
            {
              gchar *full_filename = g_build_filename(filename, entry, NULL);
              if (stat(full_filename, &st) < 0 || S_ISDIR(st.st_mode))
                {
                  msg_debug("Skipping include file as it is a directory",
                            evt_tag_str("filename", entry),
                            NULL);
                  g_free(full_filename);
                  continue;
                }
              level->file.files = g_slist_insert_sorted(level->file.files, full_filename, (GCompareFunc) strcmp);
              msg_debug("Adding include file",
                        evt_tag_str("filename", entry),
                        NULL);
            }
        }
      g_dir_close(dir);
      if (!level->file.files)
        {
          /* no include files in the specified directory */
          msg_debug("No files in this include directory",
                    evt_tag_str("dir", filename),
                    NULL);
          self->include_depth--;
          return TRUE;
        }
    }
  else
    {
      g_assert(level->file.files == NULL);
      level->file.files = g_slist_prepend(level->file.files, g_strdup(filename));
    }
  return cfg_lexer_start_next_include(self);
 drop_level:
  g_slist_foreach(level->file.files, (GFunc) g_free, NULL);
  g_slist_free(level->file.files);
  level->file.files = NULL;

  return FALSE;
}

int
_glob_pattern_p(const char *pattern)
{
  register const char *p;
  int open = 0;

  for (p = pattern; *p != '\0'; ++p)
    switch (*p)
      {
      case '?':
      case '*':
        return 1;

      case '\\':
        if (p[1] != '\0')
          ++p;
        break;

      case '[':
        open = 1;
        break;

      case ']':
        if (open)
          return 1;
        break;
      }

  return 0;
}

static gboolean
cfg_lexer_include_file_add(CfgLexer *self, const gchar *fn)
{
  CfgIncludeLevel *level;

  level = &self->include_stack[self->include_depth];
  level->include_type = CFGI_FILE;

  level->file.files = g_slist_insert_sorted(level->file.files,
                                            strdup(fn),
                                            (GCompareFunc) strcmp);

  msg_debug("Adding include file",
            evt_tag_str("filename", fn),
            NULL);

  return TRUE;
}

#if HAVE_GLOB_H

static int
_cfg_lexer_glob_err(const char *p, gint e)
{
  if (e != ENOENT)
    {
      msg_debug ("Error processing path for inclusion",
                 evt_tag_str("path", p),
                 evt_tag_errno("errno", e),
                 NULL);
      return -1;
    }
  return 0;
}

#ifndef GLOB_NOMAGIC
#define GLOB_NOMAGIC 0
#else
#define HAVE_GLOB_NOMAGIC 1
#endif

static gboolean
cfg_lexer_include_file_glob_at(CfgLexer *self, const gchar *pattern)
{
  glob_t globbuf;
  size_t i;
  int r;

  r = glob(pattern, GLOB_NOMAGIC, _cfg_lexer_glob_err, &globbuf);

  if (r != 0)
    {
      globfree(&globbuf);
      if (r == GLOB_NOMATCH)
        {
#ifndef HAVE_GLOB_NOMAGIC
          if (!_glob_pattern_p (pattern))
            {
              self->include_depth++;
              return cfg_lexer_include_file_add(self, pattern);
            }
#endif
          return FALSE;
        }
      return TRUE;
    }

  self->include_depth++;
  for (i = 0; i < globbuf.gl_pathc; i++)
    {
      cfg_lexer_include_file_add(self, globbuf.gl_pathv[i]);
    }

  globfree(&globbuf);

  return TRUE;
}

#elif defined(_WIN32)

static gboolean
cfg_lexer_include_file_glob_at(CfgLexer *self, const gchar *pattern)
{
  WIN32_FIND_DATA find_data;
  HANDLE find_handle = FindFirstFile(pattern, &find_data);

  if (find_handle == INVALID_HANDLE_VALUE)
    {
      if (!_glob_pattern_p(pattern))
        {
          self->include_depth++;
          return cfg_lexer_include_file_add(self, pattern);
        }

      return FALSE;
    }

  gchar *dirname = g_path_get_dirname(pattern);
  self->include_depth++;

  do
    {
      gchar *full_path = g_build_path(G_DIR_SEPARATOR_S, dirname, find_data.cFileName, NULL);
      cfg_lexer_include_file_add(self, full_path);
      g_free(full_path);
    }
  while (FindNextFile(find_handle, &find_data));

  FindClose(find_handle);
  g_free(dirname);
  return TRUE;
}

#endif


#if HAVE_GLOB_H || defined(_WIN32)

static gboolean
cfg_lexer_include_file_glob(CfgLexer *self, const gchar *filename_)
{
  const gchar *path = cfg_args_get(self->globals, "include-path");
  gboolean process = FALSE;

  if (g_path_is_absolute(filename_) || !path)
    process = cfg_lexer_include_file_glob_at(self, filename_);
  else
    {
      gchar **dirs;
      gchar *cf;
      gint i = 0;

      dirs = g_strsplit(path, G_SEARCHPATH_SEPARATOR_S, 0);
      while (dirs && dirs[i])
        {
          cf = g_build_filename(dirs[i], filename_, NULL);
          process |= cfg_lexer_include_file_glob_at(self, cf);
          g_free(cf);
          i++;
        }
      g_strfreev(dirs);
    }
  if (process)
    return cfg_lexer_start_next_include(self);
  else
    return TRUE;
}

#endif

gboolean
cfg_lexer_start_next_include(CfgLexer *self)
{
  CfgIncludeLevel *level = &self->include_stack[self->include_depth];
  gchar *filename;
  gboolean buffer_processed = FALSE;

  if (self->include_depth == 0)
    {
      return FALSE;
    }

  if (level->yybuf)
    {
      msg_debug("Finishing include",
                evt_tag_str((level->include_type == CFGI_FILE ? "filename" : "content"), level->name),
                evt_tag_int("depth", self->include_depth),
                NULL);
      buffer_processed = TRUE;
    }

  /* reset the include state, should also handle initial invocations, in which case everything is NULL */
  if (level->yybuf)
    _cfg_lexer__delete_buffer(level->yybuf, self->state);

  if (level->include_type == CFGI_FILE)
    {
      if (level->file.include_file)
        {
          fclose(level->file.include_file);
        }
    }

  if ((level->include_type == CFGI_BUFFER && buffer_processed) ||
      (level->include_type == CFGI_FILE && !level->file.files))
    {
      /* we finished with an include statement that included a series of
       * files (e.g.  directory include). */
      g_free(level->name);

      if (level->include_type == CFGI_BUFFER)
        g_free(level->buffer.content);

      memset(level, 0, sizeof(*level));

      self->include_depth--;
      _cfg_lexer__switch_to_buffer(self->include_stack[self->include_depth].yybuf, self->state);

      return TRUE;
    }

  /* now populate "level" with the new include information */
  if (level->include_type == CFGI_BUFFER)
    {
      level->yybuf = _cfg_lexer__scan_buffer(level->buffer.content, level->buffer.content_length, self->state);
    }
  else if (level->include_type == CFGI_FILE)
    {
      FILE *include_file;

      filename = (gchar *) level->file.files->data;
      level->file.files = g_slist_delete_link(level->file.files, level->file.files);

      include_file = fopen(filename, "r");
      if (!include_file)
        {
          msg_error("Error opening include file",
                    evt_tag_str("filename", filename),
                    evt_tag_int("depth", self->include_depth),
                    NULL);
          g_free(filename);
          return FALSE;
        }
      msg_debug("Starting to read include file",
                evt_tag_str("filename", filename),
                evt_tag_int("depth", self->include_depth),
                NULL);
      g_free(level->name);
      level->name = filename;

      level->file.include_file = include_file;
      level->yybuf = _cfg_lexer__create_buffer(level->file.include_file, YY_BUF_SIZE, self->state);
    }
  else
    {
      g_assert_not_reached();
    }

  level->lloc.first_line = level->lloc.last_line = 1;
  level->lloc.first_column = level->lloc.last_column = 1;
  level->lloc.level = level;

  _cfg_lexer__switch_to_buffer(level->yybuf, self->state);
  return TRUE;
}

gboolean
cfg_lexer_include_file(CfgLexer *self, const gchar *filename_)
{
  struct stat st;
  gchar *filename;

  if (self->include_depth >= MAX_INCLUDE_DEPTH - 1)
    {
      msg_error("Include file depth is too deep, increase MAX_INCLUDE_DEPTH and recompile",
                evt_tag_str("filename", filename_),
                evt_tag_int("depth", self->include_depth),
                NULL);
      return FALSE;
    }

  filename = find_file_in_path(cfg_args_get(self->globals, "include-path"), filename_, G_FILE_TEST_EXISTS);
  if (!filename || stat(filename, &st) < 0)
    {
#if HAVE_GLOB_H || defined(_WIN32)
      if (cfg_lexer_include_file_glob(self, filename_))
        return TRUE;
#endif

      msg_error("Include file/directory not found",
                evt_tag_str("filename", filename_),
                evt_tag_str("include-path", cfg_args_get(self->globals, "include-path")),
                evt_tag_errno("error", errno),
                NULL);
      return FALSE;
    }
  else
    {
      gboolean result;

      result = cfg_lexer_include_file_simple(self, filename);
      g_free(filename);
      return result;
    }
}

gboolean
cfg_lexer_include_buffer(CfgLexer *self, const gchar *name, gchar *buffer, gsize length)
{
  CfgIncludeLevel *level;

  /* lex requires two NUL characters at the end of the input */
  buffer = g_realloc(buffer, length + 2);
  buffer[length] = 0;
  buffer[length + 1] = 0;
  length += 2;

  if (self->include_depth >= MAX_INCLUDE_DEPTH - 1)
    {
      msg_error("Include file depth is too deep, increase MAX_INCLUDE_DEPTH and recompile",
                evt_tag_str("buffer", name),
                evt_tag_int("depth", self->include_depth),
                NULL);
      return FALSE;
    }

  self->include_depth++;
  level = &self->include_stack[self->include_depth];

  level->include_type = CFGI_BUFFER;
  level->buffer.content = buffer;
  level->buffer.content_length = length;
  level->name = g_strdup(name);

  return cfg_lexer_start_next_include(self);
}
