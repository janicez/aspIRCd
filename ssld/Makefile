#
# Makefile.in for ssld/src
#
# $Id: Makefile.in 1285 2006-05-05 15:03:53Z nenolod $
#

CC              = gcc
INSTALL         = /usr/bin/install -c
INSTALL_BIN     = ${INSTALL}
INSTALL_DATA    = ${INSTALL} -m 644
INSTALL_SUID    = ${INSTALL} -o root -m 4755
RM              = /bin/rm
LEX             = flex
LEXLIB          = 
CFLAGS          =  -O0 -Wall -std=gnu99 -g -g -O2  -DIRCD_PREFIX=\"/home/matty/ircd\"
LDFLAGS         =  -Wl,-export-dynamic -Wl,-rpath=${libdir}
MKDEP           = gcc -MM -DIRCD_PREFIX=\"/home/matty/ircd\"
MV              = /bin/mv
RM              = /bin/rm
prefix          = /home/matty/ircd
exec_prefix     = ${prefix}
bindir          = ${exec_prefix}/bin
libdir		= ${exec_prefix}/lib
libexecdir      = ${bindir}
confdir         = /home/matty/ircd/etc
localstatedir   = ${prefix}

ZIP_LIB		= 

IRCDLIBS	=  -L../libratbox/src/.libs -lratbox -ldl -lcrypt  $(SSL_LIBS) $(ZIP_LIB)

INCLUDES        = -I. -I../include -I../libratbox/include $(SSL_INCLUDES)
CPPFLAGS        = ${INCLUDES} 

PROGS		= ssld

SOURCES =     \
  ssld.c
  

OBJECTS = ${SOURCES:.c=.o}

all: ssld

build: all

ssld: ${OBJECTS}
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ ${OBJECTS} ${IRCDLIBS}

install: build
	@echo "ircd: installing ssld ($(PROGS))"
	@for i in $(PROGS); do \
                if test -f $(DESTDIR)$(bindir)/$$i; then \
                        $(MV) $(DESTDIR)$(bindir)/$$i $(DESTDIR)$(bindir)/$$i.old; \
                fi; \
                $(INSTALL_BIN) $$i $(DESTDIR)$(bindir); \
        done

.c.o:
	${CC} ${CPPFLAGS} ${CFLAGS} -c $<

.PHONY: depend clean distclean
depend:
	@${MKDEP} ${CPPFLAGS} ${SOURCES} > .depend.tmp
	@sed -e '/^# DO NOT DELETE THIS LINE/,$$d' <Makefile >Makefile.depend
	@echo '# DO NOT DELETE THIS LINE!!!' >>Makefile.depend
	@echo '# make depend needs it.' >>Makefile.depend
	@cat .depend.tmp >>Makefile.depend
	@mv Makefile.depend Makefile
	@rm -f .depend.tmp

clean:
	${RM} -f *.o *~ *.core core ssld

lint:
	lint -aacgprxhH $(CPPFLAGS) -DIRCD_PREFIX=\"/home/matty/ircd\" $(SOURCES) >>../lint.out

distclean: clean
	${RM} -f Makefile

# End of Makefile
