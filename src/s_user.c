/*
 *  ircd-ratbox: A slightly useful ircd.
 *  s_user.c: User related functions.
 *
 *  Copyright (C) 1990 Jarkko Oikarinen and University of Oulu, Co Center
 *  Copyright (C) 1996-2002 Hybrid Development Team
 *  Copyright (C) 2002-2005 ircd-ratbox development team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 *  USA
 *
 *  $Id: s_user.c 3586 2007-11-20 11:16:43Z nenolod $
 */

#include "stdinc.h"
#include "s_user.h"
#include "channel.h"
#include "class.h"
#include "client.h"
#include "common.h"
#include "hash.h"
#include "match.h"
#include "ircd.h"
#include "listener.h"
#include "msg.h"
#include "numeric.h"
#include "s_conf.h"
#include "s_newconf.h"
#include "logger.h"
#include "s_serv.h"
#include "s_stats.h"
#include "scache.h"
#include "send.h"
#include "supported.h"
#include "whowas.h"
#include "packet.h"
#include "reject.h"
#include "cache.h"
#include "hook.h"
#include "monitor.h"
#include "snomask.h"
#include "blacklist.h"
#include "substitution.h"
#include "chmode.h"
#include "s_assert.h"
#include "irc_dictionary.h"

static void report_and_set_user_flags(struct Client *, struct ConfItem *);
void user_welcome(struct Client *source_p);

char umodebuf[128];

static int orphaned_umodes = 0;
int user_modes[256] = {
	/* 0x00 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0x0F */
	/* 0x10 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0x1F */
	/* 0x20 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0x2F */
	/* 0x30 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0x3F */
	0,			/* @ */
	0,			/* A */
	0,			/* B */
	0,			/* C */
	UMODE_DEAF,		/* D */
	0,			/* E */
	UMODE_CERTFPHIDE,	/* F */
	0,			/* G */
	UMODE_HIDEOPER,         /* H */
	UMODE_HIDEIDLE,	        /* I */
	0,			/* J */
	0,			/* K */
	0,			/* L */
	0,			/* M */
	0,			/* N */
        0,            	        /* O */
	0,			/* P */
	UMODE_NOFORWARD,	/* Q */
	UMODE_REGONLYMSG,	/* R */
	UMODE_SERVICE,		/* S */
	UMODE_SCTPCLIENT,	/* T */
	0,			/* U */
	0,			/* V */
	0,			/* W */
	0,			/* X */
	0,			/* Y */
	UMODE_SSLCLIENT,	/* Z */
	/* 0x5B */ 0, 0, 0, 0, 0, 0, /* 0x60 */
	UMODE_ADMIN,		/* a */
	0,			/* b */
	0,			/* c */
	0,			/* d */
	0,			/* e */
	0,			/* f */
	UMODE_CALLERID,		/* g */
	0,			/* h */
	UMODE_INVISIBLE,	/* i */
	0,			/* j */
	0,			/* k */
	UMODE_LOCOPS,		/* l */
	UMODE_STAFFONLYMSG,	/* m */
	0,			/* n */
	UMODE_OPER,		/* o */
	0,			/* p */
	0,			/* q */
	0,			/* r */
	UMODE_SERVNOTICE,	/* s */
	UMODE_SSLONLYMSG,	/* t */
	0,			/* u */
	0,			/* v */
	UMODE_WALLOP,		/* w */
	0,			/* x */
	0,			/* y */
	UMODE_OPERWALL,		/* z */
	/* 0x7B */ 0, 0, 0, 0, 0, /* 0x7F */
	/* 0x80 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0x9F */
	/* 0x90 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0x9F */
	/* 0xA0 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0xAF */
	/* 0xB0 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0xBF */
	/* 0xC0 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0xCF */
	/* 0xD0 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0xDF */
	/* 0xE0 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0xEF */
	/* 0xF0 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  /* 0xFF */
};


// table of usermodes
// first match wins, please don't use names that are here in modules
// to make a module one that cannot be configured in the config file,
// simply don't define a user_mode_names= in the file
// in unloads, you must reinitialise to NULL
// thanks
char *user_mode_names[256] = {
	/* 0x00 */ NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	 NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, /* 0x0F */
	/* 0x10 */ NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	 NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, /* 0x1F */
	/* 0x20 */ NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	 NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, /* 0x2F */
	/* 0x30 */ NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	 NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, /* 0x3F */
	NULL,			/* @ */
	NULL,		/* A */
	NULL,			/* B */
	NULL,			/* C */
	"deaf",			/* D */
	NULL,			/* E */
	"hidecert",		/* F */
	NULL,			/* G */
	NULL,			/* H */
	NULL,			/* I */
	NULL,			/* J */
	NULL,			/* K */
	NULL,			/* L */
	NULL,			/* M */
	NULL,	       		/* N */
	NULL,			/* O */
	NULL,			/* P */
	"noforward",		/* Q */
	"regonlymsg",		/* R */
	"services",		/* S */
	"sctpclient",		/* T */
	NULL,			/* U */
	NULL,			/* V */
	NULL,			/* W */
	NULL,			/* X */
	NULL,			/* Y */
	"sslclient",		/* Z */
	/* 0x5B */ NULL, NULL, NULL, NULL, NULL, NULL, /* 0x60 */
	"serveradmin",		/* a */
	NULL,			/* b */
	NULL,			/* c */
	NULL,			/* d */
	NULL,			/* e */
	NULL,			/* f */
	"callerid",		/* g */
	"helpop",		/* h */
	"invisible",		/* i */
	NULL,			/* j */
	NULL,			/* k */
	"locops",		/* l */
	NULL,			/* m */
	NULL,			/* n */
	"ircop",		/* o */
	"override",		/* p */
	NULL,			/* q */
	"registered",		/* r */
	"servnotice",	/* s */
	NULL,			/* t */
	NULL,			/* u */
	NULL,			/* v */
	"wallop",		/* w */
	NULL,			/* x */
	NULL,			/* y */
	"operwall",		/* z */
	/* 0x7B */ NULL, NULL, NULL, NULL, NULL, /* 0x7F */
	/* 0x80 */ NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	 NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, /* 0x9F */
	/* 0x90 */ NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	 NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, /* 0x9F */
	/* 0xA0 */ NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	 NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, /* 0xAF */
	/* 0xB0 */ NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	 NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, /* 0xBF */
	/* 0xC0 */ NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	 NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, /* 0xCF */
	/* 0xD0 */ NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	 NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, /* 0xDF */
	/* 0xE0 */ NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	 NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, /* 0xEF */
	/* 0xF0 */ NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	 NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0  /* 0xFF */
};
/* *INDENT-ON* */

/*
 * show_lusers -
 *
 * inputs	- pointer to client
 * output	-
 * side effects	- display to client user counts etc.
 */
