/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/route.h>

#include <android/log.h>
#include <cutils/properties.h>

static char propstr[65]; 	// hold "net.$LINKNAME"
static char propstr1[128];	// hold "net.$LINKNAME.xxx"

static inline struct in_addr *in_addr(struct sockaddr *sa)
{
    return &((struct sockaddr_in *)sa)->sin_addr;
}

int main(int argc, char **argv)
{
    struct rtentry rt;
    int s, ret;

/* args is like this:
    argv[1] = ifname;
    argv[2] = devnam;
    argv[3] = strspeed;
    argv[4] = strlocal;
    argv[5] = strremote;
    argv[6] = ipparam;
*/
    if (argc < 6)
	return EINVAL;

    __android_log_print(ANDROID_LOG_INFO, "ip-up-ppp0", "ip-up-ppp0 script is launched with \n");
    __android_log_print(ANDROID_LOG_INFO, "ip-up-ppp0", "\tInterface\t:\t%s", 	argv[1]);
    __android_log_print(ANDROID_LOG_INFO, "ip-up-ppp0", "\tTTY device\t:\t%s",	argv[2]);
    __android_log_print(ANDROID_LOG_INFO, "ip-up-ppp0", "\tTTY speed\t:\t%s",	argv[3]);
    __android_log_print(ANDROID_LOG_INFO, "ip-up-ppp0", "\tLocal IP\t:\t%s",	argv[4]);
    __android_log_print(ANDROID_LOG_INFO, "ip-up-ppp0", "\tRemote IP\t:\t%s",	argv[5]);

    /* Clean out the RTREQ structure */
    memset(&rt, 0, sizeof(rt));

    /* Prepare new "default" route entry for ppp0 link 
       Destination: 0.0.0.0
       Netmask:     0.0.0.0
       Gateway:     not specified 
       Interface:   ifname as argv[1] 
       Flag:	    RTF_UP
    */
    rt.rt_dst.sa_family = AF_INET;
    ((struct sockaddr_in *)&rt.rt_dst)->sin_addr.s_addr = INADDR_ANY;

    rt.rt_genmask.sa_family = AF_INET;
    ((struct sockaddr_in *)&rt.rt_genmask)->sin_addr.s_addr = INADDR_ANY;

    rt.rt_dev = argv[1];

    rt.rt_flags = RTF_UP;
    /* Add "default" entry into route table */
    s = socket(AF_INET, SOCK_DGRAM, 0);
    if(s == -1) {
        __android_log_print(ANDROID_LOG_ERROR, "ip-up-ppp0", "Fail to get socket to modify route table");
	return EPERM;
    } 	
    ret = ioctl(s, SIOCADDRT, &rt);
    if(ret == -1) {
	if(errno == EEXIST) {
            __android_log_print(ANDROID_LOG_WARN, "ip-up-ppp0", "Unable to add default entry in route table - Already exist");
	    return 0;
	}
        __android_log_print(ANDROID_LOG_ERROR, "ip-up-ppp0", "Fail to add default entry in route table - %s", strerror(errno));
	return EPERM;
    }
    __android_log_print(ANDROID_LOG_INFO, "ip-up-ppp0", "Add default entry into route table");

    {
	// we need setup lots of property like net.$LINKNAME.[local-ip|remote-ip|dns1|dns2]
	strcpy(propstr, "net."); 
	strncat(propstr, getenv("LINKNAME"), 60);  // we never allow $LINKNAME exceed 60 chars
	strncat(propstr, ".", 1);
        __android_log_print(ANDROID_LOG_INFO, "ip-up-ppp0", "LINKNAME is %s", propstr);

	// set net.$LINKNAME.local-ip prop to $IPLOCAL
	strcpy(propstr1, propstr);
	strcat(propstr1, "local-ip");
	char *iplocal = getenv("IPLOCAL");
	property_set(propstr1, iplocal ? iplocal : "");
        __android_log_print(ANDROID_LOG_INFO, "ip-up-ppp0", "Setprop %s to %s", propstr1, iplocal ? iplocal : "");

	// set net.$LINKNAME.remote-ip prop to $IPREMOTE
	strcpy(propstr1, propstr);
	strcat(propstr1, "remote-ip");
	char *ipremote = getenv("IPREMOTE");
	property_set(propstr1, ipremote ? ipremote : "");
        __android_log_print(ANDROID_LOG_INFO, "ip-up-ppp0", "Setprop %s to %s", propstr1, ipremote ? ipremote : "");

	// set net.$LINKNAME.dns1 prop to $DNS1
	strcpy(propstr1, propstr);
	strcat(propstr1, "dns1");
	char *dns1 = getenv("DNS1");
	property_set(propstr1, dns1 ? dns1 : "");
    __android_log_print(ANDROID_LOG_INFO, "ip-up-ppp0", "Setprop %s to %s", propstr1, dns1 ? dns1 : "");

	// set net.$LINKNAME.dns2 prop to $DNS2
	strcpy(propstr1, propstr);
	strcat(propstr1, "dns2");
	char *dns2 = getenv("DNS2");
	property_set(propstr1, dns2 ? dns2 : "");
     __android_log_print(ANDROID_LOG_INFO, "ip-up-ppp0", "Setprop %s to %s", propstr1, dns2 ? dns2 : "");
    }
    __android_log_print(ANDROID_LOG_INFO, "ip-up-ppp0",
                        "All traffic is now redirected to %s", argv[1]);

    return 0;
}
