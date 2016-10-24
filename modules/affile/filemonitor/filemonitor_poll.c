typedef struct _MonitorPoll
{
  MonitorBase super;
  GDir *dir;
  gint poll_freq;
  struct iv_timer poll_timer;
} MonitorPoll;

static void file_monitor_poll_timer_callback(gpointer s);

void
file_monitor_poll_destroy(gpointer source,gpointer monitor)
{
  MonitorPoll *self = (MonitorPoll *)source;
  if(iv_timer_registered(&self->poll_timer))
    {
      iv_timer_unregister(&self->poll_timer);
    }
  if (self->super.base_dir)
    g_free(self->super.base_dir);
}

MonitorPoll *
monitor_source_poll_new(guint32 poll_freq, FileMonitor *monitor)
{
  MonitorPoll *self = g_new0(MonitorPoll,1);
  self->super.base_dir = NULL;
  self->dir = NULL;
  self->poll_freq = poll_freq;
  IV_TIMER_INIT(&self->poll_timer);
  self->poll_timer.cookie = self;
  self->poll_timer.handler = file_monitor_poll_timer_callback;

  self->super.file_monitor = monitor;
  return self;
}

static gboolean
file_monitor_process_poll_event(FileMonitor *monitor, MonitorPoll *source)
{
  msg_trace("file_monitor_process_poll_event", NULL);
  file_monitor_list_directory(monitor, &source->super, source->super.base_dir);
  return TRUE;
}

static void
file_monitor_poll_timer_callback(gpointer s)
{
  MonitorPoll *self = (MonitorPoll *)s;
  self->poll_freq = self->super.file_monitor->poll_freq;
  file_monitor_process_poll_event(self->super.file_monitor, self);
  if(iv_timer_registered(&self->poll_timer))
    iv_timer_unregister(&self->poll_timer);
  iv_validate_now();
  self->poll_timer.expires = iv_now;
  timespec_add_msec(&self->poll_timer.expires, self->poll_freq);
  iv_timer_register(&self->poll_timer);
}

static gboolean
file_monitor_poll_init(MonitorPoll *self, const gchar *base_dir)
{
  self->super.base_dir = g_strdup(base_dir);
  return TRUE;
}

static void
monitor_poll_free(MonitorPoll *self)
{
  if (self->dir)
    g_dir_close(self->dir);
  if (self->super.base_dir)
    g_free(self->super.base_dir);
  if (iv_timer_registered(&self->poll_timer))
    iv_timer_unregister(&self->poll_timer);
}

MonitorBase *
file_monitor_create_poll(FileMonitor *self, const gchar *base_dir)
{
  MonitorPoll *source = monitor_source_poll_new(self->poll_freq, self);

  msg_debug("Initializing poll-based file monitoring method", evt_tag_str("directory", base_dir), NULL);
  if(!file_monitor_poll_init(source,base_dir))
    {
      monitor_poll_free(source);
      return NULL;
    }
  return &source->super;
}

void
file_monitor_start_poll(FileMonitor *self, MonitorBase *source, const gchar *base_dir)
{
  MonitorPoll *p_source = (MonitorPoll *)source;
  file_monitor_list_directory(self, source, base_dir);
  if(iv_timer_registered(&p_source->poll_timer))
    iv_timer_unregister(&p_source->poll_timer);
  iv_validate_now();
  p_source->poll_timer.expires = iv_now;
  timespec_add_msec(&p_source->poll_timer.expires, p_source->poll_freq);
  iv_timer_register(&p_source->poll_timer);
}