int
show_lusers(struct Client *source_p)
{
	if(rb_dlink_list_length(&lclient_list) > (unsigned long)MaxClientCount)
		MaxClientCount = rb_dlink_list_length(&lclient_list);

	if((rb_dlink_list_length(&lclient_list) + rb_dlink_list_length(&serv_list)) >
	   (unsigned long)MaxConnectionCount)
		MaxConnectionCount = rb_dlink_list_length(&lclient_list) +
					rb_dlink_list_length(&serv_list);

	sendto_one_numeric(source_p, RPL_LUSERCLIENT, form_str(RPL_LUSERCLIENT),
			   (Count.total - Count.invisi),
			   Count.invisi,
			   (int)rb_dlink_list_length(&global_serv_list));

	if(rb_dlink_list_length(&oper_list) > 0)
		sendto_one_numeric(source_p, RPL_LUSEROP,
				   form_str(RPL_LUSEROP),
				   (int)rb_dlink_list_length(&oper_list));

	if(rb_dlink_list_length(&unknown_list) > 0)
		sendto_one_numeric(source_p, RPL_LUSERUNKNOWN,
				   form_str(RPL_LUSERUNKNOWN),
				   (int)rb_dlink_list_length(&unknown_list));

	if(rb_dlink_list_length(&global_channel_list) > 0)
		sendto_one_numeric(source_p, RPL_LUSERCHANNELS,
				   form_str(RPL_LUSERCHANNELS),
				   rb_dlink_list_length(&global_channel_list));

	sendto_one_numeric(source_p, RPL_LUSERME, form_str(RPL_LUSERME),
			   (int)rb_dlink_list_length(&lclient_list),
			   (int)rb_dlink_list_length(&serv_list));

	sendto_one_numeric(source_p, RPL_LOCALUSERS,
			   form_str(RPL_LOCALUSERS),
			   (int)rb_dlink_list_length(&lclient_list),
			   Count.max_loc,
			   (int)rb_dlink_list_length(&lclient_list),
			   Count.max_loc);

	sendto_one_numeric(source_p, RPL_GLOBALUSERS, form_str(RPL_GLOBALUSERS),
			   Count.total, Count.max_tot,
			   Count.total, Count.max_tot);

	sendto_one_numeric(source_p, RPL_STATSCONN,
			   form_str(RPL_STATSCONN),
			   MaxConnectionCount, MaxClientCount,
			   Count.totalrestartcount);

	return 0;
}

/*
** register_local_user
**      This function is called when both NICK and USER messages
**      have been accepted for the client, in whatever order. Only
**      after this, is the USER message propagated.
**
**      NICK's must be propagated at once when received, although
**      it would be better to delay them too until full info is
**      available. Doing it is not so simple though, would have
**      to implement the following:
**
**      (actually it has been implemented already for a while) -orabidoo
**
**      1) user telnets in and gives only "NICK foobar" and waits
**      2) another user far away logs in normally with the nick
**         "foobar" (quite legal, as this server didn't propagate
**         it).
**      3) now this server gets nick "foobar" from outside, but
**         has alread the same defined locally. Current server
**         would just issue "KILL foobar" to clean out dups. But,
**         this is not fair. It should actually request another
**         nick from local user or kill him/her...
*/

