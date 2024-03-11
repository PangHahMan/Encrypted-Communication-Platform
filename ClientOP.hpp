#pragma once
#include <string>
#include <jsoncpp/json/json.h>
#include <fstream>
#include <sstream>

using namespace std;

//客户端信息
struct ClientInfo
{
	string server_id;   //服务端ID
	string client_id;   //客户端ID
	string ip;         //客户端ip
	unsigned short port;  //客户端端口号
};

class ClientOP
{
public:
    //通过读取JSON文件初始化客户端信息
	ClientOP(string json_file);
	~ClientOP();

	// 秘钥协商
	bool seckeyAgree();

	// 秘钥校验
	void seckeyCheck();

	// 秘钥注销
	void seckeyLogout();

private:
	ClientInfo client_info;  //客户端信息的实例，用于存储当前会话的详细信息
};

