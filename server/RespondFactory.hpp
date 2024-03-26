#pragma once
#include "CodecFactory.hpp"
#include "RespondCodec.hpp"
#include <iostream>
#include <memory>

// 响应工厂类，继承自编解码工厂类
class RespondFactory : public CodecFactory {
public:
    // 构造函数，根据编码字符串创建响应工厂对象
    RespondFactory(std::string enc) {
        m_flag = false;// 标志位设为 false，表示根据编码字符串创建 解码器
        m_encStr = enc;// 保存编码字符串
    }

    // 构造函数，根据响应信息对象创建响应工厂对象
    RespondFactory(RespondInfo *info) {
        m_flag = true;// 标志位设为 true，表示根据响应信息对象创建 编码器
        m_info = info;// 保存响应信息对象指针
    }

    // 创建编解码器对象
    Codec *createCodec() {
        Codec *codec = nullptr;
        if (m_flag) {
            m_ptr = make_shared<RespondCodec>(m_info);
        } else {
            m_ptr = make_shared<RespondCodec>(m_encStr);
        }
        return m_ptr.get();
    }

    // 析构函数
    ~RespondFactory() {}

private:
    bool m_flag;                        // 标志位，
    std::string m_encStr;               // 编码字符串
    RespondInfo *m_info;                // 响应信息对象指针
    std::shared_ptr<RespondCodec> m_ptr;// 响应编解码器智能指针
};