int
register_local_user(struct Client *client_p, struct Client *source_p)
{
	struct ConfItem *aconf, *xconf;
	struct User *user = source_p->user;
	char tmpstr2[IRCD_BUFSIZE];
	char ipaddr[HOSTIPLEN];
	char myusername[USERLEN+1];
	int status;

	s_assert(NULL != source_p);
	s_assert(MyConnect(source_p));

	if(source_p == NULL)
		return -1;

	if(IsAnyDead(source_p))
		return -1;

	if(ConfigFileEntry.ping_cookie)
	{
		if(!(source_p->flags & FLAGS_PINGSENT) && source_p->localClient->random_ping == 0)
		{
			source_p->localClient->random_ping = (unsigned long) (rand() * rand()) << 1;
			sendto_one(source_p, "PING :%08lX",
				   (unsigned long) source_p->localClient->random_ping);
			source_p->flags |= FLAGS_PINGSENT;
			return -1;
		}
		if(!(source_p->flags & FLAGS_PING_COOKIE))
		{
			return -1;
		}
	}

	/* hasnt finished client cap negotiation */
	if(source_p->flags & FLAGS_CLICAP)
		return -1;

	/* still has DNSbls to validate against */
	if(rb_dlink_list_length(&source_p->preClient->dnsbl_queries) > 0)
		return -1;

	client_p->localClient->last = rb_current_time();

	/* XXX - fixme. we shouldnt have to build a users buffer twice.. */
	if(!IsGotId(source_p) && (strchr(source_p->username, '[') != NULL))
	{
		const char *p;
		int i = 0;

		p = source_p->username;

		while(*p && i < USERLEN)
		{
			if(*p != '[')
				myusername[i++] = *p;
			p++;
		}

		myusername[i] = '\0';
	}
	else
		rb_strlcpy(myusername, source_p->username, sizeof myusername);

	if((status = check_client(client_p, source_p, myusername)) < 0)
		return (CLIENT_EXITED);

	/* Apply nick override */
	if(*source_p->preClient->spoofnick)
	{
		char note[NICKLEN + 10];

		del_from_client_hash(source_p->name, source_p);
		rb_strlcpy(source_p->name, source_p->preClient->spoofnick, NICKLEN + 1);
		add_to_client_hash(source_p->name, source_p);

		rb_snprintf(note, NICKLEN + 10, "Nick: %s", source_p->name);
		rb_note(source_p->localClient->F, note);
	}

	if(!valid_hostname(source_p->host))
	{
		sendto_one_notice(source_p, ":(\x02Notice\x02) You have an illegal character in your hostname");

		rb_strlcpy(source_p->host, source_p->sockhost, sizeof(source_p->host));
 	}


	aconf = source_p->localClient->att_conf;

	if(aconf == NULL)
	{
		exit_client(client_p, source_p, &me, "*** Not Authorised");
		return (CLIENT_EXITED);
	}

	if(IsConfSSLNeeded(aconf) && !IsSSL(source_p))
	{
		ServerStats.is_ref++;
		sendto_one_notice(source_p, ":(\x02Notice\x02) You need to use SSL/TLS to use this server");
		exit_client(client_p, source_p, &me, "Use SSL/TLS");
		return (CLIENT_EXITED);
	}

	if(!IsGotId(source_p))
	{
		const char *p;
		int i = 0;

		if(IsNeedIdentd(aconf))
		{
			ServerStats.is_ref++;
			sendto_one_notice(source_p, ":(\x02Notice\x02) You need to install identd to use this server");
			exit_client(client_p, source_p, &me, "Install identd");
			return (CLIENT_EXITED);
		}

		/* dont replace username if its supposed to be spoofed --fl */
		if(!IsConfDoSpoofIp(aconf) || !strchr(aconf->info.name, '@'))
		{
			p = myusername;

			if(!IsNoTilde(aconf))
				source_p->username[i++] = '~';

			while (*p && i < USERLEN)
			{
				if(*p != '[')
					source_p->username[i++] = *p;
				p++;
			}

			source_p->username[i] = '\0';
		}
	}

	if (IsConfUseUserIdent(aconf)) {
		// User is "use_user_ident". Always use ident from USER command.
		const char *wp; int wi = 0;
		wp = source_p->userusername;

		while (*wp && wi < USERLEN)
		{
			if(*wp != '[')
				source_p->username[wi++] = *wp;
			wp++;
		}

		source_p->username[wi] = '\0';

	}
	if(IsNeedSasl(aconf) && !*user->suser)
	{
		ServerStats.is_ref++;
		sendto_one_notice(source_p, ":(\x02Notice\x02) You need to identify via SASL to use this server");
		exit_client(client_p, source_p, &me, "SASL access only");
		return (CLIENT_EXITED);
	}

	/* password check */
	if(!EmptyString(aconf->passwd))
	{
		const char *encr;

		if(EmptyString(source_p->localClient->passwd))
			encr = "";
		else if(IsConfEncrypted(aconf))
			encr = rb_crypt(source_p->localClient->passwd, aconf->passwd);
		else
			encr = source_p->localClient->passwd;

		if(encr == NULL || strcmp(encr, aconf->passwd))
		{
			ServerStats.is_ref++;
			sendto_one(source_p, form_str(ERR_PASSWDMISMATCH), me.name, source_p->name);
			exit_client(client_p, source_p, &me, "Bad Password");
			return (CLIENT_EXITED);
		}

		/* clear password only if used now, otherwise send it
		 * to services -- jilles */
		if(source_p->localClient->passwd)
		{
			memset(source_p->localClient->passwd, 0, strlen(source_p->localClient->passwd));
			rb_free(source_p->localClient->passwd);
			source_p->localClient->passwd = NULL;
		}
	}

	/* report if user has &^>= etc. and set flags as needed in source_p */
	report_and_set_user_flags(source_p, aconf);

	/* Limit clients */
	/*
	 * We want to be able to have servers and F-line clients
	 * connect, so save room for "buffer" connections.
	 * Smaller servers may want to decrease this, and it should
	 * probably be just a percentage of the MAXCLIENTS...
	 *   -Taner
	 */
	/* Except "F:" clients */
	if(rb_dlink_list_length(&lclient_list) >=
	    (unsigned long)GlobalSetOptions.maxclients && !IsConfExemptLimits(aconf))
	{
		sendto_realops_snomask(SNO_FULL, L_ALL,
				     "Too many clients, rejecting %s[%s].", source_p->name, source_p->host);

		ServerStats.is_ref++;
		exit_client(client_p, source_p, &me, "Sorry, server is full - try later");
		return (CLIENT_EXITED);
	}

	/* kline exemption extends to xline too */
	if(!IsExemptKline(source_p) &&
	   (xconf = find_xline(source_p->info, 1)) != NULL)
	{
		ServerStats.is_ref++;
		add_reject(source_p, xconf->host, NULL);
		exit_client(client_p, source_p, &me, "Bad user info");
		return CLIENT_EXITED;
	}

	/* dnsbl check */
	if (source_p->preClient->dnsbl_listed != NULL)
	{
		if (IsExemptKline(source_p) || IsConfExemptDNSBL(aconf))
			sendto_one_notice(source_p, ":*** Your IP address %s is listed in %s, but you are exempt",
					source_p->sockhost, source_p->preClient->dnsbl_listed->host);
		else
		{
			sendto_realops_snomask(SNO_REJ, L_NETWIDE,
				"Listed on DNSBL %s: %s (%s@%s) [%s] [%s]",
				source_p->preClient->dnsbl_listed->host,
				source_p->name,
				source_p->username, source_p->host,
				IsIPSpoof(source_p) ? "255.255.255.255" : source_p->sockhost,
				source_p->info);

			rb_dlink_list varlist = { NULL, NULL, 0 };

			substitution_append_var(&varlist, "nick", source_p->name);
			substitution_append_var(&varlist, "ip", source_p->sockhost);
			substitution_append_var(&varlist, "host", source_p->host);
			substitution_append_var(&varlist, "dnsbl-host", source_p->preClient->dnsbl_listed->host);
			substitution_append_var(&varlist, "network-name", ServerInfo.network_name);

			ServerStats.is_ref++;

			sendto_one(source_p, form_str(ERR_YOUREBANNEDCREEP),
					me.name, source_p->name,
					substitution_parse(source_p->preClient->dnsbl_listed->reject_reason, &varlist));

			substitution_free(&varlist);

			sendto_one_notice(source_p, ":*** Your IP address %s is listed in %s",
					source_p->sockhost, source_p->preClient->dnsbl_listed->host);
			source_p->preClient->dnsbl_listed->hits++;
			add_reject(source_p, NULL, NULL);
			exit_client(client_p, source_p, &me, "*** Banned (DNS blacklist)");
			return CLIENT_EXITED;
		}
	}

	/* valid user name check */

	if(!valid_username(source_p->username))
	{
		sendto_realops_snomask(SNO_REJ, L_ALL,
				     "Invalid username: %s (%s@%s)",
				     source_p->name, source_p->username, source_p->host);
		ServerStats.is_ref++;
		sendto_one_notice(source_p, ":*** Your username is invalid. Please make sure that your username contains "
					    "only alphanumeric characters.");
		rb_sprintf(tmpstr2, "Invalid username [%s]", source_p->username);
		exit_client(client_p, source_p, &me, tmpstr2);
		return (CLIENT_EXITED);
	}

	/* end of valid user name check */

	/* Store original hostname -- jilles */
	rb_strlcpy(source_p->orighost, source_p->host, HOSTLEN + 1);

	/* Spoof user@host */
	if(*source_p->preClient->spoofuser)
		rb_strlcpy(source_p->username, source_p->preClient->spoofuser, USERLEN + 1);
	if(*source_p->preClient->spoofhost)
	{
		rb_strlcpy(source_p->host, source_p->preClient->spoofhost, HOSTLEN + 1);
		if (irccmp(source_p->host, source_p->orighost))
			SetDynSpoof(source_p);
	}

	source_p->umodes |= ConfigFileEntry.default_umodes & ~ConfigFileEntry.oper_only_umodes & ~orphaned_umodes;

	call_hook(h_new_local_user, source_p);

	/* If they have died in send_* or were thrown out by the
	 * new_local_user hook don't do anything. */
	if(IsAnyDead(source_p))
		return CLIENT_EXITED;

	/* To avoid inconsistencies, do not abort the registration
	 * starting from this point -- jilles
	 */
	rb_inet_ntop_sock((struct sockaddr *)&source_p->localClient->ip, ipaddr, sizeof(ipaddr));

	sendto_realops_snomask(SNO_CCONN, L_ALL,
			     "Client connecting: %s (%s@%s) [%s] {%s} [%s]",
			     source_p->name, source_p->username, source_p->orighost,
			     show_ip(NULL, source_p) ? ipaddr : "255.255.255.255",
			     get_client_class(source_p), source_p->info);

	sendto_realops_snomask(SNO_CCONNEXT, L_ALL,
			"CLICONN %s %s %s %s %s %s 0 :%s",
			source_p->name, source_p->username, source_p->orighost,
			show_ip(NULL, source_p) ? ipaddr : "255.255.255.255",
			get_client_class(source_p),
			/* mirc can sometimes send ips here */
			show_ip(NULL, source_p) ? source_p->localClient->fullcaps : "<hidden> <hidden>",
			source_p->info);

	add_to_hostname_hash(source_p->orighost, source_p);

	/* Allocate a UID if it was not previously allocated.
	 * If this already occured, it was probably during SASL auth...
	 */
	if(!*source_p->id)
	{
		strcpy(source_p->id, generate_uid());
		add_to_id_hash(source_p->id, source_p);
	}

	if (IsSSL(source_p))
		source_p->umodes |= UMODE_SSLCLIENT;

	if (IsSCTP(source_p))
		source_p->umodes |= UMODE_SCTPCLIENT;

	if (source_p->umodes & UMODE_INVISIBLE)
		Count.invisi++;

	s_assert(!IsClient(source_p));
	rb_dlinkMoveNode(&source_p->localClient->tnode, &unknown_list, &lclient_list);
	SetClient(source_p);

	source_p->servptr = &me;
	rb_dlinkAdd(source_p, &source_p->lnode, &source_p->servptr->serv->users);

	/* Increment our total user count here */
	if(++Count.total > Count.max_tot)
		Count.max_tot = Count.total;

	Count.totalrestartcount++;

	s_assert(source_p->localClient != NULL);

	if(rb_dlink_list_length(&lclient_list) > (unsigned long)Count.max_loc)
	{
		Count.max_loc = rb_dlink_list_length(&lclient_list);
		if(!(Count.max_loc % 10))
			sendto_realops_snomask(SNO_GENERAL, L_ALL,
					     "New Max Local Clients: %d", Count.max_loc);
	}

	/* they get a reduced limit */
	if(find_tgchange(source_p->sockhost))
		source_p->localClient->targets_free = TGCHANGE_INITIAL_LOW;
	else
		source_p->localClient->targets_free = TGCHANGE_INITIAL;

	monitor_signon(source_p);
	user_welcome(source_p);

	free_pre_client(source_p);

	return (introduce_client(client_p, source_p, user, source_p->name, 1));
}

