/* vi: set sw=4 ts=4: */
/*
 * Mini date implementation for busybox
 *
 * by Matthew Grant <grantma@anathoth.gen.nz>
 *
 * iso-format handling added by Robert Griebl <griebl@gmx.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
*/

#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include "busybox.h"
#include <fcntl.h>
/////////////////////////////////////////hhcn add by it
int fd;
unsigned char SetTime[] = 
        {
                0x00,0x30,0x00,  //second
                0x00,0x31,0x00,  //minute
                0x00,0x32,0x00,  //hour
                0x00,0x33,0x00,  //date
                0x00,0x34,0x00,  //month
                0x00,0x35,0x00,  //year
        };
/////////////////////////////////////////


/* This 'date' command supports only 2 time setting formats,
   all the GNU strftime stuff (its in libc, lets use it),
   setting time using UTC and displaying int, as well as
   an RFC 822 complient date output for shell scripting
   mail commands */

/* Input parsing code is always bulky - used heavy duty libc stuff as
   much as possible, missed out a lot of bounds checking */

/* Default input handling to save suprising some people */

static struct tm *date_conv_time(struct tm *tm_time, const char *t_string)
{
	int nr;

	nr = sscanf(t_string, "%2d%2d%2d%2d%d", &(tm_time->tm_mon),
				&(tm_time->tm_mday), &(tm_time->tm_hour), &(tm_time->tm_min),
				&(tm_time->tm_year));

	if (nr < 4 || nr > 5) {
		bb_error_msg_and_die(bb_msg_invalid_date, t_string);
	}

	/* correct for century  - minor Y2K problem here? */
	if (tm_time->tm_year >= 1900) {
		tm_time->tm_year -= 1900;
	}
	/* adjust date */
	tm_time->tm_mon -= 1;

	return (tm_time);

}


/* The new stuff for LRP */

static struct tm *date_conv_ftime(struct tm *tm_time, const char *t_string)
{
	struct tm t;

	/* Parse input and assign appropriately to tm_time */

	if (t =
		*tm_time, sscanf(t_string, "%d:%d:%d", &t.tm_hour, &t.tm_min,
						 &t.tm_sec) == 3) {
		/* no adjustments needed */
	} else if (t =
			   *tm_time, sscanf(t_string, "%d:%d", &t.tm_hour,
								&t.tm_min) == 2) {
		/* no adjustments needed */
	} else if (t =
			   *tm_time, sscanf(t_string, "%d.%d-%d:%d:%d", &t.tm_mon,
								&t.tm_mday, &t.tm_hour, &t.tm_min,
								&t.tm_sec) == 5) {
		/* Adjust dates from 1-12 to 0-11 */
		t.tm_mon -= 1;
	} else if (t =
			   *tm_time, sscanf(t_string, "%d.%d-%d:%d", &t.tm_mon,
								&t.tm_mday, &t.tm_hour, &t.tm_min) == 4) {
		/* Adjust dates from 1-12 to 0-11 */
		t.tm_mon -= 1;
	} else if (t =
			   *tm_time, sscanf(t_string, "%d.%d.%d-%d:%d:%d", &t.tm_year,
								&t.tm_mon, &t.tm_mday, &t.tm_hour, &t.tm_min,
								&t.tm_sec) == 6) {
		t.tm_year -= 1900;	/* Adjust years */
		t.tm_mon -= 1;	/* Adjust dates from 1-12 to 0-11 */
	} else if (t =
			   *tm_time, sscanf(t_string, "%d.%d.%d-%d:%d", &t.tm_year,
								&t.tm_mon, &t.tm_mday, &t.tm_hour,
								&t.tm_min) == 5) {
		t.tm_year -= 1900;	/* Adjust years */
		t.tm_mon -= 1;	/* Adjust dates from 1-12 to 0-11 */
	} else {
		bb_error_msg_and_die(bb_msg_invalid_date, t_string);
	}
	*tm_time = t;
	return (tm_time);
}

#define DATE_OPT_RFC2822	0x01
#define DATE_OPT_SET    	0x02
#define DATE_OPT_UTC    	0x04
#define DATE_OPT_DATE   	0x08
#define DATE_OPT_REFERENCE	0x10
#ifdef CONFIG_FEATURE_DATE_ISOFMT
# define DATE_OPT_TIMESPEC	0x20
#endif

