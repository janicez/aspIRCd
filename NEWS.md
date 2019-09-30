Copyright (C) 2018 - DavidFranklin (as ircd-nine)
Copyright (C) 2019 - DavidFranklin (as aspIRCd)

##aspIRCd 5.2.0

Hi folks, this is AspIRCd 5.2.0-

- For now, +P has been moved to +r in line with Rizon's use of +z as both "channel is registered" and "channel is permanent" - charybdis had no "channel is registered" mode so it was overloaded on +P to make it so that https://github.com/aspircd/shale-services did what DALNet users would think of as "set channel registered" when it was told to set +r on channels when they were registered. 
- The idea is if a channel is CMODE_PERM then it will be less taxing on
Services to enforce modes.
Charybdis' +z is now +u, think "auditorium."
 
- +D has been added (which temporarily had caused a bug where CHFL_DELAY users (status mode not synced) were conflated with voiced users in listings, meaning voiced users were listed as delayed forever) 
- to complement +u, such that +mDu results in a modified version of Unreal's "weird +mu sending behaviour" (the Unreal behaviour was to send from a user called "IRC", implemented to support the horribly broken WebTV IRC client and repurposed to be the source for messages sent in moderated auditoria. We just send it from the user, without joining, to ops only.) Voice a user to unhide them if you are doing +Dmu.

- +L, allowing longer ban lists, has been moved to +X.

- +q, quiet, has been moved to +M. This is because there are 4 new
- status modes (oper business, owner, superop, halfop)
- It was decided that owner would be +q. "Oper business" may be set on any oper by any oper regardless of whether they are a channel op or not and should not give them channel operator access, merely privilege of place on the nicklist
- Old Charybdis +r, registered, identified and verified users only, has been moved to +R and has the same semantics (Services must have given the user a services username).

- On those systems supporting SCTP (that is: most Linux other than OpenVZ, and FreeBSD. It is unknown whether or not illumos supports SCTP, however it's assumed that it doesn't.) this IRCd may take advantage of SCTP as an available transport for listening. 
- Most IRC clients do not support SCTP; however, they exist. Other than SCTP connections being more difficult to detect and block, SCTP covers no advantage over TCP. This does not increase your attack surface compared to a TCP-only IRCd. The example configuration dictates that some ports shall be listened to on SCTP as well as
- TCP. It is safe to leave these in the default position; however,
- if you do not desire to do so, you may disable them. If a hub server you are linking to leaves SCTP open, if your system is capable of SCTP, you will still be able to connect to said hub even
- if you do not listen on any SCTP ports. SCTP without SSL is no more secure than TCP without SSL. It is recommended that you use SSL on
- all server to server links and that you ensure that SSL usage is rewarded on your IRC network. Users connecting via SCTP will gain a usermode (+T).

- TL;DR? listen::sctpport and listen::sctpsslport may either be left in the example positions or turned off, as desired. For 99% of
- IRC users, your decision will have no effect and will go unnoticed.
- If you desire to manipulate them, they may be manipulated the same
 way as listen::port and listen::sslport may

- Chosen charybdis 3.5.5 as the base project and made it the same functionality as AspIRCd 5.0.9-5.1.1
-- Added ownermode, admin mode, halfop mode (+q, +a , +h)
-- Added umode +h (Helpop, sets swhois "is available for help" Unreal like)
-- TLS 1.2
-- added the ability to show SSL ciphers on /whois 
- Added hidebans and changed ownermode bot to +U
- Added +B for botmode
- Various bug fixes.
Note: We have removed some usermodes like +m +t which are useless. We might add some more in the next pre-release.

##aspIRCd 5.0.9
After including the updates from aspIRCd 5.0.7 RC (Development Version) into the Final Release of 5.0.9, aspIRCd ships you:
- Added mbedTLS support, the most secured TLS base in the IRC industry.
- Updated sqlite version to sqlite3.12.0 - the latest version.
- Added support for IRCv3.2 user host in names.
- Re-enabled away-notify feature.
- When a user is killed, it no longer echoes the name of the IRC Operator who killed the user.
- Added new user mode for Web Chat users, +W that displays a swhois.
- Fixed egdpool compile error.
- Added all-new **SASL** , taken from Charybdis 3.6.
- Added all-new IRCv3.2  starttls, taken from Charybdis 3.6
- Added ability to CertFP hash
- SSLD: Fixed compile error
- Removed m_force.c and added 4 new modules, **/forcejoin /forcenick /forcepart /forcequit**, which work over remote servers!
- Made ownermode to be +q instead of +w
- Made quiet mode to be +y.
- Fixed /rehash ssld "Unknown Command" - Error.
- Fixed /stats S "Unknown Command" - Error.

This was the biggest leap, aspIRCd ever took, in , considering this release to be named as 5.0.9 *Final Release*.

##aspIRCd 5.0.7 RC1
aspIRCd has jumped to the next level, 5.0.7-RC1 (Release Candidate) \/
**NOTE**: This is a Release Candidate, you may or may not spot bugs. If you do, report it to us at our Developmental Server.
- Upon realising, that aspIRCd didn't have got mbedTLS , we have finally added support for mbedTLS in aspIRCd (The most secured TLS base).
- Updated sqlite to sqlite3.12.0 for better functioning and to prevent memory leaks.
- **cap-notify** : fixed possible crashes on 64-bit systems.
- Removed **RB_PRNG_EGD** to limit down crashes and it wasn't used and/or needed by any users.
- **src/channel** : added support for IRCv3.2 userhost-in-names.
- Reinitialised and removed **away-notify** , as it wasn't needed.
- **m_kill**: No longer notify public users the OPER that killed the user.
- Added support for usermode +W and displays swhois "is using a Web-Based client." as a usermode.
- Finally fixed the swhois compile error.
- SCTP now functions properly.

##aspIRCd 5.0.6
The aspIRCd 5.0.6-RC1 brings you a variety of features as listed below:
**NOTE:** This is a Release Candidate-1 for aspIRCd's 5.0.6 version. You may report any bugs , if found (any). either /at our Development Server or /at our GitHub by opening a new Issue.
- Added TLS capability for SSL connections. We are using IRCv3.1 TLS this time.
- For the same, we had to add m_starttls module which is added in the initial Makefile, it will compile.
- For this update, we are keeping mbedTLS, as it is arm based iirc and our starttls module (IRCv3.1 TLS) will depend upon it to run in background.
- We added the command /dehelper for IRC Operators who don't have much time to run `/mode (nick) -h` to disappear in /stats p , probabaly when they are going away, the /dehelper command extends to modding your and as well as some other person's nick **-h** to disappear as helpop.
- Fixed a small bug, in messages.tab corrected it from `/msg ChanServ help mlock` to `/msg ChanServ help set mlock` 
- added all new SASL authentication, which features upto the features of IRCv3.2 SASL.
- We **haven't** updated IRCv3.2's chghost , so you may find it to be obslete at the moment. We will get it worth to you next update.
- Fixed **stringops.h** `include/inline/stringops.h` as it could not print the string on high bit bytes, as the script as from initial was coded so as to break, whenever high bit bytes were supplied.
- With all new SASL, we have fixed the compile error on Debian 8 , which extends to new features and extendability.

##aspIRCd 5.0.5
- Improved scalability of the daemon.
- Various Bug Fixes.
- Made Channel Owner Registrationship Mode: +w
- Made quiet mode as +q
- Improved configuration blocks.
- Added /svsjoin and /forcejoin command to forcejoin a user to a channel.
- Updated snomasks to latest charybdis release.
- Changed Client connection regex as  as per the requirements.