/*
 * introduce_clients
 *
 * inputs	-
 * output	-
 * side effects - This common function introduces a client to the rest
 *		  of the net, either from a local client connect or
 *		  from a remote connect.
 */
int
introduce_client(struct Client *client_p, struct Client *source_p, struct User *user, const char *nick, int use_euid)
{
	char ubuf[BUFSIZE];
	struct Client *identifyservice_p;
	char *p;
	hook_data_umode_changed hdata;
	hook_data_client hdata2;
	struct Metadata *md;
	struct DictionaryIter iter;

	if(MyClient(source_p))
		send_umode(source_p, source_p, 0, ubuf);
	else
		send_umode(NULL, source_p, 0, ubuf);

	if(!*ubuf)
	{
		ubuf[0] = '+';
		ubuf[1] = '\0';
	}

	s_assert(has_id(source_p));

	if (use_euid)
		sendto_server(client_p, NULL, CAP_EUID | CAP_TS6, NOCAPS,
				":%s EUID %s %d %ld %s %s %s %s %s %s %s :%s",
				source_p->servptr->id, nick,
				source_p->hopcount + 1,
				(long) source_p->tsinfo, ubuf,
				source_p->username, source_p->host,
				IsIPSpoof(source_p) ? "0" : source_p->sockhost,
				source_p->id,
				IsDynSpoof(source_p) ? source_p->orighost : "*",
				EmptyString(source_p->user->suser) ? "*" : source_p->user->suser,
				source_p->info);

	sendto_server(client_p, NULL, CAP_TS6, use_euid ? CAP_EUID : NOCAPS,
		      ":%s UID %s %d %ld %s %s %s %s %s :%s",
		      source_p->servptr->id, nick,
		      source_p->hopcount + 1,
		      (long) source_p->tsinfo, ubuf,
		      source_p->username, source_p->host,
		      IsIPSpoof(source_p) ? "0" : source_p->sockhost,
		      source_p->id, source_p->info);

	if(!EmptyString(source_p->certfp))
		sendto_server(client_p, NULL, CAP_TS6, NOCAPS,
				":%s ENCAP * CERTFP :%s",
				use_id(source_p), source_p->certfp);

	DICTIONARY_FOREACH(md, &iter, source_p->metadata)
	{
		sendto_server(client_p, NULL, CAP_TS6, NOCAPS, ":%s ENCAP * METADATA ADD %s %s :%s",
		          use_id(&me), use_id(source_p), md->name, md->value);
	}

	if (IsDynSpoof(source_p))
	{
		sendto_server(client_p, NULL, CAP_TS6, use_euid ? CAP_EUID : NOCAPS, ":%s ENCAP * REALHOST %s",
				use_id(source_p), source_p->orighost);
	}

	if (!EmptyString(source_p->user->suser))
	{
		sendto_server(client_p, NULL, CAP_TS6, use_euid ? CAP_EUID : NOCAPS, ":%s ENCAP * LOGIN %s",
				use_id(source_p), source_p->user->suser);
	}

	if(MyConnect(source_p) && source_p->localClient->passwd)
	{
		if (!EmptyString(ConfigFileEntry.identifyservice) &&
				!EmptyString(ConfigFileEntry.identifycommand))
		{
			/* use user@server */
			p = strchr(ConfigFileEntry.identifyservice, '@');
			if (p != NULL)
				identifyservice_p = find_named_client(p + 1);
			else
				identifyservice_p = NULL;
			if (identifyservice_p != NULL)
			{
				if (!EmptyString(source_p->localClient->auth_user))
					sendto_one(identifyservice_p, ":%s PRIVMSG %s :%s %s %s",
							get_id(source_p, identifyservice_p),
							ConfigFileEntry.identifyservice,
							ConfigFileEntry.identifycommand,
							source_p->localClient->auth_user,
							source_p->localClient->passwd);
				else
					sendto_one(identifyservice_p, ":%s PRIVMSG %s :%s %s",
							get_id(source_p, identifyservice_p),
							ConfigFileEntry.identifyservice,
							ConfigFileEntry.identifycommand,
							source_p->localClient->passwd);
			}
		}
		memset(source_p->localClient->passwd, 0, strlen(source_p->localClient->passwd));
		rb_free(source_p->localClient->passwd);
		source_p->localClient->passwd = NULL;
	}

	/* let modules providing usermodes know that we've got a new user,
	 * why is this here? -- well, some modules need to be able to send out new
	 * information about a client, so this was the best place to do it
	 *    --nenolod
	 */
	hdata.client = source_p;
	hdata.oldumodes = 0;
	hdata.oldsnomask = 0;
	call_hook(h_umode_changed, &hdata);

	/* On the other hand, some modules need to know when a client is
	 * being introduced, period.
	 * --gxti
	 */
	hdata2.client = client_p;
	hdata2.target = source_p;
	call_hook(h_introduce_client, &hdata2);

	return 0;
}

/*
 * valid_hostname - check hostname for validity
 *
 * Inputs       - pointer to user
 * Output       - YES if valid, NO if not
 * Side effects - NONE
 *
 * NOTE: this doesn't allow a hostname to begin with a dot and
 * will not allow more dots than chars.
 */