//HHTECH data.c for busybox
int date_main(int argc, char **argv)
{
	char CurTime[8];
	char *date_str = NULL;
	char *date_fmt = NULL;
	char *t_buff;
	int set_time;
	int utc;
	int use_arg = 0;
	time_t tm;
	unsigned long opt;
	struct tm tm_time;
	char *filename = NULL;

#ifdef CONFIG_FEATURE_DATE_ISOFMT
	int ifmt = 0;
	char *isofmt_arg;

# define GETOPT_ISOFMT  "I::"
#else
# define GETOPT_ISOFMT
#endif
	bb_opt_complementaly = "d~ds:s~ds";
	opt = bb_getopt_ulflags(argc, argv, "Rs:ud:r:" GETOPT_ISOFMT,
					&date_str, &date_str, &filename
#ifdef CONFIG_FEATURE_DATE_ISOFMT
					, &isofmt_arg
#endif
					);
	//printf("opt=%08x\n",opt);
	set_time = opt & DATE_OPT_SET;
	utc = opt & DATE_OPT_UTC;
	if ((utc) && (putenv("TZ=UTC0") != 0)) {
		bb_error_msg_and_die(bb_msg_memory_exhausted);
	}
	use_arg = opt & DATE_OPT_DATE;
	//printf("set_time=%d,utc=%d,use_arg=%d\n",set_time,utc,use_arg);
	if(opt & 0x80000000UL)
		bb_show_usage();
#ifdef CONFIG_FEATURE_DATE_ISOFMT
	if(opt & DATE_OPT_TIMESPEC) {
		if (!isofmt_arg) {
			//printf("!isofmt_arg\n");
			ifmt = 1;
		} else {
			int ifmt_len = bb_strlen(isofmt_arg);
			//printf("bb_strlen(isofmt_arg),ifmt_len=%d\n",ifmt_len);
			if ((ifmt_len <= 4)
				&& (strncmp(isofmt_arg, "date", ifmt_len) == 0)) {
				ifmt = 1;
			} else if ((ifmt_len <= 5)
				   && (strncmp(isofmt_arg, "hours", ifmt_len) == 0)) {
				ifmt = 2;
			} else if ((ifmt_len <= 7)
				   && (strncmp(isofmt_arg, "minutes", ifmt_len) == 0)) {
				ifmt = 3;
			} else if ((ifmt_len <= 7)
				   && (strncmp(isofmt_arg, "seconds", ifmt_len) == 0)) {
				ifmt = 4;
			}
			//printf("ifmt = %d\n",ifmt);
		}
		if (!ifmt) {
			bb_show_usage();
		}
	}
#endif

	if ((date_fmt == NULL) && (optind < argc) && (argv[optind][0] == '+')) {
		date_fmt = &argv[optind][1];	/* Skip over the '+' */
		//printf("Skip over the '+'\n");
	} else if (date_str == NULL) {
		set_time = 1;
		date_str = argv[optind];
		//printf("set_time=1,date_str=%s\n",date_str);
	}

	/* Now we have parsed all the information except the date format
	   which depends on whether the clock is being set or read */

	if(filename) {
		struct stat statbuf;
		//printf("filename=%s\n",filename);
		if(stat(filename,&statbuf))
			bb_perror_msg_and_die("File '%s' not found.\n",filename);
		tm=statbuf.st_mtime;
	} else {
		time(&tm);
		//printf("get time for tm\n");
	}
	
	memcpy(&tm_time, localtime(&tm), sizeof(tm_time));
	/* Zero out fields - take her back to midnight! */
	
	////////////////////////////////////////////////hhcn add by it
	//get time
	fd = open("/dev/X1227/rtc",O_RDWR);
	read(fd,CurTime,6);
	CurTime[2] &= 0x7f;
	tm_time.tm_sec = CurTime[0]/16*10+ CurTime[0]%16;
	tm_time.tm_min = CurTime[1]/16*10+ CurTime[1]%16;
       tm_time.tm_hour = CurTime[2]/16*10+ CurTime[2]%16;
       tm_time.tm_mday = CurTime[3]/16*10+ CurTime[3]%16;
       tm_time.tm_mon = CurTime[4]/16*10+ CurTime[4]%16;
       tm_time.tm_year = CurTime[5]/16*10+ CurTime[5]%16;
	//////////////////////////////////////////////////////////////
	
	//printf("Zero out fields,date_str=%d\n",date_str);
	if (date_str != NULL) {
		//printf("date_str=%s\n",date_str);
		tm_time.tm_sec = 0;
		tm_time.tm_min = 0;
		tm_time.tm_hour = 0;

		/* Process any date input to UNIX time since 1 Jan 1970 */
		if (strchr(date_str, ':') != NULL) {
			//printf("date_conv_ftime\n");
			date_conv_ftime(&tm_time, date_str);
		} else {
			//printf("date_conv_time\n");
			date_conv_time(&tm_time, date_str);
		}

		///////////////////////////////////////////////////hhcn add by it
		//set time
                SetTime[2] = tm_time.tm_sec/10*16+tm_time.tm_sec%10;  //second
                SetTime[5] = tm_time.tm_min/10*16+tm_time.tm_min%10;  //minute
                SetTime[8] = tm_time.tm_hour/10*16+tm_time.tm_hour%10;  //hour
                SetTime[8] |= 0x80;
		  SetTime[11] = tm_time.tm_mday/10*16+tm_time.tm_mday%10;  //date
                SetTime[14] = tm_time.tm_mon/10*16+tm_time.tm_mon%10;  //month
                SetTime[17] = tm_time.tm_year/10*16+tm_time.tm_year%10;  //year
       	  write(fd,SetTime,18);
		//////////////////////////////////////////////////////
		
		/* Correct any day of week and day of year etc. fields */
		tm_time.tm_isdst = -1;	/* Be sure to recheck dst. */
		tm = mktime(&tm_time);
		if (tm < 0) {
			bb_error_msg_and_die(bb_msg_invalid_date, date_str);
		}
		if (utc && (putenv("TZ=UTC0") != 0)) {
			bb_error_msg_and_die(bb_msg_memory_exhausted);
		}

		/* if setting time, set it */
		if (set_time && (stime(&tm) < 0)) {
			bb_perror_msg("cannot set date");
		}
	}

	///////////////////////////////////////////////////hhcn add by it
	close(fd);
	//////////////////////////////////////////////////////////////
	/* Display output */

	/* Deal with format string */
	if (date_fmt == NULL) {
#ifdef CONFIG_FEATURE_DATE_ISOFMT
		switch (ifmt) {
		case 4:
			date_fmt = utc ? "%Y-%m-%dT%H:%M:%SZ" : "%Y-%m-%dT%H:%M:%S%z";
			break;
		case 3:
			date_fmt = utc ? "%Y-%m-%dT%H:%MZ" : "%Y-%m-%dT%H:%M%z";
			break;
		case 2:
			date_fmt = utc ? "%Y-%m-%dT%HZ" : "%Y-%m-%dT%H%z";
			break;
		case 1:
			date_fmt = "%Y-%m-%d";
			break;
		case 0:
		default:
#endif
			date_fmt = (opt & DATE_OPT_RFC2822 ?
					(utc ? "%a, %d %b %Y %H:%M:%S GMT" :
					"%a, %d %b %Y %H:%M:%S %z") :
					"%a %b %e %H:%M:%S %Z %Y");

#ifdef CONFIG_FEATURE_DATE_ISOFMT
			break;
		}
#endif
	} else if (*date_fmt == '\0') {
		/* Imitate what GNU 'date' does with NO format string! */
		printf("\n");
		return EXIT_SUCCESS;
	}

	/* Handle special conversions */

	if (strncmp(date_fmt, "%f", 2) == 0) {
		date_fmt = "%Y.%m.%d-%H:%M:%S";
	}

	/* Print OUTPUT (after ALL that!) */
	t_buff = xmalloc(201);
	strftime(t_buff, 200, date_fmt, &tm_time);
	puts(t_buff);

	return EXIT_SUCCESS;
}
