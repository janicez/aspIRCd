/*
 * Hostmask extban type: bans all users matching a given hostmask, used for stacked extbans
 * -- kaniini
 */

#include "stdinc.h"
#include "modules.h"
#include "client.h"
#include "ircd.h"

static const char extb_desc[] = "Hostmask ($h) extban type";

static int _modinit(void);
static void _moddeinit(void);
static int eb_hostmask(const char *data, struct Client *client_p, struct Channel *chptr, long mode_type);

DECLARE_MODULE_AV1(extb_hostmask, _modinit, _moddeinit, NULL, NULL, NULL, "1.0.1");

static int
_modinit(void)
{
    extban_table['h'] = eb_hostmask;
    return 0;
}

static void
_moddeinit(void)
{
    extban_table['h'] = NULL;
}

static int
eb_hostmask(const char *banstr, struct Client *client_p, struct Channel *chptr, long mode_type)
{
    char src_host[NICKLEN + USERLEN + HOSTLEN + 6];
    char src_iphost[NICKLEN + USERLEN + HOSTLEN + 6];
    char src_althost[NICKLEN + USERLEN + HOSTLEN + 6];
    char *s = src_host, *s2 = src_iphost, *s3 = NULL, *s4 = NULL;

    sprintf(src_host, "%s!%s@%s", client_p->name, client_p->username, client_p->host);
    sprintf(src_iphost, "%s!%s@%s", client_p->name, client_p->username, client_p->sockhost);

    /* handle hostmangling if necessary */
    if (client_p->localClient->mangledhost != NULL) {
        if (!strcmp(client_p->host, client_p->localClient->mangledhost))
            sprintf(src_althost, "%s!%s@%s", client_p->name, client_p->username, client_p->orighost);
        else if (!IsDynSpoof(client_p))
            sprintf(src_althost, "%s!%s@%s", client_p->name, client_p->username, client_p->localClient->mangledhost);

        s3 = src_althost;
    }

    return match(banstr, s) || match(banstr, s2) || (s3 != NULL && match(banstr, s3)) || (s4 != NULL && match(banstr, s4)) ? EXTBAN_MATCH : EXTBAN_NOMATCH;
}
