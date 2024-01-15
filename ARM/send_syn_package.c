#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

unsigned short csum(unsigned short *ptr, int nbytes);

void send_syn_packet(const char *src_ip, const char *dest_ip, int src_port, int dest_port, uint32_t seq_num, uint16_t window_size);
struct pseudo_header
{
    unsigned int source_address;
    unsigned int dest_address;
    unsigned char placeholder;
    unsigned char protocol;
    unsigned short tcp_length;

    struct tcphdr tcp;
};
int main()
{
    const char *source_ip = "192.168.1.6";
    const char *dest_ip = "192.168.1.10";
    int source_port = 6052;
    int dest_port = 12345;
    uint32_t seq_num = 0;
    uint16_t window_size = 64240;

    send_syn_packet(source_ip, dest_ip, source_port, dest_port, seq_num, window_size);

    return 0;
}

void send_syn_packet(const char *src_ip, const char *dest_ip, int src_port, int dest_port, uint32_t seq_num, uint16_t window_size)
{
    int s = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (s < 0)
    {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    char datagram[4096];
    struct iphdr *iph = (struct iphdr *)datagram;
    struct tcphdr *tcph = (struct tcphdr *)(datagram + sizeof(struct iphdr));
    struct sockaddr_in dest_addr;
    struct pseudo_header psh;

    memset(datagram, 0, 4096);

    // Fill in the IP Header
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = sizeof(struct iphdr) + sizeof(struct tcphdr);
    iph->id = htons(54321);          // Id of this packet
    iph->frag_off = htons(16384);
    iph->ttl = 64;
    iph->protocol = IPPROTO_TCP;
    iph->check = 0;                  // Set to 0 before calculating checksum
    iph->saddr = inet_addr(src_ip);  // Source IP address
    iph->daddr = inet_addr(dest_ip); // Destination IP address

    iph->check = csum((unsigned short *)datagram, iph->tot_len >> 1);

    // TCP Header
    tcph->source = htons(src_port);
    tcph->dest = htons(dest_port);
    tcph->seq = htonl(seq_num);
    tcph->ack_seq = 0;
    tcph->doff = sizeof(struct tcphdr) / 4; // Size of TCP header
    tcph->fin = 0;
    tcph->syn = 1;        // Set the SYN flag
    tcph->rst = 0;
    tcph->psh = 0;
    tcph->ack = 0;
    tcph->urg = 0;
    tcph->window = htons(window_size); // Window size
    tcph->check = 0;                    // Set to 0 for kernel to fill correct checksum
    tcph->urg_ptr = 0;

    // Set destination address for SYN
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(dest_ip);
    dest_addr.sin_port = htons(dest_port);

    // Set up pseudo-header for checksum calculation
    psh.source_address = inet_addr(src_ip);
    psh.dest_address = inet_addr(dest_ip);
    psh.placeholder = 0;
    psh.protocol = IPPROTO_TCP;
    psh.tcp_length = htons(sizeof(struct tcphdr));

    memcpy(&psh.tcp, tcph, sizeof(struct tcphdr));

    tcph->check = csum((unsigned short *)&psh, sizeof(struct pseudo_header));

    // Send the SYN packet
    if (sendto(s, datagram, sizeof(struct iphdr) + sizeof(struct tcphdr), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0)
    {
        perror("Error sending SYN packet");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("SYN packet sent successfully.\n");
    }

    close(s);
}

unsigned short csum(unsigned short *ptr, int nbytes)
{
    register long sum;
    unsigned short oddbyte;
    register short answer;

    sum = 0;
    while (nbytes > 1)
    {
        sum += *ptr++;
        nbytes -= 2;
    }
    if (nbytes == 1)
    {
        oddbyte = 0;
        *((u_char *)&oddbyte) = *(u_char *)ptr;
        sum += oddbyte;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum = sum + (sum >> 16);
    answer = (short)~sum;

    return (answer);
}
