# Generated automatically from Makefile.in by configure.
# $Id: Makefile.in 1044 2006-03-12 16:05:39Z jilles $
# makefile for include/

INSTALL=	/usr/bin/install -c
INSTALL_DATA=	${INSTALL} -m 644
RM=		/bin/rm

prefix=		/home/matty/ircd
exec_prefix=	@execprefix@
helpdir=	/home/matty/ircd/help
uhelpdir=	${helpdir}/users
ohelpdir=	${helpdir}/opers

SYMLINKS=	topic accept cmode admin names links away whowas \
		version kick who invite quit join list nick oper part \
		time credits motd userhost users whois ison lusers \
		user help pass error challenge knock ping pong \
		cprivmsg cnotice map trace chantrace extban

all:
build:
clean:
depend:
lint:

index:
	@echo building index files
	rm -f users/index.tmp
	@for help in users/*; do \
		if [ -f $$help ]; then \
			echo $$help >> users/index.tmp; \
		fi \
	done
	@for help in $(SYMLINKS); do \
		echo $$help >> users/index.tmp; \
	done
	echo 'Help topics available to users:' > users/index
	echo '' >> users/index
	cat users/index.tmp \
		| sed -e 's|^users/||' \
		| sort -u \
		| tr a-z A-Z \
		| column -c 65 -x \
		| expand \
		>> users/index
	rm -f users/index.tmp
	rm -f opers/index.tmp
	@for help in opers/*; do \
		if [ -f $$help ]; then \
			echo $$help >> opers/index.tmp; \
		fi \
	done
	echo 'Help topics available to opers:' > opers/index
	echo '' >> opers/index
	cat opers/index.tmp \
		| sed -e 's|^opers/||' \
		| sort -u \
		| tr a-z A-Z \
		| column -c 65 -s ' ' -x \
		| expand \
		>> opers/index
	rm -f opers/index.tmp

install:
	-@if test -d $(DESTDIR)$(helpdir)-old; then \
		rm -rf $(DESTDIR)$(helpdir)-old; \
	fi
	-@if test -d $(DESTDIR)$(helpdir); then \
		echo "ircd: backing up old help files"; \
		mv $(DESTDIR)$(helpdir) $(DESTDIR)$(helpdir)-old; \
	fi

	@echo "ircd: setting up help directory structure"
	@mkdir -p -m 755 $(DESTDIR)$(helpdir)
	@mkdir -p -m 755 $(DESTDIR)$(helpdir)/opers
	@mkdir -p -m 755 $(DESTDIR)$(helpdir)/users

	@for help in opers/*; do \
	if [ -f $$help ]; then \
		${INSTALL_DATA} $$help $(DESTDIR)$(ohelpdir); \
	fi \
	done
	@for help in users/*; do \
	if [ -f $$help ]; then \
		$(INSTALL_DATA) $$help $(DESTDIR)$(uhelpdir); \
	fi \
	done
	@for link in $(SYMLINKS); do \
		rm -f $(DESTDIR)$(uhelpdir)/$$link; \
		ln -s $(ohelpdir)/$$link $(DESTDIR)$(uhelpdir); \
	done


distclean:
	${RM} -f Makefile

depend:
