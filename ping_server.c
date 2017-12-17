#include <stdio.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <linux/icmp.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

#define ICMP_DATA_OFFSET (sizeof(struct iphdr) + sizeof(struct icmphdr))

uint16_t in_cksum(uint16_t *addr, int len)
{
        int             nleft = len;
        uint32_t        sum = 0;
        uint16_t        *w = addr;
        uint16_t        answer = 0;

        while (nleft > 1)  {
                sum += *w++;
                nleft -= 2;
        }

        if (nleft == 1) {
                *(unsigned char *)(&answer) = *(unsigned char *)w ;
                sum += answer;
        }

        sum = (sum >> 16) + (sum & 0xffff);
        sum += (sum >> 16);
        answer = ~sum;
        return(answer);
}

int main() {
	int     sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sock == -1) {
		perror("Create sockfd error");
		exit(-1);
	}

	printf("sock ready\n");
	while (1) {
		char buf[4096] = "";
		int len;
		struct iphdr *pip;
		struct icmphdr *picmp;
		len = recvfrom(sock,buf,sizeof(buf) - 1, 0, NULL, NULL);
		if (len == -1) {
			perror("Recv data from socket error");
			return -1;
		}
		pip = (struct iphdr*) buf;
		picmp = (struct icmphdr*) (buf + sizeof(struct iphdr));

		char sip[20],dip[20];
		char *data;
		int data_len;
		uint16_t id,seq;
		struct in_addr saddr,daddr;
		uint8_t type;
		
		if( picmp->code != 0 ) 
			continue;

		/*resolve fields from buffer*/
		saddr.s_addr = pip->saddr;
		daddr.s_addr = pip->daddr;
		strcpy(sip,(inet_ntoa(saddr)));
		strcpy(dip,inet_ntoa(daddr));
		data = buf + ICMP_DATA_OFFSET;
		data_len = len - ICMP_DATA_OFFSET;

		id = ntohs(picmp -> un.echo.id);
		seq = ntohs(picmp -> un.echo.sequence);
		printf("Icmp ECHO Received  (%s => %s)\n"
			"\tLength=%d, Id=%d, Sequence=%d\n\n",
			sip, dip, len, id, seq);
		
		/*construct send buffer*/
		pip->ihl=5;
		pip->version = 4;
		pip->saddr = daddr.s_addr;
		pip->daddr = saddr.s_addr;
		pip->check = 0;
		pip->check = in_cksum((uint16_t*)pip,sizeof(struct iphdr));
		picmp->type = ICMP_ECHOREPLY;
		picmp->code = 0;
		picmp->checksum = 0;
		picmp->checksum = in_cksum((uint16_t*)picmp,
			sizeof(struct icmphdr) + data_len);
		struct sockaddr_in sinaddr;
		sinaddr.sin_family = AF_INET;
		sinaddr.sin_addr.s_addr = pip->daddr;
		int optval;
		setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &optval, sizeof(int));	
		int ret = sendto(sock, buf, len, 
			0, (struct sockaddr *)&sinaddr,sizeof(struct sockaddr));
	}
	return 0;
}
