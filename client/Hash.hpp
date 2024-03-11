#pragma once
#include <iostream>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <string.h>
using namespace std;

// 枚举HashType用来表示支持的哈希类型
enum HashType {
    MD5,
    SHA1,
    SHA224,
    SHA256,
    SHA384,
    SHA512
};

class Hash {
public:
    //根据指定的哈希类型初始化相应的哈希上下文
    Hash(HashType type) {
        m_type = type;
        switch (type) {
            case HashType::MD5:
                md5Init();
                break;
            case HashType::SHA1:
                sha1Init();
                break;
            case HashType::SHA224:
                sha224Init();
                break;
            case HashType::SHA256:
                sha256Init();
                break;
            case HashType::SHA384:
                sha384Init();
                break;
            case HashType::SHA512:
                sha512Init();
                break;
            default:
                md5Init();
                break;
        }
    }
    ~Hash() {
    }

    void addData(string data) {
        switch (m_type) {
            case HashType::MD5:
                md5AddData(data.data());
                break;
            case HashType::SHA1:
                sha1AddData(data.data());
                break;
            case HashType::SHA224:
                sha224AddData(data.data());
                break;
            case HashType::SHA256:
                sha256AddData(data.data());
                break;
            case HashType::SHA384:
                sha384AddData(data.data());
                break;
            case HashType::SHA512:
                sha512AddData(data.data());
                break;
            default:
                md5AddData(data.data());
                break;
        }
    }

    string result() {
        string result = string();
        switch (m_type) {
            case HashType::MD5:
                result = md5Result();
                break;
            case HashType::SHA1:
                result = sha1Result();
                break;
            case HashType::SHA224:
                result = sha224Result();
                break;
            case HashType::SHA256:
                result = sha256Result();
                break;
            case HashType::SHA384:
                result = sha384Result();
                break;
            case HashType::SHA512:
                result = sha512Result();
                break;
            default:
                result = md5Result();
                break;
        }
        return result;
    }

private:
    // md5
    inline void md5Init() {
        MD5_Init(&m_md5);
    }

    inline void md5AddData(const char *data) {
        MD5_Update(&m_md5, data, strlen(data));
    }

    string md5Result() {
        unsigned char md[MD5_DIGEST_LENGTH];
        char res[MD5_DIGEST_LENGTH * 2 + 1];
        MD5_Final(md, &m_md5);
        for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
            sprintf(&res[i * 2], "%02x", md[i]);
        }
        return string(res, MD5_DIGEST_LENGTH * 2 + 1);
    }

    // sha1
    inline void sha1Init() {
        SHA1_Init(&m_sha1);
    }

    inline void sha1AddData(const char *data) {
        SHA1_Update(&m_sha1, data, strlen(data));
    }

    string sha1Result() {
        unsigned char md[SHA_DIGEST_LENGTH];
        char res[SHA_DIGEST_LENGTH * 2 + 1];
        SHA1_Final(md, &m_sha1);
        for (int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
            sprintf(&res[i * 2], "%02x", md[i]);
        }
        return string(res, SHA_DIGEST_LENGTH * 2 + 1);
    }

    // sha224
    inline void sha224Init() {
        SHA224_Init(&m_sha224);
    }

    inline void sha224AddData(const char *data) {
        SHA224_Update(&m_sha224, data, strlen(data));
    }

    string sha224Result() {
        unsigned char md[SHA224_DIGEST_LENGTH];
        char res[SHA224_DIGEST_LENGTH * 2 + 1];
        SHA224_Final(md, &m_sha224);
        for (int i = 0; i < SHA224_DIGEST_LENGTH; ++i) {
            sprintf(&res[i * 2], "%02x", md[i]);
        }
        return string(res, SHA224_DIGEST_LENGTH * 2 + 1);
    }

    // sha256
    inline void sha256Init() {
        SHA256_Init(&m_sha256);
    }

    inline void sha256AddData(const char *data) {
        SHA256_Update(&m_sha256, data, strlen(data));
    }

    string sha256Result() {
        unsigned char md[SHA256_DIGEST_LENGTH];
        char res[SHA256_DIGEST_LENGTH * 2 + 1];
        SHA256_Final(md, &m_sha256);
        for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
            sprintf(&res[i * 2], "%02x", md[i]);
        }
        return string(res, SHA256_DIGEST_LENGTH * 2 + 1);
    }

    // sha384
    inline void sha384Init() {
        SHA384_Init(&m_sha384);
    }

    inline void sha384AddData(const char *data) {
        SHA384_Update(&m_sha384, data, strlen(data));
    }

    string sha384Result() {
        unsigned char md[SHA384_DIGEST_LENGTH];
        char res[SHA384_DIGEST_LENGTH * 2 + 1];
        SHA384_Final(md, &m_sha384);
        for (int i = 0; i < SHA384_DIGEST_LENGTH; ++i) {
            sprintf(&res[i * 2], "%02x", md[i]);
        }
        return string(res, SHA384_DIGEST_LENGTH * 2 + 1);
    }

    // sha512
    inline void sha512Init() {
        SHA512_Init(&m_sha512);
    }

    inline void sha512AddData(const char *data) {
        SHA512_Update(&m_sha512, data, strlen(data));
    }

    string sha512Result() {
        unsigned char md[SHA512_DIGEST_LENGTH];
        char res[SHA512_DIGEST_LENGTH * 2 + 1];
        SHA512_Final(md, &m_sha512);
        for (int i = 0; i < SHA512_DIGEST_LENGTH; ++i) {
            sprintf(&res[i * 2], "%02x", md[i]);
        }
        return string(res, SHA512_DIGEST_LENGTH * 2 + 1);
    }

private:
    HashType m_type;
    MD5_CTX m_md5;
    SHA_CTX m_sha1;
    SHA256_CTX m_sha224;
    SHA256_CTX m_sha256;
    SHA512_CTX m_sha384;
    SHA512_CTX m_sha512;
};


/* 解释：


 */