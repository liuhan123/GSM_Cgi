all:index.html message.cgi

index.html:main.cpp
	g++ main.cpp -o index.html

message.cgi:message.cpp
	g++ message.cpp -o message.cgi

clean:
	rm -rf index.html message.cgi
