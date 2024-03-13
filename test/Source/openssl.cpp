#include "../Header/openssl.hpp"

int main()
{
	char result[33] = { 0 };
	getMD5("hello, md5", result);
	printf("md5 value: %s\n", result);

	return 0;
}