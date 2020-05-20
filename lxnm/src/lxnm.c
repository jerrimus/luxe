/**
 * Copyright (c) 2008 LxDE Developers, see the file AUTHORS for details.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <glib.h>

#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

#include "lxnm.h"

static LxND *lxnm;

static const char *protocol_name[] = {
	"NONE",
	"WEP",
	"WPA",
};

static const char *cypher_name[] = {
	"NONE",
	"WEP40",
	"TKIP",
	"WRAP",
	"CCMP",
	"WEP104",
};

static const char *key_mgmt_name[] = {
	"NONE",
	"IEEE8021X",
	"WPA-PSK",
};

static char*
hex2asc(char *hexsrc)
{
	char *buf, *tmp;
	char c[3];

	buf = malloc(sizeof(char)+strlen(hexsrc)/2);
	tmp = buf;

	for (;*hexsrc!='\0';hexsrc+=2) {
		c[0] = *hexsrc;
		c[1] = *(hexsrc+1);
		c[2] = '\0';

		*tmp = strtol(c, NULL, 16);
		tmp++;
	}

	*tmp = '\0';
	return buf;
}

static gboolean
lxnm_isifname(const char *ifname)
{
	struct ifreq ifr;
	bzero(&ifr, sizeof(ifr));

	strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));
	if (ioctl(lxnm->sockfd, SIOCGIFFLAGS, &ifr)<0)
		return FALSE;

	return TRUE;
}

static int
CommandProcess(void *arg)
{
	return system((char *)arg);
}

static int
ethernet_up(void *arg)
{
	char *p;
	/* interface name */
	p = strtok((char *)arg, " ");
	if (lxnm_isifname(p)) {
		setenv("LXNM_IFNAME", p, 1);
		return system(lxnm->setting->eth_up);
	}
}

static int
ethernet_down(void *arg)
{
	char *p;
	/* interface name */
	p = strtok((char *)arg, " ");
	if (lxnm_isifname(p)) {
		setenv("LXNM_IFNAME", p, 1);
		return system(lxnm->setting->eth_down);
	}
}

static int
ethernet_repair(void *arg)
{
	char *p;
	/* interface name */
	p = strtok((char *)arg, " ");
	if (lxnm_isifname(p)) {
		setenv("LXNM_IFNAME", p, 1);
		return system(lxnm->setting->eth_repair);
	}
}

static int
wireless_up(void *arg)
{
	char *p;
	/* interface name */
	p = strtok((char *)arg, " ");
	if (lxnm_isifname(p)) {
		setenv("LXNM_IFNAME", p, 1);
		return system(lxnm->setting->wifi_up);
	}
}

static int
wireless_down(void *arg)
{
	char *p;
	/* interface name */
	p = strtok((char *)arg, " ");
	if (lxnm_isifname(p)) {
		setenv("LXNM_IFNAME", p, 1);
		return system(lxnm->setting->wifi_down);
	}
}

static int
wireless_repair(void *arg)
{
	char *p;
	/* interface name */
	p = strtok((char *)arg, " ");
	if (lxnm_isifname(p)) {
		setenv("LXNM_IFNAME", p, 1);
		return system(lxnm->setting->wifi_repair);
	}
}

static int
wireless_connect(void *arg)
{
	char *p;
	/* <interface> <essid> <apaddr> <key> <protocol> <key_mgmt> <grpup> <pairwise> */
	/* interface name */
	p = strtok((char *)arg, " ");
	if (lxnm_isifname(p)) {
		setenv("LXNM_IFNAME", p, 1);
		/* ESSID */
		p = strtok(NULL, " ");
		setenv("LXNM_WIFI_ESSID", hex2asc(p), 1);
		/* AP Addr */
		p = strtok(NULL, " ");
		setenv("LXNM_WIFI_APADDR", p, 1);
		/* key */
		p = strtok(NULL, " ");
		setenv("LXNM_WIFI_KEY", hex2asc(p), 1);
		/* protocol */
		p = strtok(NULL, " ");
		setenv("LXNM_WIFI_PROTO", protocol_name[atoi(p)], 1);
		/* key_mgmt */
		p = strtok(NULL, " ");
		setenv("LXNM_WIFI_KEYMGMT", key_mgmt_name[atoi(p)], 1);
		/* group */
		p = strtok(NULL, " ");
		setenv("LXNM_WIFI_GROUP", cypher_name[atoi(p)], 1);
		/* pairwise */
		p = strtok(NULL, " ");
		setenv("LXNM_WIFI_PAIRWISE", cypher_name[atoi(p)], 1);

		return system(lxnm->setting->wifi_connect);
	}
}

