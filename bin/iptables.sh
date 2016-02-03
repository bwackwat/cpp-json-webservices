#!/bin/bash

# Reset all.
iptables -P INPUT ACCEPT
iptables -P FORWARD ACCEPT
iptables -P OUTPUT ACCEPT
iptables -F

ip6tables -P INPUT ACCEPT
ip6tables -P FORWARD ACCEPT
ip6tables -P OUTPUT ACCEPT
ip6tables -F

#IPv4
iptables -A INPUT -i lo -j ACCEPT
iptables -A INPUT -m conntrack --ctstate ESTABLISHED,RELATED -j ACCEPT

#SSH
iptables -A INPUT -p tcp --dport 22 -j ACCEPT

#Application
iptables -A INPUT -p tcp --dport 3000 -j ACCEPT

iptables -A INPUT -j DROP

#IPv6
ip6tables -A INPUT -i lo -j ACCEPT
ip6tables -A INPUT -m conntrack --ctstate ESTABLISHED,RELATED -j ACCEPT

#SSH
ip6tables -A INPUT -p tcp --dport 22 -j ACCEPT

ip6tables -A INPUT -j DROP
