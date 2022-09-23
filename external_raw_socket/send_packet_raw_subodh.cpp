#include <iostream>
#include <cstdlib>
#include<string.h> //memset
#include<sys/socket.h>	//for socket ofcourse
#include<stdlib.h> //for exit(0);
#include<errno.h> //For errno - the error number
#include<netinet/udp.h>	//Provides declarations for udp header
#include<netinet/ip.h>	//Provides declarations for ip header
#include <arpa/inet.h>
#include <unistd.h>

/*
	96 bit (12 bytes) pseudo header needed for udp header checksum calculation
*/
struct pseudo_header
{
    u_int32_t source_address;
    u_int32_t dest_address;
    u_int8_t placeholder;
    u_int8_t protocol;
    u_int16_t udp_length;
};

/*
	Generic checksum calculation function
*/
unsigned short csum(unsigned short *ptr,int nbytes)
{
    register long sum;
    unsigned short oddbyte;
    register short answer;

    sum=0;
    while(nbytes>1) {
        sum+=*ptr++;
        nbytes-=2;
    }
    if(nbytes==1) {
        oddbyte=0;
        *((u_char*)&oddbyte)=*(u_char*)ptr;
        sum+=oddbyte;
    }

    sum = (sum>>16)+(sum & 0xffff);
    sum = sum + (sum>>16);
    answer=(short)~sum;

    return(answer);
}

int main (int argc, char **argv) {
    //Create a raw socket of type IPPROTO
    int s = socket (AF_INET, SOCK_RAW, IPPROTO_RAW);
    if(s < 0) {
        //socket creation failed, may be because of non-root privileges
        perror("Failed to create raw socket");
        exit(1);
    }

    //Datagram to represent the packet
    char datagram[4096] , source_ip[32] , *data , *pseudogram;
    const short port = atoi(argv[3]);
    //zero out the packet buffer
    memset (datagram, 0, 4096);

    //IP header
    struct iphdr *iph = (struct iphdr *) datagram;

    //UDP header
    struct udphdr *udph = (struct udphdr *) (datagram + sizeof (struct ip));

    struct sockaddr_in sin;
    struct pseudo_header psh;

    //Data part
    data = datagram + sizeof(struct iphdr) + sizeof(struct udphdr);
    strcpy(data , "ABCDEFGHIJKLMNOPQRSTUVWXYZ   ");

    //some address resolution
    strcpy(source_ip , argv[1]);

    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = inet_addr(argv[2]);

    //Fill in the IP Header
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = sizeof(struct iphdr) + sizeof(struct udphdr) + strlen(data);
    iph->id = htonl(54321);	//Id of this packet
    iph->frag_off = 0;
    iph->ttl = 255;
    iph->protocol = IPPROTO_UDP;
    iph->check = 0;		//Set to 0 before calculating checksum
    iph->saddr = inet_addr(source_ip);	//Spoof the source ip address
    iph->daddr = sin.sin_addr.s_addr;

    //Ip checksum
    iph->check = csum ((unsigned short *)datagram, iph->tot_len);

    //UDP header
    udph->source = htons(port);
    udph->dest = htons(port);
    udph->len = htons(8 + strlen(data));	//tcp header size
    udph->check = 0;	//leave checksum 0 now, filled later by pseudo header

    //Now the UDP checksum using the pseudo header
    psh.source_address = inet_addr(source_ip);
    psh.dest_address = sin.sin_addr.s_addr;
    psh.placeholder = 0;
    psh.protocol = IPPROTO_UDP;
    psh.udp_length = htons(sizeof(struct udphdr) + strlen(data));

    int psize = sizeof(struct pseudo_header) + sizeof(struct udphdr) + strlen(data);
    pseudogram = (char*)malloc(psize);

    memcpy(pseudogram, (char*)&psh, sizeof (struct pseudo_header));
    memcpy(pseudogram + sizeof(struct pseudo_header), udph , sizeof(struct udphdr) + strlen(data));

    udph->check = csum((unsigned short*)pseudogram, psize);

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = inet_addr(argv[1]);
    if (bind(s, (struct sockaddr*)&address, sizeof(address))) {
        std::cout << "Can`t bind server address" << std::endl;
        exit(-1);
    }

    // Send the packet
    if (sendto(s, datagram, iph->tot_len ,	0, (struct sockaddr *) &sin, sizeof (sin)) < 0) {
        perror("sendto failed");
    }
        //Data send successfully
    else {
        std::cout <<  "Packet Send. Length : "<< iph->tot_len << std::endl;
    }

    close(s);
    return 0;
}