static void
lxnm_parse_command(GIOChannel *gio, const char *cmd)
{
	char *p, *cmdstr;
	int i, command;
    pthread_t actionThread;

	cmdstr = g_strdup(cmd);
	/* Command */
	p = strtok((char *)cmd, " ");
	command = atoi(p);
	switch(command) {
		case LXNM_VERSION:
		case LXNM_ETHERNET_UP:
			pthread_create(&actionThread, NULL, ethernet_up, (void *)cmdstr+2);
			break;
		case LXNM_ETHERNET_DOWN:
			pthread_create(&actionThread, NULL, ethernet_down, (void *)cmdstr+2);
			break;
		case LXNM_ETHERNET_REPAIR:
			pthread_create(&actionThread, NULL, ethernet_repair, (void *)cmdstr+2);
			break;
		case LXNM_WIRELESS_UP:
			pthread_create(&actionThread, NULL, wireless_up, (void *)cmdstr+2);
			break;
		case LXNM_WIRELESS_DOWN:
			pthread_create(&actionThread, NULL, wireless_down, (void *)cmdstr+2);
			break;
		case LXNM_WIRELESS_REPAIR:
			pthread_create(&actionThread, NULL, wireless_repair, (void *)cmdstr+2);
			break;
		case LXNM_WIRELESS_CONNECT:
			pthread_create(&actionThread, NULL, wireless_connect, (void *)cmdstr+2);
			break;
		default:
			printf("Unknown command");
	}

	/* Args */
/*
	p = strtok(NULL, " ");
	for (i=0;p;i++,p=strtok(NULL, " ")) {
		printf("%s\n", p);
	}
*/
}

static gboolean
lxnm_read_channel(GIOChannel *gio, GIOCondition condition, gpointer data)
{
	GIOStatus ret;
	GError *err = NULL;
	gchar *msg;
	gsize len;
	gsize term;
//	int cmd;

	ret = g_io_channel_read_line(gio, &msg, &len, &term, &err);
	if (ret == G_IO_STATUS_ERROR)
		g_error("Error reading: %s\n", err->message);


	if (len > 0) {
//		cmd = (int)*msg;
		msg[term] = '\0';
//		printf("Command: %d\n", cmd);
		lxnm_parse_command(gio, msg);
	}
	g_free(msg);

	if (condition & G_IO_HUP)
		return FALSE;

	return TRUE;
}

static gboolean
lxnm_accept_client(GIOChannel *source, GIOCondition condition, gpointer data G_GNUC_UNUSED)
{
	if (condition & G_IO_IN) {
		GIOChannel *gio;
		int fd;
		int flags;

		/* new connection */
		fd = accept(g_io_channel_unix_get_fd(source), NULL, NULL);
		if (fd < 0)
			g_error("Accept failed: %s\n", g_strerror(errno));

		flags = fcntl(fd, F_GETFL, 0);
		fcntl(fd, F_SETFL, flags | O_NONBLOCK);

		gio = g_io_channel_unix_new(fd);
		if (!gio)
			g_error("Cannot create new GIOChannel!\n");

		g_io_channel_set_encoding(gio, NULL, NULL);

		g_io_add_watch(gio, G_IO_IN | G_IO_HUP, lxnm_read_channel, NULL);

		g_io_channel_unref(gio);
	}

	/* our listener socket hung up - we are dead */
	if (condition & G_IO_HUP)
		g_error("Server listening socket died!\n");

	return TRUE;
}

