#pragma once
#include "Codec.hpp"
#include "CodecFactory.hpp"
#include "RequestCodec.hpp"
#include <iostream>

// 请求工厂类，继承自编解码工厂类
class RequestFactory : public CodecFactory {
public:
    // 构造函数，根据编码字符串创建请求工厂对象
    RequestFactory(std::string enc) {
        m_flag = false;// 标志位设为 false，表示根据编码字符串创建 解码器
        m_encStr = enc;// 保存编码字符串
    }

    // 构造函数，根据请求信息对象创建请求工厂对象
    RequestFactory(RequestInfo *info) {
        m_flag = true;// 标志位设为 true，表示根据请求信息对象创建 编码器
        m_info = info;// 保存请求信息对象指针
    }

    // 创建编解码器对象
    Codec *createCodec() {
        Codec *codec = nullptr;
        if (m_flag) {// 如果标志位为 true，根据请求信息对象 编码器
            codec = new RequestCodec(m_info);
        } else {// 如果标志位为 false，根据编码字符串创建 解码器
            codec = new RequestCodec(m_encStr);
        }
        return codec;// 返回创建的编解码器对象
    }

    // 析构函数
    ~RequestFactory() {}

private:
    bool m_flag;         // 标志位，字符串->false表示解码器，info对象->true 表示编码器
    std::string m_encStr;// 编码字符串
    RequestInfo *m_info; // 请求信息对象指针
};
