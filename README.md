Ping Server for linux


a ping server for linux ,receive ECHO Request package then send ECHO Response package to remote ping process.

Linux下的ping服务端，接收ping回应请求，然后回复该回应请求

Usage:

	# echo "1">/proc/sys/net/ipv4/icmp_echo_ignore_all
	# gcc -o ping_server ping_server.c
	# ./ping_server
