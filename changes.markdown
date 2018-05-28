# sIRCd Changes

## Changes since Version 1.0.9-1 (fear)

- [X] - added usermode +I which will hide idle from /whois output for non oper accounts
- [X] - reinstated configuration option for the nick of the SASL agent
- [X] - moved ip_cloaking to modules and renamed to m_cloak. Automatically loaded now
- [X] - fix gnutls error with version 3+
- [X] - update openSSL 
- [X] - removed unused cloaking
- [X] - show users the RESV reason
- [X] - purged local channels (not really used)
- [X] - hard coded chm_sslonly into the core
- [X] - fixed extb_channel bug where if banned target channel has +s or +p set it wouldn't work
- [X] - Use NAMELEN instead of NICKLEN when accessing Client->name because it could in theory be a HOSTLEN length string.
- [X] - added usermode +O which will hide oper from /whois for non oper accounts