int
valid_hostname(const char *hostname)
{
	const char *p = hostname, *last_slash = 0;
	int found_sep = 0;

	s_assert(NULL != p);

	if(hostname == NULL)
		return NO;

	if(!strcmp(hostname, "localhost"))
		return YES;

	if('.' == *p || ':' == *p || '/' == *p)
		return NO;

	while (*p)
	{
		if(!IsHostChar(*p))
			return NO;
                if(*p == '.' || *p == ':')
  			found_sep++;
		else if(*p == '/')
		{
			found_sep++;
			last_slash = p;
		}
		p++;
	}

	if(found_sep == 0)
		return NO;

	if(last_slash && IsDigit(last_slash[1]))
		return NO;

	return YES;
}

/*
 * valid_username - check username for validity
 *
 * Inputs       - pointer to user
 * Output       - YES if valid, NO if not
 * Side effects - NONE
 *
 * Absolutely always reject any '*' '!' '?' '@' in an user name
 * reject any odd control characters names.
 * Allow '.' in username to allow for "first.last"
 * style of username
 */
int
valid_username(const char *username)
{
	int dots = 0;
	const char *p = username;

	s_assert(NULL != p);

	if(username == NULL)
		return NO;

	if('~' == *p)
		++p;

	/* reject usernames that don't start with an alphanum
	 * i.e. reject jokers who have '-@somehost' or '.@somehost'
	 * or "-hi-@somehost", "h-----@somehost" would still be accepted.
	 */
	if(!IsAlNum(*p))
		return NO;

	while (*++p)
	{
		if((*p == '.') && ConfigFileEntry.dots_in_ident)
		{
			dots++;
			if(dots > ConfigFileEntry.dots_in_ident)
				return NO;
			if(!IsUserChar(p[1]))
				return NO;
		}
		else if(!IsUserChar(*p))
			return NO;
	}
	return YES;
}

/* report_and_set_user_flags
 *
 * Inputs       - pointer to source_p
 *              - pointer to aconf for this user
 * Output       - NONE
 * Side effects -
 * Report to user any special flags they are getting, and set them.
 */

static void
report_and_set_user_flags(struct Client *source_p, struct ConfItem *aconf)
{
	/* If this user is being spoofed, tell them so */
	if(IsConfDoSpoofIp(aconf))
	{
#ifdef FLIRTY
		sendto_one_notice(source_p, ":*** Hiding your IP by static spoof. The ircd bets the operators love you ;)");
#else
		sendto_one_notice(source_p, ":*** Hiding your IP by static spoof.");
#endif
	}

	/* If this user is in the exception class, Set it "E lined" */
	if(IsConfExemptKline(aconf))
	{
		SetExemptKline(source_p);
		sendto_one_notice(source_p, ":*** You are exempt from K/X lines, be they local or global. Why this is a good thing isn't clear to the coder.");
	}

	if(IsConfExemptDNSBL(aconf))
		/* kline exempt implies this, don't send both */
		if(!IsConfExemptKline(aconf))
			sendto_one_notice(source_p, ":*** You are exempt from being checked against DNS blackhole lists. Good news, eh?");

	/* If this user is exempt from user limits set it F lined" */
	if(IsConfExemptLimits(aconf))
	{
		sendto_one_notice(source_p, ":*** Your I:line permits you to connect even if it's full - you're exempt from user limits");
	}

	if(IsConfExemptFlood(aconf) || ConfFloodMultiplier(aconf) == 0)
	{
		SetExemptFlood(source_p);
		sendto_one_notice(source_p, ":*** You are completely exempt from flood limits. Congratulations. Bask in the knowlege that your voluminous contributions to the community will not be throttled.");
	}

	source_p->localClient->flood_multiplier = ConfFloodMultiplier(aconf);

	if(ConfFloodMultiplier(aconf) > 16 && !IsConfExemptFlood(aconf))
	{
		sendto_one_notice(source_p, ":*** Warning: You are more severely limited from flooding than average connections. This could be because your server admin doesn't want distant connections to be able to talk as much as locals, to encourage them to use a closer server? Ask the admin.");
	}

	if(ConfFloodMultiplier(aconf) < 16 && !IsConfExemptFlood(aconf))
	{
		sendto_one_notice(source_p, ":*** You are less severely limited from flooding than average connections. Bask in the knowlege that your voluminous contributions will take longer to get throttled.");
	}

	if(IsConfExemptSpambot(aconf))
	{
		SetExemptSpambot(source_p);
		sendto_one_notice(source_p, ":*** You are exempt from spambot checks. Lucky you!");
	}

	if(IsConfExemptJupe(aconf))
	{
		SetExemptJupe(source_p);
		sendto_one_notice(source_p, ":*** If you attempt to join a juped channel, the opers won't be warned. This is unlike most connections.");
	}

	if(IsConfExemptResv(aconf))
	{
		SetExemptResv(source_p);
		sendto_one_notice(source_p, ":*** You may use juped nicknames if you like.");
	}

	if(IsConfExemptShide(aconf))
	{
		SetExemptShide(source_p);
		sendto_one_notice(source_p, ":*** You are able to see information about servers that most users may not.");
	}
}

static void
show_other_user_mode(struct Client *source_p, struct Client *target_p)
{
	int i;
	char buf[BUFSIZE];
	char *m;

	m = buf;
	*m++ = '+';

	for (i = 0; i < 128; i++) /* >= 127 is extended ascii */
		if (target_p->umodes & user_modes[i])
			*m++ = (char) i;
	*m = '\0';

	if (MyConnect(target_p) && target_p->snomask != 0)
		sendto_one_notice(source_p, ":Modes for %s are %s %s",
				target_p->name, buf,
				construct_snobuf(target_p->snomask));
	else
		sendto_one_notice(source_p, ":Modes for %s are %s",
				target_p->name, buf);
}

/*
 * user_mode - set get current users mode
 *
 * m_umode() added 15/10/91 By Darren Reed.
 * parv[1] - username to change mode for
 * parv[2] - modes to change
 */
