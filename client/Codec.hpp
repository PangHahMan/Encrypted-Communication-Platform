#pragma once
#include <iostream>
#include <string>
using namespace std;

// Codec 类定义了一个编解码器的接口
class Codec {
public:
    Codec() {}

    // 编码消息的虚拟函数，返回编码后的消息字符串
    virtual std::string encodeMsg() {
        return string();
    }

    // 解码消息的虚拟函数，返回解码后的消息指针
    virtual void *decodeMsg() {
        return nullptr;
    }

    virtual ~Codec() {}
};