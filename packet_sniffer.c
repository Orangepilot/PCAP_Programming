#include <stdlib.h>
#include <stdio.h>
#include <pcap.h>
#include <arpa/inet.h>
#include <string.h>
#include <ctype.h>

#include "myheader.h" //제공된 IP, TCP, Ethernet구조체등이 정의된 헤더 포함


void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {
    struct ethheader *eth = (struct ethheader *)packet;

    if(ntohs(eth->ether_type) != 0X0800){  //IP 패킷이 아니면 무시
        return;
    }
    struct ipheader *ip = (struct ipheader *)(packet + sizeof(struct ethheader));
    //IP Header. 패킷 시작점에서 Ethernet Header 크기만큼 이동
    int ip_header_len = ip->iph_ihl * 4; //IP Header 실제 길 = ihl값 X 4byte

    if (ip->iph_protocol != IPPROTO_TCP){    //TCP 패킷이 아닌경우 무시
        return;
    }
    struct tcpheader *tcp = (struct tcpheader *)((u_char *)ip + ip_header_len);
    // 패킷 시작점 + Ethernet Header 크기 + IP 헤더 크기 만큼 이동
    int tcp_header_len = ((tcp->tcp_offx2 & 0Xf0) >> 4) * 4;
    //TCP Header 길이 계산. 상위4비트가 길이를 나타내므로 shift연산 후 4byte를 곱함
    
    int payload_len = ntohs(ip->iph_len) - ip_header_len - tcp_header_len;
    //전체 packet길이에서 IP Header와 TCP Header 길이를 빼서 Data의 길이를 구한다
    u_char *payload = (u_char *)tcp + tcp_header_len; //데이터 시작점 = TCP Header 끝지점
    
    printf("\n-----------------------------\n");
    printf("Ethernet SRC MAC: %02x:%02x:%02x:%02x:%02x:%02x\n", eth->ether_shost[0], eth->ether_shost[1], eth->ether_shost[2], eth->ether_shost[3], eth->ether_shost[4], eth->ether_shost[5]);
    printf("Ethernet DST MAC: %02x:%02x:%02x:%02x:%02x:%02x\n", eth->ether_dhost[0], eth->ether_dhost[1], eth->ether_dhost[2], eth->ether_dhost[3], eth->ether_dhost[4], eth->ether_dhost[5]);
    //MAC 주소 출력
    printf("IP SRC IP: %s\n", inet_ntoa(ip->iph_sourceip));
    printf("IP DST IP: %s\n", inet_ntoa(ip->iph_destip));
    //IP 주소 출력
    printf("TCP SRC PORT: %d\n", ntohs(tcp->tcp_sport));
    printf("TCP DST PORT: %d\n", ntohs(tcp->tcp_dport));
    //port번호 출력

    if (payload_len > 0) {
        printf("HTTP Message (Length: %d bytes)\n", payload_len);
        //출력 가능한 문자만 필터링해서 출력
        for (int i = 0; i < payload_len; i++) {
            if (isprint(payload[i]) || payload[i] == '\n' || payload[i] == '\r') {
                printf("%c", payload[i]);
            } 
            else {
                printf(".");//이상한 값은 .출력
            }
        }
        printf("\n");
    } 
    else {
        printf("No Data\n");
    }
    //HTTP messages 출력
  
}

int main()
{
  pcap_t *handle;
  char errbuf[PCAP_ERRBUF_SIZE];
  struct bpf_program fp;
  char filter_exp[] = "tcp port 8080";     //tcp만 잡도록 제한
  bpf_u_int32 net;

  
  handle = pcap_open_live("lo", BUFSIZ, 1, 1000, errbuf);

  // 필터 컴파일 / 적용
  pcap_compile(handle, &fp, filter_exp, 0, net);
  if (pcap_setfilter(handle, &fp) !=0) {
      pcap_perror(handle, "Error:");
      exit(EXIT_FAILURE);
  }
  printf("Packet Sniffer Started...Wait for TCP packets\n");

  // 패킷을 캡쳐하는 루프 시작
  pcap_loop(handle, -1, got_packet, NULL);

  pcap_close(handle);   //종료시 handle 닫기
  return 0;
}