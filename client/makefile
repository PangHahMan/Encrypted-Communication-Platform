client:client.cpp Message.pb.cpp
	g++ -o $@ $^ -std=c++17 -lpthread -lssl -lcrypto -lprotobuf -ljsoncpp

.PHONY:clean

clean:
	rm -f client

run:
	./client