#
# Makefile.in for ircd/unsupported
#
# $Id: Makefile.in 3478 2007-05-24 15:10:06Z jilles $
#
CC		= gcc
RM		= /bin/rm
SED             = /bin/sed
LEX		= flex
LEXLIB		= -lfl
CFLAGS		=  -O0 -Wall -std=gnu99 -g -g -O2  -DIRCD_PREFIX=\"/home/j4jackj/hIRCd/run\"
PICFLAGS	= -fPIC -DPIC -shared
MKDEP		= gcc -MM
INSTALL		= /usr/bin/install -c
INSTALL_PROGRAM	= ${INSTALL}
INSTALL_DATA	= ${INSTALL} -m 644
INSTALL_SUID    = ${INSTALL} -o root -m 4755
SHELL		= /bin/sh
AUTOMODULEDIR	= /home/j4jackj/hIRCd/run/modules/unsupported

SSL_LIBS	=  -lcrypto
SSL_INCLUDES	= 

IRCDLIBS	= -ldl -lcrypt  $(SSL_LIBS)

INCLUDES	= -I. -I../include -I../libcharybdis -I../libratbox/include $(SSL_INCLUDES)
CPPFLAGS	= ${INCLUDES} 

SRCS =                          \
  make_override_immune.c	\
  sno_channeljoin.c

OBJS = ${SRCS:.c=.so}

default:	build
build: all
all: $(OBJS)

install: all
	-@if test ! -d $(DESTDIR)$(AUTOMODULEDIR); then \
                mkdir $(DESTDIR)$(AUTOMODULEDIR); \
        fi
	@echo "Installing modules into $(DESTDIR)$(AUTOMODULEDIR) .."
	@for file in $(OBJS); do \
		$(INSTALL_DATA) $$file $(DESTDIR)$(AUTOMODULEDIR); \
	done

.SUFFIXES: .so

.c.so:
	${CC} ${PICFLAGS}  ${CPPFLAGS} ${CFLAGS} $< -o $@

.PHONY: depend clean distclean
depend:
	@${MKDEP} ${CPPFLAGS} ${SRCS} > .depend
	@sed s/\\\.o/\\\.so/ < .depend > .depend.tmp
	@sed -e '/^# DO NOT DELETE THIS LINE/,$$d' <Makefile >Makefile.depend
	@echo '# DO NOT DELETE THIS LINE!!!' >>Makefile.depend
	@echo '# make depend needs it.' >>Makefile.depend
	@cat .depend.tmp >>Makefile.depend
	@mv Makefile.depend Makefile
	@rm -f .depend.tmp .depend

clean:
	${RM} -f *.so *~ 

distclean: clean
	${RM} -f Makefile

