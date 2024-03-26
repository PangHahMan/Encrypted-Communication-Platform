#pragma once
#include "Codec.hpp"
#include "Message.pb.hpp"
#include <iostream>
using namespace std;

// 响应信息结构体
struct RespondInfo {
    int status;      // 状态码
    int seckey_id;   // 安全密钥ID
    string client_id;// 客户端ID
    string server_id;// 服务器ID
    string data;     // 数据
};

// 响应编解码器类
class RespondCodec : public Codec {
private:
    void initMessage(string enc) {
        m_encstr = enc;
    }

    void initMessage(RespondInfo *info) {
        // 设置响应消息的各个字段
        m_msg.set_status(info->status);
        m_msg.set_seckey_id(info->seckey_id);
        m_msg.set_client_id(info->client_id);
        m_msg.set_server_id(info->server_id);
        m_msg.set_data(info->data);
    }

public:
    // 默认构造函数
    RespondCodec() {}

    // 带编码字符串参数的构造函数
    RespondCodec(string enc) {
        initMessage(enc);// 初始化消息
    }

    // 带响应信息对象参数的构造函数
    RespondCodec(RespondInfo *info) {
        initMessage(info);// 初始化消息
    }

    // 编码消息为字符串
    string encodeMsg() {
        string output;
        m_msg.SerializeToString(&output);// 将RespondInfo编码成字符串
        return output;                   // 返回编码后的字符串
    }

    // 解码字符串为消息对象
    void *decodeMsg() {
        m_msg.ParseFromString(m_encstr);// 将字符串解码成RespondInfo
        return &m_msg;                  // 返回消息对象指针
    }

    // 析构函数
    ~RespondCodec() {}

private:
    string m_encstr; // 编码字符串
    RespondMsg m_msg;// 响应消息对象
};
