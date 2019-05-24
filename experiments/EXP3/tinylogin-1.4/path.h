/*
 * define the system file's path 
 * */
#ifndef _PAHT_H_
#ifdef _FLATFSD_
#define GROUP_FILE      "/etc/group"
#define SHADOW_FILE		"/etc/gshadow"
#define PASSWD_FILE     "/etc/passwd"
#define SHADOW_FILE		"/etc/shadow"
#define CONFIG_FILE "/etc/tinylogin.conf"
#define PASSWD_FILE     "/etc/passwd"
#define GROUP_FILE      "/etc/group"
#define SHADOW_FILE		"/etc/shadow"
#define GSHADOW_FILE	"/etc/gshadow"
#define	ISSUE "/etc/issue"		/* displayed before the login prompt */
#define MOTD_FILE     "/etc/motd"
#define NOLOGIN_FILE  "/etc/nologin"
#define SECURETTY_FILE "/etc/securetty"
#else
#endif
#endif
