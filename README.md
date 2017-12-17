# Ping Server for linux #


a ping server for linux ,receive ECHO Request package then send ECHO Reply package to remote ping process.

Linux下的ping服务端，接收ping回应请求，然后回复该请求

Usage:

* Server

	#&resp;echo "1">/proc/sys/net/ipv4/icmp_echo_ignore_all
	# gcc -o ping_server ping_server.c
	# ./ping_server

* Client
	C:\> ping.exe x.x.x.x


Good Luck
