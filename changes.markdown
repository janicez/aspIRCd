# sIRCd Changes

## Changes since Version 1.0.7

 - [X] change who can set selected channel modes
 - [X] remove NOKICKOPER(Channel mode +M) from core and make a module
 - [X] allow self-devoicing (enbaled/disabled via config)
 - [X] moved selected extensions to modules   
 - [X] fix halfop permissions (Allow self-dehalfop)
 - [X] show a client's unique ID on connect & whois
 - [X] add static parts
 - [X] remove m_roleplay
 - [X] add sasl_usercloak
 - [X] added m_forcequit (Allows admins to /forcequit users remote/locally and IRCOP's locally only)
 - [X] added show_whois - usermode +W which will notify you when someone does a /whois on you
 - [X] added a idle time feature for IRCOPs, if Idle for longer than set time they'll be removed from /stats p (defaults: 2hours)
 - [X] added extb_hostmask - Hostmask extban type: bans all users matching a given hostmask
 - [X] reverted ip_cloaking module back to original state
