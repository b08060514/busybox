/*
 * ip.c		"ip" utility frontend.
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
 *
 * Authors:	Alexey Kuznetsov, <kuznet@ms2.inr.ac.ru>
 *
 *
 * Changes:
 *
 * Rani Assaf <rani@magic.metawire.com> 980929:	resolve addresses
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#include "./libiproute/utils.h"
#include "./libiproute/ip_common.h"

#include "busybox.h"

int preferred_family = AF_UNSPEC;
int oneline = 0;
char * _SL_ = NULL;

int ip_main(int argc, char **argv)
{
	char *basename;

	basename = strrchr(argv[0], '/');
	if (basename == NULL)
		basename = argv[0];
	else
		basename++;
	
	while (argc > 1) {
		char *opt = argv[1];
		if (strcmp(opt,"--") == 0) {
			argc--; argv++;
			break;
		}
		if (opt[0] != '-')
			break;
		if (opt[1] == '-')
			opt++;
		if (matches(opt, "-family") == 0) {
			argc--;
			argv++;
			if (strcmp(argv[1], "inet") == 0)
				preferred_family = AF_INET;
			else if (strcmp(argv[1], "inet6") == 0)
				preferred_family = AF_INET6;
			else if (strcmp(argv[1], "link") == 0)
				preferred_family = AF_PACKET;
			else
				invarg(argv[1], "invalid protocol family");
		} else if (strcmp(opt, "-4") == 0) {
			preferred_family = AF_INET;
		} else if (strcmp(opt, "-6") == 0) {
			preferred_family = AF_INET6;
		} else if (strcmp(opt, "-0") == 0) {
			preferred_family = AF_PACKET;
		} else if (matches(opt, "-oneline") == 0) {
			++oneline;
		} else {
			fprintf(stderr, "Option \"%s\" is unknown, try \"ip -help\".\n", opt);
			exit(-1);
		}
		argc--;	argv++;
	}

	_SL_ = oneline ? "\\" : "\n" ;

	if (argc > 1) {
#ifdef CONFIG_FEATURE_IP_ADDRESS
		if (matches(argv[1], "address") == 0)
			return do_ipaddr(argc-2, argv+2);
#endif
#ifdef CONFIG_FEATURE_IP_ROUTE
		if (matches(argv[1], "route") == 0)
			return do_iproute(argc-2, argv+2);
#endif
#ifdef CONFIG_FEATURE_IP_LINK
		if (matches(argv[1], "link") == 0)
			return do_iplink(argc-2, argv+2);
#endif
#ifdef CONFIG_FEATURE_IP_TUNNEL
		if (matches(argv[1], "tunnel") == 0 ||
		    strcmp(argv[1], "tunl") == 0)
			return do_iptunnel(argc-2, argv+2);
#endif
		fprintf(stderr, "Object \"%s\" is unknown, try \"ip help\".\n", argv[1]);
		exit(-1);
	}
}
