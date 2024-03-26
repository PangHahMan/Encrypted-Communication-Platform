#include <cstdio>
#include "ServerOP.hpp"
int main()
{
	ServerOP op("server.json");
	op.startServer();

    return 0;
}