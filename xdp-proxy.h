#ifndef __XDP_PROXY_H__
#define __XDP_PROXY_H__

#include <linux/types.h>
#include <linux/if_ether.h>

#define SVC1_KEY 0x1

struct endpoints {
	__be32 client;
	__be32 ep1;
	__be32 ep2;
	__be32 vip;
	unsigned char ep1_mac[ETH_ALEN];
	unsigned char ep2_mac[ETH_ALEN];
	unsigned char client_mac[ETH_ALEN];
	unsigned char vip_mac[ETH_ALEN];
} __attribute__((packed));

struct session_key {
    __be32 lip;     //lvs local ip
    __be32 lport;   //lvs local port
    __be32 fip;     //forward(self) ip
    __be32 fport;   //forward(self) port
} __attribute__((packed));

struct session {
    __be32 cip;     //client ip
    __be32 cport;   //client port
    __be32 rip;     //real server ip
    __be32 rport;   //real server port
    unsigned char lmac[ETH_ALEN];//lvs local mac
    unsigned char rmac[ETH_ALEN];//real server mac
} __attribute__((packed));

struct router {
	__be32 rip;
	unsigned char rmac[ETH_ALEN];
} __attribute__((packed));

#ifndef memcpy
#define memcpy(dest, src, n) __builtin_memcpy((dest), (src), (n))
#endif

#endif				/* __XDP_PROXY_H__ */
