## IPv4 Protocol

> https://www.ietf.org/rfc/rfc791.html

```spec
+-----------------------------------------------------------------------+
byte    |    byte 0     |    byte 1     |    byte 2     |    byte 3     |
bit     |0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7|
+-------+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
word 0  |Version|  IHL  |Type of Service|          Total Length         |
+-------+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
word 1  |         Identification        |Flags|      Fragment Offset    |
+-------+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
word 2  |  Time to Live |    Protocol   |         Header Checksum       |
+-------+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
word 3  |                       Source Address                          |
+-------+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
word 4  |                    Destination Address                        |
+-------+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
word 5  |                    Options                    |    Padding    |
+-------+---------------------------------------------------------------+
|       |                             Payload                           |
+-------+---------------------------------------------------------------+
```

| Field                       | abbr.    | Length   | Description | Remark | Other |
| --------------------------- | -------- | -------- | ----------- | ------ | ----- |
| Version                     | version  | 4 bits   |             |        |       |
| IHL(Internet Header Length) | ihl      | 4 bits   |             |        |       |
| Type of Service             | tos      | 8 bits   |             |        |       |
| Total Length                | tot_len  | 16 bits  |             |        |       |
| Identification              | id       | 16 bits  |             |        |       |
| Flags                       | -        | 3 bits   |             |        |       |
| Fragment Offset             | frag_off | 3 bits   |             |        |       |
| Time to Live                | ttl      | 8 bits   |             |        |       |
| Protocol                    | protocol | 8 bits   |             |        |       |
| Header Checksum             | check    | 16 bits  |             |        |       |
| Source Address              | saddr    | 32 bits  |             |        |       |
| Destination Address         | daddr    | 32 bits  |             |        |       |
| Options                     | -        | variable |             |        |       |

```c
// linux/ip.h

  85 struct iphdr {
#if defined(__LITTLE_ENDIAN_BITFIELD)
    __u8    ihl:4,
        version:4;
#elif defined (__BIG_ENDIAN_BITFIELD)
    __u8    version:4,
                ihl:4;
#else
#error  "Please fix <asm/byteorder.h>"
#endif
    __u8    tos;
    __be16  tot_len;
    __be16  id;
    __be16  frag_off;
    __u8    ttl;
    __u8    protocol;
    __sum16 check;
    __be32  saddr;
    __be32  daddr;
    /* The options start here. */
};
```

## IPv6 Protocol

> https://www.ietf.org/rfc/rfc2460.html

## TCP

> https://www.ietf.org/rfc/rfc793.html

```txt
+-----------------------------------------------------------------------+
byte    |    byte 0     |    byte 1     |    byte 2     |    byte 3     |
bit     |0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7|
+-------+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
word 0  |          Source Port          |       Destination Port        |
+-------+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
word 1  |                        Sequence Number                        |
+-------+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
word 2  |                    Acknowledgment Number                      |
+-------+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
word 3  |  Data |           |U|A|P|R|S|F|                               |
        | Offset| Reserved  |R|C|S|S|Y|I|            Window             |
        |       |           |G|K|H|T|N|N|                               |
+-------+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
word 4  |           Checksum            |         Urgent Pointer        |
+-------+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
word 5  |                    Options                    |    Padding    |
+-----------------------------------------------------------------------+
        |                             data                              |
+-----------------------------------------------------------------------+
```

| Field                 | abbr.   | Length   | Destination                                                                                                                                                                                                                                                                                          |
| --------------------- | ------- | -------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Source Port           | source  | 16 bits  | The source port number                                                                                                                                                                                                                                                                               |
| Destination Port      | dest    | 16 bits  | The destination port number                                                                                                                                                                                                                                                                          |
| Sequence Number       | seq     | 32 bits  | The sequence number of the first data octet in this segment (except when SYN is present). If SYN is present the sequence number is the initial sequence number (ISN) and the first data octet is ISN+1.                                                                                              |
| Acknowledgment Number | ack_seq | 32 bits  | If the ACK control bit is set this field contains the value of the next sequence number the sender of the segment is expecting to receive.  Once a connection is established this is always sent.                                                                                                    |
| Data Offset           | doff    | 4 bits   | The number of 32 bit words in the TCP Header.  This indicates where the data begins.  The TCP header (even one including options) is an integral number of 32 bits long                                                                                                                              |
| Reserved              | res1    | 6 bits   | Reserved for future use.  Must be zero                                                                                                                                                                                                                                                               |
| Control Bits          | -       | 6 bits   | from left to right: <br>URG:  Urgent Pointer field significant<br>ACK:  Acknowledgment field significant<br>PSH:  Push Function<br>RST:  Reset the connection<br>SYN:  Synchronize sequence numbers<br>FIN:  No more data from sender                                                                |
| Window                | window  | 16 bits  | The number of data octets beginning with the one indicated in the acknowledgment field which the sender of this segment is willing to accept                                                                                                                                                         |
| Checksum              | check   | 16 bits  | The checksum field is the 16 bit one's complement of the one's complement sum of all 16 bit words in the header and text                                                                                                                                                                             |
| Urgent Pointer        | urg_ptr | 16 bits  | This field communicates the current value of the urgent pointer as a positive offset from the sequence number in this segment.  The urgent pointer points to the sequence number of the octet following the urgent data.  This field is only be interpreted in segments with the URG control bit set |
| Options               | -       | variable | Options may occupy space at the end of the TCP header and are a multiple of 8 bits in length.  All options are included in the checksum.  An option may begin on any octet boundary.  There are two cases for the format of an option                                                                |

```c
// linux/tcp.h
 struct tcphdr {
    __be16  source;
    __be16  dest;
    __be32  seq;
    __be32  ack_seq;
#if defined(__LITTLE_ENDIAN_BITFIELD)
    __u16   res1:4,
        doff:4,
        fin:1,
        syn:1,
        rst:1,
        psh:1,
        ack:1,
        urg:1,
        ece:1,
        cwr:1;
#elif defined(__BIG_ENDIAN_BITFIELD)
    __u16   doff:4,
        res1:4,
        cwr:1,
        ece:1,
        urg:1,
        ack:1,
        psh:1,
        rst:1,
        syn:1,
        fin:1;
#else
#error  "Adjust your <asm/byteorder.h> defines"
#endif  
    __be16  window;
    __sum16 check;
    __be16  urg_ptr;
};
```