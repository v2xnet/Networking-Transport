/***************************************************************************
 *   ITSNET  Intelligent Transport System networking Stack                 *
 * 									   *
 ** Copyright(C)2010 ESPRIT											   *
 * 	        "École supérieure privée d'ingénierie et de technologie"       *
 *                                                                         *
 *   barghich@gmail.com                                                    *
 *   anouar.chemek@gmail.com                                               *
 *  							                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/**
 * @file main.c
 * main code.
 * @author hichem BARGAOUI
 * @author anouar Chemek
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "itsnet.h"
#include "itsnet_parser.h"
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <syslog.h>
#include <unistd.h>

/**
 * Set all the option flags according to the switches specified.
 * @param argc
 * @param argv
 * @return the index of the first non-option argument.
 */

#define MAXPATHL 4096 /* chars in a path */
char cfile[MAXPATHL] = "../extra/itsnet.conf";
char* program_name; /**< The name the program was run with, stripped of any leading path */
pthread_t itsnet;
static struct option const long_options[] = { { "conf", required_argument, 0, 'c' }, { "help", no_argument, 0, 'h' },
    { "version", no_argument, 0, 'V' }, { NULL, 0, NULL, 0 } };

/**
 * Print good usage for command line call.
 * @param status the exit status
 * @return the status it is called with.
 */
static void usage(int status)
{
    printf("%s - C2Cnet implementation\n", program_name);
    printf("Usage: %s [option]... [FILE]...\n", program_name);
    printf("Options:\n\
	        -c <file>                  Read configuration from <file>\n\
	        -h, --help                 Display this help and exit\n\
	        -V, --version              Output version information and exit\n");
    exit(status);
}

static int decode_switches(int argc, char** argv)
{
    int c;

    while((c = getopt_long(argc, argv,
               "c:" /* conf file */
               "h"  /* help      */
               "V", /* version   */
               long_options, (int*)0)) != EOF) {
        switch(c) {
        case 'c': /* --conf */
            strncpy(cfile, optarg, MAXPATHLEN);
            break;

        case 'V': /* --version */
            printf("itsnet %s\n", VERSION);
            exit(0);

        case 'h': /* --help */
            usage(0);

        default:
            usage(EXIT_FAILURE);
        }
    }

    return optind;
}

void sig_child(int unused)
{
    int pid, status;

    while((pid = waitpid(0, &status, WNOHANG)) > 0)
        ;
}

/*
 * Detach from any tty.
 */
/*# FINAL */
static void daemon_start(int ignsigcld)
{
    register int childpid, fd;

    if(getppid() == 1)
        goto out;

#ifdef SIGTTOU
    signal(SIGTTOU, SIG_IGN);
#endif
#ifdef SIGTTIN
    signal(SIGTTIN, SIG_IGN);
#endif
#ifdef SIGTSTP
    signal(SIGTSTP, SIG_IGN);
#endif

    if((childpid = fork()) < 0)
        fprintf(stderr, "can't fork first child\n");
    else if(childpid > 0)
        exit(0);

    if(setpgrp() == -1)
        fprintf(stderr, "can't change process group\n");
    if((fd = open("/dev/tty", O_RDWR)) >= 0) {
        ioctl(fd, TIOCNOTTY, (char*)NULL);
        close(fd);
    }

out:
    for(fd = 0; fd < NOFILE; fd++)
        close(fd);
    errno = 0;

    if(chdir("/tmp") != 0) {
        fprintf(stderr, "can't change dir to /tmp \n");
        return;
    }
    umask(0);

    if(ignsigcld) {
#ifdef SIGTSTP
        signal(SIGCLD, sig_child);
#else
        signal(SIGCLD, SIG_IGN);
#endif
    }
}

void* sigh(void* arg)
{
    int signum;
    sigset_t sigcatch;

    sigemptyset(&sigcatch);
    sigaddset(&sigcatch, SIGHUP);
    sigaddset(&sigcatch, SIGINT);
    sigaddset(&sigcatch, SIGTERM);

    for(;;) {
        sigwait(&sigcatch, &signum);

        switch(signum) {
        case SIGHUP:
            // printf("got SIGHUP, reinitilize\n");
            break;
        case SIGINT:
        case SIGTERM:
            // graceful_exit();
            pthread_cancel(itsnet);
            pthread_exit(NULL);
            // pthread_kill(itsnet,9);
            break;
        default:
            break;
        }
    }
    pthread_exit(NULL);
    exit(0);
}

/**
 * Start Itsnet specific functions.
 * @param argc
 * @param argv
 * @return 0
 */

int main(int argc, char** argv)
{

    pthread_t sigth;
    sigset_t sigblock;

    int i;
    program_name = basename(argv[0]);
    i = decode_switches(argc, argv);

    sigemptyset(&sigblock);
    sigaddset(&sigblock, SIGHUP);
    sigaddset(&sigblock, SIGINT);
    sigaddset(&sigblock, SIGTERM);
    pthread_sigmask(SIG_BLOCK, &sigblock, NULL);

    if(itsnet_cfg_parse(cfile) != 0) {
        printf("Failed to parse configuration file %s\n", cfile);
        itsnet_cfg_clean();
        return 1;
    }

    if(DetachFromTTY_parse()) {
        daemon_start(1);
    } else {
        printf("%s v%s started\n", program_name, PACKAGE_VERSION);
    }

    itsnet_main(argc, argv);
    pthread_create(&sigth, NULL, sigh, NULL);
    pthread_join(sigth, NULL);

    return 0;
}
