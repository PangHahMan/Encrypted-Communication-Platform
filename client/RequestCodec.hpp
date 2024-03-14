#pragma once

#include "Codec.hpp"
#include "Message.pb.hpp"// 包含Protobuf消息定义
#include <iostream>
using namespace std;

// 请求信息的结构体
struct RequestInfo {
    int cmd;         // 命令类型
    string client_id;// 客户端ID
    string server_id;// 服务器ID
    string sign;     // 签名
    string data;     // 数据
};

// RequestCodec类，继承自Codec基类
class RequestCodec : public Codec {
private:
    // 解码场景的初始化函数
    void initMessage(string encstr) {
        m_encStr = encstr;
    }
    // 编码场景的初始化函数
    void initMessage(RequestInfo *info) {
        m_msg.set_cmd_type(info->cmd);
        m_msg.set_client_id(info->client_id);
        m_msg.set_server_id(info->server_id);
        m_msg.set_sign(info->sign);
        m_msg.set_data(info->data);
    }

public:
    RequestCodec() {}
    // 构造函数，用于解码场景，从编码后的字符串构造对象
    RequestCodec(string encstr) {
        initMessage(encstr);
    }
    // 构造函数，用于编码场景，从RequestInfo构造对象
    RequestCodec(RequestInfo *info) {
        initMessage(info);
    }

    // 重写基类函数 -> 序列化函数，返回序列化后的字符串
    string encodeMsg() {
        string output;
        m_msg.SerializeToString(&output);//将RequestMsg序列化成字符串
        return output;
    }
    // 重写基类函数 -> 反序列化函数，返回指向结构体/类对象的指针
    void *decodeMsg() {
        m_msg.ParseFromString(m_encStr);//编码后的字符串解码回去
        return &m_msg;
    }

    // 析构函数
    ~RequestCodec() {}

private:
    string m_encStr; // 编码后的字符串
    RequestMsg m_msg;// 请求消息对象,protobuf中的类
};