int
user_mode(struct Client *client_p, struct Client *source_p, int parc, const char *parv[])
{
	int flag;
	int i;
	char *m;
	const char *pm;
	struct Client *target_p;
	int what, setflags;
	int badflag = NO;	/* Only send one bad flag notice */
	int showsnomask = NO;
	unsigned int setsnomask;
	char buf[BUFSIZE];
	hook_data_umode_changed hdata;

	what = MODE_ADD;

	if(parc < 2)
	{
		sendto_one(source_p, form_str(ERR_NEEDMOREPARAMS), me.name, source_p->name, "MODE");
		return 0;
	}

	if((target_p = MyClient(source_p) ? find_named_person(parv[1]) : find_person(parv[1])) == NULL)
	{
		if(MyConnect(source_p))
			sendto_one_numeric(source_p, ERR_NOSUCHCHANNEL,
					   form_str(ERR_NOSUCHCHANNEL), parv[1]);
		return 0;
	}

	/* Dont know why these were commented out..
	 * put them back using new sendto() funcs
	 */

	if(IsServer(source_p))
	{
		sendto_realops_snomask(SNO_GENERAL, L_ADMIN,
				     "*** Mode for User %s from %s", parv[1], source_p->name);
		return 0;
	}

	if(source_p != target_p)
	{
		if (MyOper(source_p) && parc < 3)
			show_other_user_mode(source_p, target_p);
		else
			sendto_one(source_p, form_str(ERR_USERSDONTMATCH), me.name, source_p->name);
		return 0;
	}

	if(parc < 3)
	{
		m = buf;
		*m++ = '+';

		for (i = 0; i < 128; i++) /* >= 127 is extended ascii */
			if (source_p->umodes & user_modes[i])
				*m++ = (char) i;

		*m = '\0';
		sendto_one_numeric(source_p, RPL_UMODEIS, form_str(RPL_UMODEIS), buf);

		if (source_p->snomask != 0)
			sendto_one_numeric(source_p, RPL_SNOMASK, form_str(RPL_SNOMASK),
				construct_snobuf(source_p->snomask));

		return 0;
	}

	/* find flags already set for user */
	setflags = source_p->umodes;
	setsnomask = source_p->snomask;

	/*
	 * parse mode change string(s)
	 */
	for (pm = parv[2]; *pm; pm++)
		switch (*pm)
		{
		case '+':
			what = MODE_ADD;
			break;
		case '-':
			what = MODE_DEL;
			break;

		case 'o':
			if(what == MODE_ADD)
			{
				if(IsServer(client_p) && !IsOper(source_p))
				{
					++Count.oper;
					SetOper(source_p);
					rb_dlinkAddAlloc(source_p, &oper_list);
				}
			}
			else
			{
				/* Only decrement the oper counts if an oper to begin with
				 * found by Pat Szuta, Perly , perly@xnet.com
				 */

				if(!IsOper(source_p))
					break;

				ClearOper(source_p);

				Count.oper--;

				if(MyConnect(source_p))
				{
					source_p->umodes &= ~ConfigFileEntry.oper_only_umodes;
					if (!(source_p->umodes & UMODE_SERVNOTICE) && source_p->snomask != 0)
					{
						source_p->snomask = 0;
						showsnomask = YES;
					}
					source_p->flags2 &= ~OPER_FLAGS;

					rb_free(source_p->localClient->opername);
					source_p->localClient->opername = NULL;

					rb_dlinkFindDestroy(source_p, &local_oper_list);
					privilegeset_unref(source_p->localClient->privset);
					source_p->localClient->privset = NULL;
					user_metadata_delete(source_p, "PRIVS", 1);
				}

				rb_dlinkFindDestroy(source_p, &oper_list);
			}
			break;

			/* we may not get these,
			 * but they shouldnt be in default
			 */

		/* can only be set on burst */
		case 'S':
		case 'Z':
		case 'T':
                case ' ':
		case '\n':
		case '\r':
		case '\t':
			break;

		case 's':
			if (MyConnect(source_p))
			{
				if(!IsOper(source_p)
						&& (ConfigFileEntry.oper_only_umodes & UMODE_SERVNOTICE))
				{
					if (what == MODE_ADD || source_p->umodes & UMODE_SERVNOTICE)
						badflag = YES;
					continue;
				}
				showsnomask = YES;
				if(what == MODE_ADD)
				{
					if (parc > 3)
						source_p->snomask = parse_snobuf_to_mask(source_p->snomask, parv[3]);
					else
						source_p->snomask |= SNO_GENERAL;
				}
				else
					source_p->snomask = 0;
				if (source_p->snomask != 0)
					source_p->umodes |= UMODE_SERVNOTICE;
				else
					source_p->umodes &= ~UMODE_SERVNOTICE;
				break;
			}
			/* FALLTHROUGH */
		default:
			if (MyConnect(source_p) && *pm == 'Q' && !ConfigChannel.use_forward)
			{
				badflag = YES;
				break;
			}

			if((flag = user_modes[(unsigned char) *pm]))
			{
				if(MyConnect(source_p)
						&& ((!IsOper(source_p)
							&& (ConfigFileEntry.oper_only_umodes & flag))
						|| (orphaned_umodes & flag)))
				{
					if (what == MODE_ADD || source_p->umodes & flag)
						badflag = YES;
				}
				else
				{
					if(what == MODE_ADD)
						source_p->umodes |= flag;
					else
						source_p->umodes &= ~flag;
				}
			}
			else
			{
				if(MyConnect(source_p))
					badflag = YES;
			}
			break;
		}

	if(badflag)
		sendto_one(source_p, form_str(ERR_UMODEUNKNOWNFLAG), me.name, source_p->name);

	if(MyClient(source_p) && (source_p->snomask & SNO_NCHANGE) && !IsOperN(source_p))
	{
		sendto_one_notice(source_p, ":*** You need oper and nick_changes flag for +s +n");
		source_p->snomask &= ~SNO_NCHANGE;	/* only tcm's really need this */
	}

	if(MyClient(source_p) && (source_p->umodes & UMODE_OPERWALL) && !IsOperOperwall(source_p))
	{
		sendto_one_notice(source_p, ":*** You need oper and operwall flag for +z");
		source_p->umodes &= ~UMODE_OPERWALL;
	}

	if(MyConnect(source_p) && (source_p->umodes & UMODE_ADMIN) &&
	   (!IsOperAdmin(source_p) || IsOperHiddenAdmin(source_p)))
	{
		sendto_one_notice(source_p, ":*** You need oper and admin flag for +a");
		source_p->umodes &= ~UMODE_ADMIN;
	}

        /* preventing anyone setting UMODE_HIDEOPER because not everyone will be IRC operator */

        if (MyConnect(source_p) && (source_p->umodes & UMODE_HIDEOPER) && (!IsOper(source_p))) {
            sendto_one_numeric(source_p, 481, ":Permission Denied - You're not an IRC operator");
            source_p->umodes &= ~UMODE_HIDEOPER;
         }

        if (MyConnect(source_p) && (source_p->umodes & UMODE_STAFFONLYMSG) && (!IsOper(source_p))) {
            sendto_one_numeric(source_p, 496, ":Permission Denied - You're not an IRC operator");
            source_p->umodes &= ~UMODE_STAFFONLYMSG;
         }

         if (MyConnect(source_p) && (source_p->umodes & UMODE_SSLONLYMSG) && (!IsOper(source_p))) {
            sendto_one_numeric(source_p, 497, ":*** Notice -- You need to be connected using SSL/TLS to set +t");
            source_p->umodes &= ~UMODE_STAFFONLYMSG;
         }


	/* let modules providing usermodes know that we've changed our usermode --nenolod */
	hdata.client = source_p;
	hdata.oldumodes = setflags;
	hdata.oldsnomask = setsnomask;
	call_hook(h_umode_changed, &hdata);

	if(!(setflags & UMODE_INVISIBLE) && IsInvisible(source_p))
		++Count.invisi;
	if((setflags & UMODE_INVISIBLE) && !IsInvisible(source_p))
		--Count.invisi;
	/*
	 * compare new flags with old flags and send string which
	 * will cause servers to update correctly.
	 */
	send_umode_out(client_p, source_p, setflags);
	if (showsnomask && MyConnect(source_p))
		sendto_one_numeric(source_p, RPL_SNOMASK, form_str(RPL_SNOMASK),
			construct_snobuf(source_p->snomask));

	return (0);
}

/*
 * send the MODE string for user (user) to connection client_p
 * -avalon
 */
