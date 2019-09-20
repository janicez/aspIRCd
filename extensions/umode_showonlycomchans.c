/*
 * how many of these have i made
 * used to require a +IP aware whois.
 */

#include "stdinc.h"
#include "modules.h"
#include "hook.h"
#include "client.h"
#include "ircd.h"
#include "send.h"
#include "hash.h"
#include "s_conf.h"
#include "s_user.h"
#include "s_serv.h"
#include "numeric.h"
#include "privilege.h"
#include "s_newconf.h"
#include "newconf.h"

#define UMODE_SHOWONLYCOMCHANS_MODE_IN_USE 'I'
#define UMODE_SHOWONLYCOMCHANS_EXCEPT_MODE_IN_USE 'P'

static void check_umode_change(void *data);
char *hidechannels_privilege = "oper:hidechannels";
char *hidechannels_override_privilege = "oper:spy";
unsigned int hidechannels_isoperonly = 0;
unsigned int hidechannels_override_isoperonly = 0;

static void
conf_set_hidechannels_hide_privilege(void *data)
{
	hidechannels_privilege = rb_strdup(data);
}

static void
conf_set_hidechannels_override_privilege(void *data)
{
	hidechannels_override_privilege = rb_strdup(data);
}

static void
conf_set_hidechannels_hide_operonly(void *data)
{
	hidechannels_isoperonly = * (unsigned int *) data;
}

static void
conf_set_hidechannels_override_operonly(void *data)
{
	hidechannels_override_isoperonly = * (unsigned int *) data;
}

#define IsChanInvisible(user) ((user)->umodes & user_modes[UMODE_SHOWONLYCOMCHANS_MODE_IN_USE])
#define IsChanInvisibleTo(to, user) ((user)->umodes & user_modes[UMODE_SHOWONLYCOMCHANS_MODE_IN_USE] && !((to)->umodes & user_modes[UMODE_SHOWONLYCOMCHANS_EXCEPT_MODE_IN_USE]))

static void
h_uhuc_doing_whois_channel_visibility(hook_data_client *hdata)
{
	hdata->approved = ((PubChannel(hdata->chptr) && !IsChanInvisibleTo(hdata->client, hdata->target)) || IsMember((hdata->client), (hdata->chptr)));
}

mapi_hfn_list_av1 hidechannels_hfnlist[] = {
	{ "umode_changed", (hookfn) check_umode_change },
	{ "doing_whois_channel_visibility", (hookfn) h_uhuc_doing_whois_channel_visibility },
	{ NULL, NULL }
};

static void
check_umode_change(void *vdata)
{
	hook_data_umode_changed *data = (hook_data_umode_changed *)vdata;
	struct Client *source_p = data->client;

	if (!MyClient(source_p))
		return;

	/* didn't change umode, we don't need to do anything */
	if (!((data->oldumodes ^ source_p->umodes) & user_modes[UMODE_SHOWONLYCOMCHANS_EXCEPT_MODE_IN_USE]) && !((data->oldumodes ^ source_p->umodes) & user_modes[UMODE_SHOWONLYCOMCHANS_MODE_IN_USE]))
		return;

	if (hidechannels_isoperonly != 0 && !((data->oldumodes ^ source_p->umodes) & user_modes[UMODE_SHOWONLYCOMCHANS_MODE_IN_USE]) && !HasPrivilege(source_p, hidechannels_privilege))
	{
		sendto_one_notice(source_p, ":*** You need the privilege %s to be able to hide your channel list.", hidechannels_privilege);
		source_p->umodes &= ~user_modes[UMODE_SHOWONLYCOMCHANS_MODE_IN_USE];
	}

	if (hidechannels_override_isoperonly != 0 && !((data->oldumodes ^ source_p->umodes) & user_modes[UMODE_SHOWONLYCOMCHANS_EXCEPT_MODE_IN_USE]) && !HasPrivilege(source_p, hidechannels_override_privilege))
	{
		sendto_one_notice(source_p, ":*** You need the privilege %s to be able to see the channel lists of +%c users.", hidechannels_override_privilege, UMODE_SHOWONLYCOMCHANS_MODE_IN_USE);
		source_p->umodes &= ~user_modes[UMODE_SHOWONLYCOMCHANS_EXCEPT_MODE_IN_USE];
	}
	return;
}

static int
_modinit(void)
{
	/* add the usermode to the available slot */
	add_top_conf("channelhiding", NULL, NULL, NULL);
	add_conf_item("channelhiding", "hide_operonly", CF_YESNO, conf_set_hidechannels_hide_operonly);
	add_conf_item("channelhiding", "hide_privilege", CF_QSTRING, conf_set_hidechannels_hide_privilege);
	add_conf_item("channelhiding", "override_operonly", CF_YESNO, conf_set_hidechannels_override_operonly);
	add_conf_item("channelhiding", "override_privilege", CF_QSTRING, conf_set_hidechannels_override_privilege);
	user_modes[UMODE_SHOWONLYCOMCHANS_MODE_IN_USE] = find_umode_slot();
	user_mode_names[UMODE_SHOWONLYCOMCHANS_MODE_IN_USE] = "chanhide";
	user_modes[UMODE_SHOWONLYCOMCHANS_EXCEPT_MODE_IN_USE] = find_umode_slot();
	user_mode_names[UMODE_SHOWONLYCOMCHANS_EXCEPT_MODE_IN_USE] = "override_chanhide";
	construct_umodebuf();

	return 0;
}

static void
_moddeinit(void)
{
	/* disable the umode and remove it from the available list */
	user_modes[UMODE_SHOWONLYCOMCHANS_MODE_IN_USE] = 0;
	user_modes[UMODE_SHOWONLYCOMCHANS_EXCEPT_MODE_IN_USE] = 0;
	user_mode_names[UMODE_SHOWONLYCOMCHANS_MODE_IN_USE] = 0;
	user_mode_names[UMODE_SHOWONLYCOMCHANS_EXCEPT_MODE_IN_USE] = 0;
	remove_conf_item("channelhiding", "hide_operonly");
	remove_conf_item("channelhiding", "hide_privilege");
	remove_conf_item("channelhiding", "override_operonly");
	remove_conf_item("channelhiding", "override_privilege");
	remove_top_conf("channelhiding");
	construct_umodebuf();
}

DECLARE_MODULE_AV1(hidechannels, _modinit, _moddeinit, NULL, NULL,
			hidechannels_hfnlist, "$Revision: 3526 $");
