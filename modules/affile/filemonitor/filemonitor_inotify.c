#if ENABLE_MONITOR_INOTIFY
# ifdef HAVE_SYS_INOTIFY_H
#  include <sys/inotify.h>
# elif HAVE_INOTIFYTOOLS_INOTIFY_H
#  include <inotifytools/inotify.h>
# endif
#endif

#if ENABLE_MONITOR_INOTIFY

typedef struct _MonitorInotify
{
  MonitorBase super;
  struct iv_fd fd_watch;
  guint32 watchd;
} MonitorInotify;

static gboolean
file_monitor_process_inotify_event(FileMonitor *monitor, MonitorInotify *self)
{
  struct inotify_event events[32];
  cap_t caps;
  gint byte_read = 0;
  gint i = 0;
  gchar *path = NULL;

  memset(events, 0, sizeof(events) );
  /* Read some events from the kernel buffer */
  byte_read = read(self->fd_watch.fd, &events, sizeof(events));

  if (byte_read == -1)
    {
      msg_error("Failed to read inotify event", evt_tag_errno("read", errno), NULL);
      return FALSE;
    }
  else if (byte_read == 0)
    {
      return TRUE;
    }

  caps = file_monitor_raise_caps(monitor);
  for (i = 0; i < 32; i++)
    {
      if (events[i].wd == self->watchd)
        {
          msg_debug("inotify process event",
                   evt_tag_str("filename", events[i].name),
                   NULL);
          path = resolve_to_absolute_path(events[i].name, self->super.base_dir);
          if (g_file_test(path, G_FILE_TEST_IS_DIR))
            {
              if (monitor->recursion)
                file_monitor_watch_directory(monitor, path);
            }
          else
            {
              /* file or symlink */
              file_monitor_chk_file(monitor, &self->super, events[i].name);
            }
          g_free(path);
        }

      if ((i+1)*sizeof(struct inotify_event) >= byte_read)
        break;
    }
  g_process_cap_restore(caps);

  return TRUE;
}

static void
monitor_inotify_process_input(gpointer s)
{
  MonitorInotify* self = (MonitorInotify *)s;
  file_monitor_process_inotify_event(self->super.file_monitor,self);
}

static void
monitor_inotify_init_watches(MonitorInotify *self)
{

  IV_FD_INIT(&self->fd_watch);
  self->fd_watch.cookie = self;
  self->fd_watch.handler_in = monitor_inotify_process_input;
}

static gboolean
monitor_inotify_init(MonitorInotify *self, const gchar *base_dir)
{
  main_loop_assert_main_thread();
  gint watchd = -1;
  /* Init inotify interface */
  gint ifd = inotify_init();
  monitor_inotify_init_watches(self);
  if (ifd == -1)
    {
      msg_error("Failed to init inotify subsystem", evt_tag_errno("inotify_init", errno), NULL);
      return FALSE;
    }
  /* Set the base directory */
  self->super.base_dir = g_strdup(base_dir);
  self->fd_watch.fd = ifd;
  watchd = inotify_add_watch(self->fd_watch.fd, base_dir, IN_MODIFY | IN_MOVED_TO | IN_CREATE);
  if (watchd == -1)
    {
      msg_error("Failed to add directory to inotify monitoring", evt_tag_str("directory", base_dir), evt_tag_errno("inotify_add_watch",errno), NULL);
      return FALSE;
    }
  self->watchd = watchd;
  iv_fd_register(&self->fd_watch);
  return TRUE;
}

static void
monitor_source_inotify_free(gpointer s)
{
  MonitorInotify *self = (MonitorInotify*) s;
  if (iv_fd_registered(&self->fd_watch))
    {
      iv_fd_unregister(&self->fd_watch);
    }
  if (self->fd_watch.fd != -1)
    {
      /* release watchd */
      inotify_rm_watch(self->fd_watch.fd, self->watchd);
      close(self->fd_watch.fd);
      self->fd_watch.fd = -1;
      self->watchd = 0;
    }
  /* Here we have to unregister the ivykis callings */
  if (self->super.base_dir)
    g_free(self->super.base_dir);
}

static MonitorBase *
monitor_inotify_new(FileMonitor *monitor)
{
  MonitorInotify *self = g_new0(MonitorInotify,1);
  self->super.base_dir = NULL;
  self->fd_watch.fd = -1;
  self->super.file_monitor = monitor;
  return &self->super;
}

MonitorBase *
file_monitor_create_inotify(FileMonitor *self, const gchar *base_dir)
{
  MonitorBase *source = monitor_inotify_new(self);

  msg_debug("Initializing inotify-based file monitoring method", evt_tag_str("directory", base_dir), NULL);
  if (!monitor_inotify_init((MonitorInotify*) source, base_dir))
    {
      monitor_source_inotify_free(source);
      msg_error("Failed to initialize inotify-based filesystem monitoring", evt_tag_str("directory", base_dir), NULL);
      return NULL;
    }
  return source;
}

void
file_monitor_start_inotify(FileMonitor *self, MonitorBase *source, const gchar *base_dir)
{
  file_monitor_list_directory(self, source, base_dir);
}

void
file_monitor_inotify_destroy(gpointer source,gpointer monitor)
{
  MonitorInotify *self = (MonitorInotify *)source;
  if (iv_fd_registered(&self->fd_watch))
    {
      iv_fd_unregister(&self->fd_watch);
    }
  if (self->fd_watch.fd != -1)
    {
      /* release watchd */
      inotify_rm_watch(self->fd_watch.fd, self->watchd);
      close(self->fd_watch.fd);
      self->fd_watch.fd = -1;
      self->watchd = 0;
    }
  if (self->super.base_dir)
    g_free(self->super.base_dir);
}
#endif
