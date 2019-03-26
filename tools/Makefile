# $Id: Makefile.in 6 2005-09-10 01:02:21Z nenolod $

CC		= gcc
INSTALL		= /usr/bin/install -c
INSTALL_BIN	= ${INSTALL}
INSTALL_DATA	= ${INSTALL} -m 644
INSTALL_SUID	= ${INSTALL} -o root -m 4755
RM		= /bin/rm
LEX		= flex
LEXLIB		= 
CFLAGS		=  -O0 -Wall -std=gnu99 -g -g -O2  -DIRCD_PREFIX=\"/home/matty/ircd\"
LDFLAGS   	=  -Wl,-export-dynamic -Wl,-rpath=${libdir}
MKDEP		= gcc -MM -DIRCD_PREFIX=\"/home/matty/ircd\"
MV		= /bin/mv
RM		= /bin/rm
LN              = /bin/ln

IRCDLIBS	= -L../libratbox/src/.libs -lratbox -ldl -lcrypt  
INCLUDES	= -I../include -I../libratbox/include
CPPFLAGS	= ${INCLUDES} 

prefix		= /home/matty/ircd
exec_prefix	= ${prefix}
bindir		= ${exec_prefix}/bin
libdir		= ${exec_prefix}/lib
libexecdir	= ${bindir}
confdir		= /home/matty/ircd/etc
localstatedir	= ${prefix}

PROGS		= viconf mkpasswd convertilines convertklines genssl.sh

all: $(PROGS)

build: all

mkpasswd: mkpasswd.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) mkpasswd.c -o mkpasswd $(IRCDLIBS)

viconf: viconf.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) viconf.c -o viconf $(IRCDLIBS)

convertilines: convertilines.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) convertilines.c -o convertilines $(IRCDLIBS)

convertklines: convertklines.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) convertklines.c -o convertklines $(IRCDLIBS)

genssl.sh:

clean:
	$(RM) -f encspeed viconf chkconf mkpasswd *~ core *.exe convertklines convertilines

distclean: clean
	$(RM) -f Makefile

lint:
	lint -aacgprxhH $(CPPFLAGS) -DIRCD_PREFIX=\"/home/matty/ircd\" $(convertklines_SOURCES) $(mkpasswd_SOURCES) $(viconf_SOURCES) $(encspeed_SOURCES) >>../lint.out

depend:

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) $(CPPFLAGS) -c $<

# DO NOT DELETE

viconf.o: ../include/config.h ../include/setup.h
viconf.o:


prefix		= /home/matty/ircd
exec_prefix	= ${prefix}
exec_suffix	= @exec_suffix@
bindir		= ${exec_prefix}/bin
libexecdir	= ${bindir}
confdir		= /home/matty/ircd/etc
localstatedir	= ${prefix}
# Change this later! -- adrian
moduledir	= /home/matty/ircd/modules
automoduledir	= /home/matty/ircd/modules/autoload


 
install-mkdirs:
	-@if test ! -d $(DESTDIR)$(prefix); then \
		echo "ircd: setting up tools directory structure"; \
		mkdir $(DESTDIR)$(prefix); \
	fi
	-@if test ! -d $(DESTDIR)$(exec_prefix); then \
		mkdir $(DESTDIR)$(exec_prefix); \
	fi
	-@if test ! -d $(DESTDIR)$(bindir); then \
		mkdir $(DESTDIR)$(bindir); \
	fi

install: install-mkdirs build
	@echo "ircd: installing tools ($(PROGS))"
	@for i in $(PROGS); do \
		if test -f $(DESTDIR)$(bindir)/$$i; then \
			$(MV) $(DESTDIR)$(bindir)/$$i $(DESTDIR)$(bindir)/$$i.old; \
		fi; \
		$(INSTALL_BIN) $$i $(DESTDIR)$(bindir); \
	done; \
	$(RM) -f $(DESTDIR)$(bindir)/vimotd
	$(LN) $(DESTDIR)$(bindir)/viconf $(DESTDIR)$(bindir)/vimotd

