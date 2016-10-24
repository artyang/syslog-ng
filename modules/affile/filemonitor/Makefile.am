filemonitor_headers = filemonitor/filemonitor.h
filemonitor_sources = filemonitor/filemonitor.c

if IS_WINDOWS
filemonitor_sources += filemonitor/filemonitor_win.c
else
filemonitor_headers += \
  filemonitor/filemonitor_unix.h \
  filemonitor/filemonitor_inotify.h \
  filemonitor/filemonitor_poll.h
filemonitor_sources += \
  filemonitor/filemonitor_unix.c \
  filemonitor/filemonitor_inotify.c \
  filemonitor/filemonitor_poll.c
endif
