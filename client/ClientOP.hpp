#pragma once
#include "Hash.hpp"
#include "Message.pb.hpp"
#include "RequestCodec.hpp"
#include "RequestFactory.hpp"
#include "RespondCodec.hpp"
#include "RespondFactory.hpp"
#include "RsaCrypto.hpp"
#include "TcpSocket.hpp"
#include <fstream>
#include <iostream>
#include <json/json.h>

#include <sstream>
#include <string>
using namespace std;
using namespace Json;

struct ClientInfo {
    string server_id;
    string client_id;
    string ip;
    unsigned short port;
};

class ClientOP {
public:
    ClientOP(string jsonFile) {
        cout << "开始解析文件" << endl;
        // 解析json文件, 读文件 -> Value
        ifstream ifs(jsonFile);
        Reader r;
        Value root;
        r.parse(ifs, root);
        // 将root中的键值对value值取出
        m_info.server_id = root["server_id"].asString();
        m_info.client_id = root["client_Id"].asString();
        m_info.ip = root["server_ip"].asString();
        m_info.port = root["port"].asInt();

        cout << "server_id:" << m_info.server_id << endl;
    }
    ~ClientOP() {}

    // 秘钥协商
    bool seckeyAgree() {
        // 0. 生成密钥对, 将公钥字符串读出
        RsaCrypto rsa;
        // 生成密钥对
        rsa.generateRsakey(1024);
        // 读公钥文件
        ifstream ifs("public.pem");
        stringstream str;
        str << ifs.rdbuf();
        // 1. 初始化序列化数据
        // 序列化的类对象 -> 工厂类创建
        RequestInfo reqInfo;
        reqInfo.client_id = m_info.client_id;
        reqInfo.server_id = m_info.server_id;
        reqInfo.cmd = 1;         // 秘钥协商
        reqInfo.data = str.str();// 非对称加密的公钥
        // 创建哈希对象
        Hash sha1(T_SHA1);
        sha1.addData(str.str());
        reqInfo.sign = rsa.rsaSign(sha1.result());// 公钥的的哈希值签名
        cout << "签名完成..." << endl;
        CodecFactory *factory = new RequestFactory(&reqInfo);
        Codec *c = factory->createCodec();
        // 得到序列化之后的数据, 可以将其发送给服务器端
        string encstr = c->encodeMsg();
        // 释放资源
        delete factory;
        delete c;

        // 套接字通信, 当前是客户端, 连接服务器
        TcpSocket *tcp = new TcpSocket;
        // 连接服务器
        int ret = tcp->connectToHost(m_info.ip, m_info.port);
        if (ret != 0) {
            cout << "连接服务器失败..." << endl;
            return false;
        }
        cout << "连接服务器成功..." << endl;
        // 发送序列化的数据
        tcp->sendMsg(encstr);
        // 等待服务器回复
        string msg = tcp->recvMsg();

        // 解析服务器数据 -> 解码(反序列化)
        // 数据还原到 RespondMsg
        factory = new RespondFactory(msg);
        c = factory->createCodec();
        RespondMsg *resData = (RespondMsg *) c->decodeMsg();
        // 判断状态
        if (!resData->status()) {
            cout << "秘钥协商失败" << endl;
            return false;
        }
        // 将得到的密文解密
        string key = rsa.rsaPriKeyDecrypt(resData->data());
        cout << "对称加密的秘钥: " << key << endl;
        // 秘钥写入共享内存中

        delete factory;
        delete c;
        // 这是一个短连接, 通信完成, 断开连接
        tcp->disConnect();
        delete tcp;

        return true;
    }

    // 秘钥校验
    void seckeyCheck() {}

    // 秘钥注销
    void seckeyLogOut() {}

private:
    ClientInfo m_info;
};
