#pragma once
#include "Hash.hpp"
#include "Message.pb.hpp"
#include "RequestCodec.hpp"
#include "RequestFactory.hpp"
#include "RespondCodec.hpp"
#include "RespondFactory.hpp"
#include "RsaCrypto.hpp"
#include "TcpServer.hpp"
#include "TcpSocket.hpp"
#include <fstream>
#include <iostream>
#include <json/json.h>
#include <string>
#include <unistd.h>
#include <unordered_map>
using namespace std;
using namespace Json;

// 处理客户端请求
class ServerOP {
public:
    enum KeyLen {
        Len16 = 16,
        Len24 = 24,
        Len32 = 32
    };
    ServerOP(string json) {
        // 解析json文件, 读文件 -> Value
        ifstream ifs(json);
        Reader r;
        Value root;
        r.parse(ifs, root);
        // 将root中的键值对value值取出
        m_port = root["port"].asInt();
        m_serverID = root["server_id"].asString();// 解析json文件, 读文件 -> Value
    }
    // 启动服务器
    void startServer() {
        m_server = new TcpServer;
        m_server->setListen(m_port);
        while (true) {
            cout << "等待客户端连接..." << endl;
            TcpSocket *tcp = m_server->acceptConn();
            if (tcp == NULL) {
                continue;
            }
            cout << "与客户端连接成功..." << endl;
            // 通信
            pthread_t tid;
            // 这个回调可以是类的静态函数, 类的友元函数, 普通的函数
            // 友元的类的朋友, 但是不属于这个类
            // 友元函数可以访问当前类的私有成员
            pthread_create(&tid, NULL, workHard, this);
            m_list.insert(make_pair(tid, tcp));
        }
    }
    // 线程工作函数 -> 推荐使用
    static void *working(void *arg) {
        return nullptr;
    }
    // 友元破坏了类的封装
    friend void *workHard(void *arg);
    // 秘钥协商
    string seckeyAgree(RequestMsg *reqMsg) {
        // 0. 对签名进行校验 -> 公钥解密 -> 得到公钥
        // 将收到的公钥数据写入本地磁盘
        ofstream ofs("public.pem");
        ofs << reqMsg->data();
        ofs.close();
        // 创建非对称加密对象
        RespondInfo info;
        RsaCrypto rsa("public.pem", false);

        // 创建哈希对象
        Hash sha(T_SHA1);
        sha.addData(reqMsg->data());
        cout << "1111111111111111" << endl;
        bool bl = rsa.rsaVerify(sha.result(), reqMsg->sign());
        cout << "00000000000000000000" << endl;
        if (bl == false) {
            cout << "签名校验失败..." << endl;
            info.status = false;
        } else {
            cout << "签名校验成功..." << endl;
            // 1. 生成随机字符串
            //   对称加密的秘钥, 使用对称加密算法 aes, 秘钥长度: 16, 24, 32byte
            string key = getRandKey(Len16);
            cout << "生成的随机秘钥: " << key << endl;
            // 2. 通过公钥加密
            cout << "aaaaaaaaaaaaaaaa" << endl;
            string seckey = rsa.rsaPubKeyEncrypt(key);
            cout << "加密之后的秘钥: " << seckey << endl;
            // 3. 初始化回复的数据
            info.client_id = reqMsg->client_id();
            info.data = seckey;
            info.seckey_id = 12;// 需要数据库操作
            info.server_id = m_serverID;
            info.status = true;
        }

        // 4. 序列化
        CodecFactory *fac = new RespondFactory(&info);
        Codec *c = fac->createCodec();
        string encMsg = c->encodeMsg();
        // 5. 发送数据
        return encMsg;
    }
    ~ServerOP() {
        if (m_server) {
            delete m_server;
        }
    }

private:
    string getRandKey(KeyLen len) {
        int flag = 0;
        string randStr = string();
        char *cs = "~!@#$%^&*()_+}{|\';[]";
        for (int i = 0; i < len; ++i) {
            flag = rand() % 4;// 4中字符类型
            switch (flag) {
                case 0:// a-z
                    randStr.append(1, 'a' + rand() % 26);
                    break;
                case 1:// A-Z
                    randStr.append(1, 'A' + rand() % 26);
                    break;
                case 2:// 0-9
                    randStr.append(1, '0' + rand() % 10);
                    break;
                case 3:// 特殊字符
                    randStr.append(1, cs[rand() % strlen(cs)]);
                    break;
                default:
                    break;
            }
        }
        return randStr;
    }

private:
    string m_serverID;// 当前服务器的ID
    unsigned short m_port;
    unordered_map<pthread_t, TcpSocket *> m_list;
    TcpServer *m_server = NULL;
};

void *workHard(void *arg) {
    sleep(1);
    string data = string();
    // 通过参数将传递的this对象转换
    ServerOP *op = (ServerOP *) arg;
    // 从op中将通信的套接字对象取出
    TcpSocket *tcp = op->m_list[pthread_self()];
    // 1. 接收客户端数据 -> 编码
    string msg = tcp->recvMsg();
    // 2. 反序列化 -> 得到原始数据 RequestMsg 类型
    CodecFactory *fac = new RequestFactory(msg);
    Codec *c = fac->createCodec();
    RequestMsg *req = (RequestMsg *) c->decodeMsg();
    // 3. 取出数据
    // 判断客户端是什么请求
    switch (req->cmd_type()) {
        case 1:
            // 秘钥协商
            data = op->seckeyAgree(req);
            break;
        case 2:
            // 秘钥校验
            break;
        default:
            break;
    }

    // 释放资源
    delete fac;
    delete c;
    // tcp对象如何处理
    tcp->sendMsg(data);
    tcp->disConnect();
    op->m_list.erase(pthread_self());
    delete tcp;

    return nullptr;
}
