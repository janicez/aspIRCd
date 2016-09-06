/*
 * +W usermode: Displays if user has done a WHOIS request on you.
 * derived from sno_whois.c.
 *
 *
 */

#include "stdinc.h"
#include "modules.h"
#include "hook.h"
#include "client.h"
#include "ircd.h"
#include "send.h"
#include "s_conf.h"
#include "s_user.h"
#include "s_serv.h"
#include "numeric.h"

void show_whois(hook_data_client *);

mapi_hfn_list_av1 whois_hfnlist[] = {
    {"doing_whois", (hookfn) show_whois},
    {"doing_whois_global", (hookfn) show_whois},
    {NULL, NULL}
};

static int
init(void)
{
    user_modes['W'] = find_umode_slot();

    return 0;
}

static void
fini(void)
{
    user_modes['W'] = find_umode_slot();
}

DECLARE_MODULE_AV1(show_whois, init, fini, NULL, NULL, whois_hfnlist, "$Revision: 1 $");

void
show_whois(hook_data_client *data)
{
    struct Client *source_p = data->client;
    struct Client *target_p = data->target;

    if (MyClient(target_p) &&
        (source_p != target_p) &&
        (target_p->umodes & user_modes['W'])) {
        sendto_one_notice(target_p,
                          ":*** Notice -- %s (%s@%s) is doing a whois on you",
                          source_p->name,
                          source_p->username, source_p->host);
    }
}

