/* aspIRCd: external include for m_forcejoin.c (2019) -- Idea suggested by TwinUsers.
 *   Copyright (C) - 2019 - DavidFranklin
 *   Copyright (C) - 2019 - Hyphovy Network Development Team.
 *
 *  Note: This is a file of external includes so that they don't confilict with other core modules.
 * When unable to compile modules/core/m_join.c, We thought of creating our own include file for the same.
 * In future, for defining variables, we might use this external.h file for including new variables.
 * This file is totally made by our Development Team, and we reserve the right to remove it anytime ,
 * You may not repoduce this file to suit your network's needs - Reselling of the file is totally restricted.
 * 
 *  FILE START!
 */
#ifndef INCLUDED_external_h
#define INCLUDED_external_h

#ifndef INCLUDED_hash_h
#define INCLUDED_hash_h

extern void do_join_0(struct Client *client_p, struct Client *source_p);
extern int check_channel_name_loc(struct Client *source_p, const char *name);
