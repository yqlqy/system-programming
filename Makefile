all:
	gcc -g tcpclient.c -o tcpclient
	gcc -g tcpserver.c -o tcpserver

clean:
	rm tcpclient tcpserver
