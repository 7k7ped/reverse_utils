/**
 * simpliest raw tcp scanner
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/if_ether.h>  // ETH_P_IP
#include <linux/if_packet.h>   // AF_PACKET
#include <sys/socket.h>

#define BUFFER_SIZE 65535

void print_hex(const unsigned char *data, int len) {
    for (int i = 0; i < len; i++) {
        printf("%02X ", data[i]);
        if ((i + 1) % 16 == 0)
            printf("\n");
    }
    if (len % 16 != 0)
        printf("\n");
}

int main() {
    int raw_sock;
    unsigned char buffer[BUFFER_SIZE];
    struct sockaddr saddr;
    socklen_t saddr_size = sizeof(saddr);

    raw_sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP));
    if (raw_sock < 0) {
        perror("Socket error");
        return 1;
    }

    printf("Listening for TCP packets...\n");

    while (1) {
        ssize_t data_size = recvfrom(raw_sock, buffer, BUFFER_SIZE, 0, &saddr, &saddr_size);
        if (data_size < 0) {
            perror("Recvfrom error");
            close(raw_sock);
            return 1;
        }

        // ethernet header 14 bytes
        struct iphdr *ip = (struct iphdr *)(buffer + 14);

        if (ip->protocol == IPPROTO_TCP) {
            int ip_header_len = ip->ihl * 4;
            struct tcphdr *tcp = (struct tcphdr *)(buffer + 14 + ip_header_len);
            int tcp_header_len = tcp->doff * 4;

            int payload_offset = 14 + ip_header_len + tcp_header_len;
            int payload_len = data_size - payload_offset;

            if (payload_len > 0) {
                struct in_addr src, dst;
                src.s_addr = ip->saddr;
                dst.s_addr = ip->daddr;

                printf("TCP %s:%d -> %s:%d | %d bytes payload\n",
                       inet_ntoa(src), ntohs(tcp->source),
                       inet_ntoa(dst), ntohs(tcp->dest),
                       payload_len);
                print_hex(buffer + payload_offset, payload_len);
                printf("\n");
            }
        }
    }

    close(raw_sock);
    return 0;
}
