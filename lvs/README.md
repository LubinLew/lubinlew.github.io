# LVS

## LVS相关术语

- DS：Director Server。指的是前端负载均衡器节点

- RS：Real Server。后端真实的工作服务器

- VIP：向外部直接面向用户请求，作为用户请求的目标的IP地址

- DIP：Director Server IP，主要用于和内部主机通讯的IP地址

- RIP：Real Server IP，后端服务器的IP地址

- CIP：Client IP，访问客户端的IP地址

## LVS基本工作原理

1、当用户向负载均衡调度器（Director Server）发起请求，调度器将请求发往至内核空间

2、 PREROUTING 链首先会接收到用户请求，判断目标 IP 确定是本机 IP ，将数据包发往INPUT 链

3、 IPVS 是工作在 INPUT 链上的，当用户请求到达 INPUT 时，IPVS 会将用户请求和自己已定义好的集群服务进行比对，如果用户请求的就是定义的集群服务，那么此时 IPVS 会强行修改数据包里的目标IP地址及端口，并将新的数据包发往 POSTROUTING 链

4、 POSTROUTING 链接收数据包后发现目标IP地址刚好是自己的后端服务器，那么此时通过选路，将数据包最终发送给后端的服务器

三种模式及原理

一）LVS-NAT原理和特点

多目标IP的DNAT，通过将请求报文中的目标地址和目标端口修改为某挑出的RS的RIP和PORT实现转发

1.当用户请求到达Director Server，此时请求的数据报文会先到内核空间的PREROUTING链。 此时报文的源IP为CIP，目标IP为VIP

2. PREROUTING检查发现数据包的目标IP是本机，将数据包送至INPUT链

3. IPVS比对数据包请求的服务是否为集群服务，若是，修改数据包的目标IP地址为后端服务器IP，后将数据包发至POSTROUTING链。 此时报文的源IP为CIP，目标IP为RIP

4. POSTROUTING链通过选路，将数据包发送给Real Server

5. Real Server比对发现目标为自己的IP，开始构建响应报文发回给Director Server。 此时报文的源IP为RIP，目标IP为CIP

6. Director Server在响应客户端前，此时会将源IP地址修改为自己的VIP地址，然后响应给客户端。 此时报文的源IP为VIP，目标IP为CIP

LVS-NAT模式的特性

(1) RS应该和DIP应该使用私网地址，且RS的网关要指向DIP；

(2) 请求和响应报文都要经由director转发；极高负载的场景中，director可能会成为系统瓶颈；

(3) 支持端口映射；

(4) RS可以使用任意OS；

(5) RS的RIP和Director的DIP必须在同一IP网络；

缺陷：对Director Server压力会比较大，请求和响应都需经过director server