void
send_umode(struct Client *client_p, struct Client *source_p, int old, char *umode_buf)
{
	int i;
	int flag;
	char *m;
	int what = 0;

	/*
	 * build a string in umode_buf to represent the change in the user's
	 * mode between the new (source_p->flag) and 'old'.
	 */
	m = umode_buf;
	*m = '\0';

	for (i = 0; i < 128; i++)
	{
		flag = user_modes[i];

		if((flag & old) && !(source_p->umodes & flag))
		{
			if(what == MODE_DEL)
				*m++ = (char) i;
			else
			{
				what = MODE_DEL;
				*m++ = '-';
				*m++ = (char) i;
			}
		}
		else if(!(flag & old) && (source_p->umodes & flag))
		{
			if(what == MODE_ADD)
				*m++ = (char) i;
			else
			{
				what = MODE_ADD;
				*m++ = '+';
				*m++ = (char) i;
			}
		}
	}
	*m = '\0';

	if(*umode_buf && client_p)
		sendto_one(client_p, ":%s MODE %s :%s", source_p->name, source_p->name, umode_buf);
}

/*
 * send_umode_out
 *
 * inputs	-
 * output	- NONE
 * side effects -
 */
void
send_umode_out(struct Client *client_p, struct Client *source_p, int old)
{
	struct Client *target_p;
	char buf[BUFSIZE];
	rb_dlink_node *ptr;

	send_umode(NULL, source_p, old, buf);

	RB_DLINK_FOREACH(ptr, serv_list.head)
	{
		target_p = ptr->data;

		if((target_p != client_p) && (target_p != source_p) && (*buf))
		{
			sendto_one(target_p, ":%s MODE %s :%s",
				   get_id(source_p, target_p),
				   get_id(source_p, target_p), buf);
		}
	}

	if(client_p && MyClient(client_p))
		send_umode(client_p, source_p, old, buf);
}

/*
 * user_welcome
 *
 * inputs	- client pointer to client to welcome
 * output	- NONE
 * side effects	-
 */
void
user_welcome(struct Client *source_p)
{
	sendto_one_numeric(source_p, RPL_WELCOME, form_str(RPL_WELCOME), ServerInfo.network_name, source_p->name);
	sendto_one_numeric(source_p, RPL_YOURHOST, form_str(RPL_YOURHOST),
		   get_listener_name(source_p->localClient->listener), ircd_version);
	sendto_one_numeric(source_p, RPL_CREATED, form_str(RPL_CREATED), creation);
	sendto_one_numeric(source_p, RPL_MYINFO, form_str(RPL_MYINFO), me.name, ircd_version, umodebuf, cflagsmyinfo);

	show_isupport(source_p);

	show_lusers(source_p);

	if(ConfigFileEntry.short_motd)
	{
		sendto_one_notice(source_p, ":(\x02Notice\x02) motd was last changed at %s", user_motd_changed);
		sendto_one_notice(source_p, ":(\x02Notice\x02) Please read the motd if you haven't read it");

		sendto_one(source_p, form_str(RPL_MOTDSTART),
			   me.name, source_p->name, me.name);

		sendto_one(source_p, form_str(RPL_MOTD),
			   me.name, source_p->name, "*** This is the short motd ***");

		sendto_one(source_p, form_str(RPL_ENDOFMOTD), me.name, source_p->name);
	}
	else
		send_user_motd(source_p);
}

/* oper_up()
 *
 * inputs	- pointer to given client to oper
 *		- pointer to ConfItem to use
 * output	- none
 * side effects	- opers up source_p using aconf for reference
 */
int
oper_up(struct Client *source_p, struct oper_conf *oper_p)
{
	unsigned int old = source_p->umodes, oldsnomask = source_p->snomask;
	int i;
	hook_data_umode_changed hdata;

	for (i = 0; i < strlen(oper_p->name); i++);
	SetOper(source_p);

	if(oper_p->umodes)
		source_p->umodes |= oper_p->umodes;
	else if(ConfigFileEntry.oper_umodes)
		source_p->umodes |= ConfigFileEntry.oper_umodes;
	else
		source_p->umodes |= DEFAULT_OPER_UMODES;

	if (oper_p->snomask)
	{
		source_p->snomask |= oper_p->snomask;
		source_p->umodes |= UMODE_SERVNOTICE;
	}
	else if (source_p->umodes & UMODE_SERVNOTICE)
	{
		/* Only apply these if +s is already set -- jilles */
		if (ConfigFileEntry.oper_snomask)
			source_p->snomask |= ConfigFileEntry.oper_snomask;
		else
			source_p->snomask |= DEFAULT_OPER_SNOMASK;
	}

	Count.oper++;

	SetExemptKline(source_p);

	source_p->flags2 |= oper_p->flags;
	source_p->localClient->opername = rb_strdup(oper_p->name);
	source_p->localClient->privset = privilegeset_ref(oper_p->privset);

	// broadcast privs over network
	user_metadata_delete(source_p, "PRIVS", 1);
	user_metadata_add(source_p, "PRIVS", oper_p->privset->privs, 1);

	rb_dlinkAddAlloc(source_p, &local_oper_list);
	rb_dlinkAddAlloc(source_p, &oper_list);

	if(IsOperAdmin(source_p) && !IsOperHiddenAdmin(source_p))
		source_p->umodes |= UMODE_ADMIN;
	if(!IsOperN(source_p))
		source_p->snomask &= ~SNO_NCHANGE;
	if(!IsOperOperwall(source_p))
		source_p->umodes &= ~UMODE_OPERWALL;
	hdata.client = source_p;
	hdata.oldumodes = old;
	hdata.oldsnomask = oldsnomask;
	call_hook(h_umode_changed, &hdata);

	sendto_realops_snomask(SNO_GENERAL, L_ALL,
			     "%s (%s!%s@%s) is now an operator", oper_p->name, source_p->name,
			     source_p->username, source_p->host);

	if (oper_p->operstring) user_metadata_add(source_p, "OPERSTRING", oper_p->operstring, 1);
	if (oper_p->swhois) user_metadata_add(source_p, "SWHOIS", oper_p->swhois, 1);
	if (oper_p->vhost) {
		if (!valid_hostname(oper_p->vhost))
			sendto_realops_snomask(SNO_GENERAL, L_NETWIDE,
				"Can someone nag the owner of %s to fix %s'%s oper block vhost so it's valid?",
				me.name, oper_p->name, (oper_p->name[i] == 's' || oper_p->name[i] == 'S' || oper_p->name[i] == 'z' || oper_p->name[i] == 'Z') ? "" : "s"
			);
		else {
			change_nick_user_host(source_p, source_p->name, source_p->username, oper_p->vhost, 0, "Opered up");
			sendto_one_numeric(source_p, RPL_HOSTHIDDEN, "%s :is now your hidden host (set by %s)", source_p->host, source_p->servptr->name);
			sendto_server(NULL, NULL, CAP_EUID | CAP_TS6, NOCAPS, ":%s CHGHOST %s :%s", use_id(&me), use_id(source_p), source_p->host);
			sendto_server(NULL, NULL, CAP_TS6, CAP_EUID|NOCAPS, ":%s ENCAP * CHGHOST %s :%s", use_id(&me), use_id(source_p), source_p->host);
			if (!IsDynSpoof(source_p)) SetDynSpoof(source_p);
		}
	}

	if(!(old & UMODE_INVISIBLE) && IsInvisible(source_p))
		++Count.invisi;
	if((old & UMODE_INVISIBLE) && !IsInvisible(source_p))
		--Count.invisi;
	if (MyClient(source_p) && oper_p->flood_multiplier != -1 && oper_p->flood_multiplier < source_p->localClient->flood_multiplier) {
		sendto_one_notice(source_p, ":*** Congratulations, your flood multiplier has decreased to %i", oper_p->flood_multiplier);
	}
	if (oper_p->flood_multiplier != -1 && oper_p->flood_multiplier < source_p->localClient->flood_multiplier) source_p->localClient->flood_multiplier = oper_p->flood_multiplier;
	send_umode_out(source_p, source_p, old);
	sendto_one_numeric(source_p, RPL_SNOMASK, form_str(RPL_SNOMASK),
		   construct_snobuf(source_p->snomask));
	sendto_one(source_p, form_str(RPL_YOUREOPER), me.name, source_p->name);
	sendto_one_notice(source_p, ":*** Oper privilege set is %s", oper_p->privset->name);
	sendto_one_notice(source_p, ":*** Oper privs are %s", oper_p->privset->privs);
	send_oper_motd(source_p);

	return (1);
}

