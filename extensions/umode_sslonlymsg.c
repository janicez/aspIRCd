/*
 * ircd-ratbox: A slightly useful ircd.
 * umode_sslonlymsg.c: usermode to require SSL for messaging
 * based on
 *  spy_admin_notice.c: Sends a notice when someone uses ADMIN.
 *
 * Copyright (C) 2002 by the past and present ircd coders, and others.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 * USA
 *
 *  $Id: spy_admin_notice.c 498 2006-01-15 16:40:33Z jilles $
 */
#include "stdinc.h"
#include "modules.h"
#include "hook.h"
#include "s_user.h"
#include "client.h"
#include "ircd.h"
#include "send.h"

int _modinit(void) {
	user_modes['t'] = find_umode_slot();
	user_mode_names['t'] = "sslonlymsg";
	construct_umodebuf();
	return 0;
}

void _moddeinit(void) {
	user_modes['t'] = 0;
	user_mode_names['t'] = NULL;
	construct_umodebuf();
}

void
checkthatuserisssl(void *vdata) {
	hook_data_privmsg_user *Hd = (hook_data_privmsg_user*)vdata;

	if (Hd->target_p->umodes & user_modes['t']) {
		if (!IsSSLClient(Hd->source_p))
		{
			Hd->approved = UMODE_CALLERID;
			Hd->whynot.mode = "t";
			Hd->whynot.umode = "server-side caller identification unless the source is using SSL";
			Hd->whynot.targnotify = "The user refuses reception of messages from non-SSL users. You should reconnect using SSL rather than wait for an accept. Most clients support it.";
		}
	}
}

mapi_hfn_list_av1 sslonlymsg_hfnlist[] = {
	{"privmsg_user", (hookfn)checkthatuserisssl},
	{NULL, NULL}
};

DECLARE_MODULE_AV1(umode_sslonlymsg, _modinit, _moddeinit, NULL, NULL, sslonlymsg_hfnlist, "$Revision: 498 $");
