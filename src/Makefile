#
# Makefile.in for ircd/src
#
# $Id: Makefile.in 3360 2007-04-03 09:37:03Z nenolod $
#
CC		= gcc
INSTALL		= /usr/bin/install -c
INSTALL_BIN	= ${INSTALL}
INSTALL_DATA	= ${INSTALL} -m 644
INSTALL_SUID	= ${INSTALL} -o root -m 4755
RM		= /bin/rm
LEX		= flex
LEXLIB		= 
PICFLAGS	= -fPIC -DPIC -shared
CFLAGS		=  -O0 -Wall -std=gnu99 -g -g -O2  -DIRCD_PREFIX=\"/home/matty/ircd\"
LDFLAGS   	=  -Wl,-export-dynamic -Wl,-rpath=${libdir}
MKDEP		= gcc -MM -DIRCD_PREFIX=\"/home/matty/ircd\"
MV		= /bin/mv
RM		= /bin/rm
YACC		= bison -y
prefix		= /home/matty/ircd
exec_prefix	= ${prefix}
bindir		= ${exec_prefix}/bin
libdir		= ${exec_prefix}/lib
libexecdir	= ${bindir}

DOLLAR = $$

IRCD_EXE	= ircd

PROGS		= $(IRCD_EXE)

SSL_LIBS	=  -lcrypto
SSL_INCLUDES	= 

IRCDLIBS	=  -L../libratbox/src/.libs -lratbox -ldl -lcrypt  $(SSL_LIBS)

INCLUDES	= -I../include -I../libratbox/include $(SSL_INCLUDES)
CPPFLAGS	= ${INCLUDES} 
 
default:	all

y.tab.o:	y.tab.c ircd_parser.y
	${CC} ${CPPFLAGS} ${PICFLAGS} ${CFLAGS} -I. -c y.tab.c

# Note GNU bison uses <file>.tab.c not y.tab.c
y.tab.c:	ircd_parser.y
	${YACC} -d ircd_parser.y

lex.yy.o:	lex.yy.c ircd_lexer.l
	${CC} ${CPPFLAGS} ${PICFLAGS} ${CFLAGS} -I. -c lex.yy.c

lex.yy.c:	ircd_lexer.l
	${LEX} ircd_lexer.l

SRCS =                          \
  bandbi.c			\
  blacklist.c			\
  cache.c			\
  channel.c                     \
  chmode.c			\
  class.c                       \
  client.c                      \
  extban.c                      \
  getopt.c                      \
  hash.c                        \
  hook.c                        \
  hostmask.c			\
  irc_dictionary.c		\
  ircd.c                        \
  ircd_signal.c                 \
  listener.c                    \
  logger.c                      \
  match.c                       \
  modules.c                     \
  monitor.c			\
  newconf.c			\
  numeric.c                     \
  operhash.c                    \
  packet.c                      \
  parse.c                       \
  privilege.c			\
  res.c				\
  reslib.c			\
  reject.c			\
  restart.c                     \
  s_auth.c                      \
  s_conf.c                      \
  s_newconf.c			\
  s_serv.c                      \
  s_user.c                      \
  scache.c                      \
  send.c                        \
  snomask.c			\
  sslproc.c			\
  substitution.c		\
  supported.c			\
  tgchange.c			\
  whowas.c

OBJS = ${SRCS:.c=.o}

all: ircd

build: all

ircd: $(OBJS) y.tab.o lex.yy.o version.o
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ ${OBJS} lex.yy.o y.tab.o version.o ${IRCDLIBS} ${LEXLIB}
	mv version.c version.c.last

install-mkdirs:
	-@if test ! -d $(DESTDIR)$(prefix); then \
		echo "ircd: setting up ircd directory structure"; \
		mkdir $(DESTDIR)$(prefix); \
	fi
	-@if test ! -d $(DESTDIR)$(exec_prefix); then \
		mkdir $(DESTDIR)$(exec_prefix); \
	fi
	-@if test ! -d $(DESTDIR)$(bindir); then \
		mkdir $(DESTDIR)$(bindir); \
	fi
	-@if test ! -d $(DESTDIR)$(libdir); then \
		mkdir $(DESTDIR)$(libdir); \
	fi

install: install-mkdirs build
	-@if test -f $(DESTDIR)$(bindir)/ircd; then \
		echo "ircd: backing up ircd"; \
	fi
	@echo "ircd: installing ircd ($(PROGS))"
	@for i in $(PROGS); do \
		if test -f $(DESTDIR)$(bindir)/$$i; then \
			$(MV) $(DESTDIR)$(bindir)/$$i $(DESTDIR)$(bindir)/$$i.old; \
		fi; \
		$(INSTALL_BIN) $$i $(DESTDIR)$(bindir); \
	done

version.c: version.c.SH
	/bin/sh ./version.c.SH


# this is really the default rule for c files
.c.o:
	${CC} ${CPPFLAGS} ${CFLAGS} -c $<
.s.o:
	${CC} ${CPPFLAGS} ${CFLAGS} -c $<

.PHONY: depend clean distclean
depend:
	${MKDEP} ${CPPFLAGS} ${SRCS} > .depend

clean:
	${RM} -f *.o *.exe *~ y.tab.* lex.yy.c ircd.core core ircd

lint:
	lint -aacgprxhH $(CPPFLAGS) -DIRCD_PREFIX=\"/home/matty/ircd\" $(SRCS) >>../lint.out

distclean: clean
	${RM} -f Makefile version.c.last

include .depend
