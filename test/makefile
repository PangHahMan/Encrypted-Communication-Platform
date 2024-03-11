.PHONY:all

.PHONY:openssl
.PHONY:clean

openssl:openssl.cpp
	g++ -o $@ $^ -std=c++17 -lcrypto -Wno-deprecated-declarations
clean:
	rm -f server