/*
 * find_umode_slot
 *
 * inputs       - NONE
 * outputs      - an available umode bitmask or
 *                0 if no umodes are available
 * side effects - NONE
 */
unsigned int
find_umode_slot(void)
{
	unsigned int all_umodes = 0, my_umode = 0, i;

	for (i = 0; i < 128; i++)
		all_umodes |= user_modes[i];

	for (my_umode = 1; my_umode && (all_umodes & my_umode);
		my_umode <<= 1);

	return my_umode;
}

void
construct_umodebuf(void)
{
	int i;
	char *ptr = umodebuf;
	static int prev_user_modes[128];

	*ptr = '\0';

	for (i = 0; i < 128; i++)
	{
		if (prev_user_modes[i] != 0 && prev_user_modes[i] != user_modes[i])
		{
			if (user_modes[i] == 0)
			{
				orphaned_umodes |= prev_user_modes[i];
				user_modes[i] = prev_user_modes[i];
			}
			else
				orphaned_umodes &= ~prev_user_modes[i];
		}
		else
			prev_user_modes[i] = user_modes[i];

		if (user_modes[i])
			*ptr++ = (char) i;
	}

	*ptr++ = '\0';
}

void
change_nick_user_host(struct Client *target_p,	const char *nick, const char *user,
		      const char *host, int newts, const char *format, ...)
{
	rb_dlink_node *ptr;
	struct Channel *chptr;
	struct membership *mscptr;
	int changed = irccmp(target_p->name, nick);
	int changed_case = strcmp(target_p->name, nick);
	int do_qjm = irccmp(target_p->username, user) || irccmp(target_p->host, host);
	char mode[10], modeval[NICKLEN * 2 + 2], reason[256], *mptr;
	va_list ap;

	modeval[0] = '\0';

	if(changed)
	{
		target_p->tsinfo = newts;
		monitor_signoff(target_p);
	}
	invalidate_bancache_user(target_p);

	if(do_qjm)
	{
		va_start(ap, format);
		vsnprintf(reason, 255, format, ap);
		va_end(ap);

		sendto_common_channels_local_butone(target_p, NOCAPS, CLICAP_CHGHOST, ":%s!%s@%s QUIT :%s",
				target_p->name, target_p->username, target_p->host,
				reason);

		RB_DLINK_FOREACH(ptr, target_p->user->channel.head)
		{
			mscptr = ptr->data;
			chptr = mscptr->chptr;
			mptr = mode;

			if (is_delayed(mscptr)) continue; // probably auditorium


			if(is_operbiz(mscptr))
			{
				*mptr++ = 'y';
				strcat(modeval, nick);
				strcat(modeval, " ");
			}

			if(is_manager(mscptr))
			{
				*mptr++ = 'q';
				strcat(modeval, nick);
				strcat(modeval, " ");
			}

			if(is_superop(mscptr))
			{
				*mptr++ = 'a';
				strcat(modeval, nick);
				strcat(modeval, " ");
			}

			if(is_chanop(mscptr))
			{
				*mptr++ = 'o';
				strcat(modeval, nick);
				strcat(modeval, " ");
			}

			if(is_halfop(mscptr))
			{
				*mptr++ = 'h';
				strcat(modeval, nick);
				strcat(modeval, " ");
			}

			if(is_voiced(mscptr))
			{
				*mptr++ = 'v';
				strcat(modeval, nick);
			}

			*mptr = '\0';

			sendto_channel_local_with_capability_butone(target_p, ALL_MEMBERS, NOCAPS, CLICAP_EXTENDED_JOIN | CLICAP_CHGHOST, chptr,
								    ":%s!%s@%s JOIN %s", nick, user, host, chptr->chname);
			sendto_channel_local_with_capability_butone(target_p, ALL_MEMBERS, CLICAP_EXTENDED_JOIN, CLICAP_CHGHOST, chptr,
								    ":%s!%s@%s JOIN %s %s :%s", nick, user, host, chptr->chname,
								    EmptyString(target_p->user->suser) ? "*" : target_p->user->suser,
								    target_p->info);

			if(*mode)
				sendto_channel_local_with_capability_butone(target_p, ALL_MEMBERS,
						NOCAPS, CLICAP_CHGHOST, chptr, ":%s MODE %s +%s %s",
						target_p->servptr->name, chptr->chname, mode, modeval);

			*modeval = '\0';
		}

		/* Resend away message to away-notify enabled clients. */
		if (target_p->user->away)
			sendto_common_channels_local_butone(target_p, CLICAP_AWAY_NOTIFY, CLICAP_CHGHOST, ":%s!%s@%s AWAY :%s",
							    nick, user, host,
							    target_p->user->away);

		sendto_common_channels_local_butone(target_p, CLICAP_CHGHOST, NOCAPS,
						    ":%s!%s@%s CHGHOST %s %s",
						    target_p->name, target_p->username, target_p->host, user, host);

		if(MyClient(target_p) && changed_case)
			sendto_one(target_p, ":%s!%s@%s NICK %s",
					target_p->name, user, host, nick);

		/* TODO: send some snotes to SNO_NCHANGE/SNO_CCONN/SNO_CCONNEXT? */
	}
	else if(changed_case)
	{
		sendto_common_channels_local(target_p, NOCAPS, NOCAPS, ":%s!%s@%s NICK :%s",
				target_p->name, user, host, nick);

		if(MyConnect(target_p))
			sendto_realops_snomask(SNO_NCHANGE, L_ALL,
					"Nick change: From %s to %s [%s@%s]",
					target_p->name, nick,
					target_p->username, target_p->host);
	}

	rb_strlcpy(target_p->username, user, sizeof target_p->username);
	rb_strlcpy(target_p->host, host, sizeof target_p->host);

	if (changed)
		add_history(target_p, 1);

	del_from_client_hash(target_p->name, target_p);
	rb_strlcpy(target_p->name, nick, NICKLEN);
	add_to_client_hash(target_p->name, target_p);

	if(changed)
	{
		monitor_signon(target_p);
		del_all_accepts(target_p);
	}
}