static void
lxnm_init_socket()
{
	struct sockaddr_un skaddr;
	GIOChannel *gio;
	int skfd;

	/* create socket */
	skfd = socket(PF_UNIX, SOCK_STREAM, 0);
	if (skfd < 0)
		g_error("Cannot create socket!");

	/* Initiate socket */
	unlink(LXNM_SOCKET);
	bzero(&skaddr, sizeof(skaddr));

	/* setting UNIX socket */
	skaddr.sun_family = AF_UNIX;
	snprintf(skaddr.sun_path, sizeof(skaddr.sun_path), LXNM_SOCKET);

	/* bind to socket */
	if (bind(skfd, (struct sockaddr *)&skaddr, sizeof(skaddr)) < 0)
		g_error("Bind on socket failed: %s\n", g_strerror(errno));

	/* listen on socket */
	if (listen(skfd, 5) < 0)
		g_error("Listen on socket failed: %s\n", g_strerror(errno));

	/* owner and permision */
	if (chown(LXNM_SOCKET, 0, 0) < 0)
		g_error("Change LXNM_SOCKET owner failed: %s\n", g_strerror(errno));
	if (chmod(LXNM_SOCKET, 0666) < 0)
		g_error("Change LXNM_SOCKET permision failed: %s\n", g_strerror(errno));

	/* create I/O channel */
	gio = g_io_channel_unix_new(skfd);
	if (!gio)
		g_error("Cannot create new GIOChannel!\n");

	/* setting encoding */
	g_io_channel_set_encoding(gio, NULL, NULL);
	g_io_channel_set_buffered(gio, FALSE);
	g_io_channel_set_close_on_unref(gio, TRUE);

	/* I/O channel into the main event loop */
	if (!g_io_add_watch(gio, G_IO_IN | G_IO_HUP, lxnm_accept_client, NULL))
		g_error("Cannot add watch on GIOChannel\n");

	/* channel will automatically shutdown when the watch returns FALSE */
	g_io_channel_set_close_on_unref(gio, TRUE);
	g_io_channel_unref(gio);
}

int
main(void)
{
	GKeyFile *keyfile;
	GKeyFileFlags flags;
	GError *error = NULL;
	pid_t pid;

	/* Run daemon in the background */
	pid = fork();
	if (pid>0) {
		return 0;
	}

	/* initiate socket for network device */
	lxnm = (LxND *)malloc(sizeof(lxnm));
	lxnm->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (lxnm->sockfd < 0) {
		g_error("Cannot create socket!");
		return -1;
	}

	/* initiate key_file */
	keyfile = g_key_file_new();
	flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

	/* Load config */
	if (!g_key_file_load_from_file (keyfile, PACKAGE_DATA_DIR "/lxnm/lxnm.conf", flags, &error)) {
		g_error ("[conf-file] %s", error->message);
		return -1;
	}


	lxnm->setting = (Setting *)malloc(sizeof(Setting));

	/* ethernet setting */
	lxnm->setting->eth_up = g_key_file_get_string(keyfile, "ethernet", "up", NULL);
	lxnm->setting->eth_down = g_key_file_get_string(keyfile, "ethernet", "down", NULL);
	lxnm->setting->eth_repair = g_key_file_get_string(keyfile, "ethernet", "repair", NULL);

	/* wireless setting */
	lxnm->setting->wifi_up = g_key_file_get_string(keyfile, "wireless", "up", NULL);
	lxnm->setting->wifi_down = g_key_file_get_string(keyfile, "wireless", "down", NULL);
	lxnm->setting->wifi_repair = g_key_file_get_string(keyfile, "wireless", "repair", NULL);
	lxnm->setting->wifi_connect = g_key_file_get_string(keyfile, "wireless", "connect", NULL);

	/* LXNM main loop */
	{
		GMainLoop *loop = g_main_loop_new(NULL, FALSE);
		lxnm_init_socket();
		g_main_loop_run(loop); /* Wheee! */
	}

	close(lxnm->sockfd);
	return 0;
}

