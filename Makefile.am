SUBDIRS = intl po
EXTRA_DIST = DESIGN qronos.jpg
bin_PROGRAMS = qronos qronosd
qronosd_SOURCES = qronosd.c log.c bbdd.c check.c qronosd.h log.h bbdd.h check.h
qronosd_LDADD = libmsg.a
qronos_SOURCES = exec.c auth.c call.c close.c qronos.c auth.h exec.h qronos.h
qronos_LDADD = libmsg.a
noinst_HEADERS = auth.h exec.h qronos.h msg.h qronosd.h log.h bbdd.h
noinst_LIBRARIES = libmsg.a
libmsg_a_SOURCES = msg.c msg.h
localedir = $(datadir)/locale
INCLUDES = -DLOCALEDIR=\"$(localedir)\"
