/*
 * ircd-ratbox: A slightly useful ircd.
 * umode_commchancallerid.c: usermode of soft caller ID
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
#include "client.h"
#include "channel.h"
#include "s_user.h"
#include "ircd.h"
#include "send.h"

int _modinit(void) {
	user_modes['G'] = find_umode_slot();
	user_mode_names['G'] = "softcallerid";
	construct_umodebuf();
	return 0;
}

void _moddeinit(void) {
	user_modes['G'] = 0;
	user_mode_names['G'] = NULL;
	construct_umodebuf();
}

static int commchancheck (struct Client *source_p, struct Client *target_p) {
	struct membership *msptr; struct Channel *chptr;
	rb_dlink_node *ptr;
	RB_DLINK_FOREACH(ptr, target_p->user->channel.head)
	{
		msptr = ptr->data; chptr = msptr->chptr;
		if (IsMember(source_p, chptr)) return 1;
	}
	// exhausted our search, callerid
	return 0;
}

void
checkthatuseriscomm(void *vdata) {
	hook_data_privmsg_user *Hd = (hook_data_privmsg_user*)vdata;

	if (Hd->target_p->umodes & user_modes['G']) {
		if (commchancheck(Hd->source_p, Hd->target_p) == 0)
		{
			Hd->approved = UMODE_CALLERID;
			Hd->whynot.mode = "G";
			Hd->whynot.umode = "server-side caller identification with the exception of common channels";
			Hd->whynot.targnotify = "Chances are that the user doesn't want messages from people they don't know and are not in common channels with. If they want your messages, they should /accept you soon.";
		}
	}
}

mapi_hfn_list_av1 commchancallerid_hfnlist[] = {
	{"privmsg_user", (hookfn)checkthatuseriscomm},
	{NULL, NULL}
};

DECLARE_MODULE_AV1(umode_commchancallerid, _modinit, _moddeinit, NULL, NULL, commchancallerid_hfnlist, "$Revision: 498 $");
