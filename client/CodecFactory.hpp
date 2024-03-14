#pragma once
#include "Codec.hpp"

// CodecFactory 类是一个编解码器工厂，用于创建 Codec 对象
class CodecFactory {
public:
    CodecFactory() {}
    
    // 创建 Codec 对象的虚拟函数，默认实现返回 nullptr
    virtual Codec *createCodec() {
        return nullptr;
    }

    virtual ~CodecFactory() {}
};