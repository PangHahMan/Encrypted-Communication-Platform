#pragma once
#include <iostream>
#include <openssl/bio.h> 
#include <openssl/err.h> 
#include <openssl/pem.h> 
#include <openssl/rsa.h> 
#include <string>
using namespace std;

enum SignLevel {
    Level1 = NID_md5,     // 使用MD5哈希算法
    Level2 = NID_sha1,    // 使用SHA1哈希算法
    Level3 = NID_sha224,  // 使用SHA224哈希算法
    Level4 = NID_sha256,  // 使用SHA256哈希算法
    Level5 = NID_sha384,  // 使用SHA384哈希算法
    Level6 = NID_sha512   // 使用SHA512哈希算法
};

class RsaCrypto {
public:
    RsaCrypto() {
        m_publicKey = RSA_new(); // 初始化公钥
        m_privateKey = RSA_new(); // 初始化私钥
    }
    RsaCrypto(string fileName, bool isPrivate = true) {
        m_publicKey = RSA_new(); // 初始化公钥
        m_privateKey = RSA_new(); // 初始化私钥
        if (isPrivate) {
            initPrivateKey(fileName); // 使用私钥文件初始化
        } else {
            initPublicKey(fileName); // 使用公钥文件初始化
        }
    }
    ~RsaCrypto() {
        RSA_free(m_publicKey); // 释放公钥
        RSA_free(m_privateKey); // 释放私钥
    }

    // 生成RSA密钥对
    void generateRsakey(int bits, string pub = "public.pem", string pri = "private.pem") {
        RSA *r = RSA_new(); // 创建一个RSA密钥对
        BIGNUM *e = BN_new(); // 创建一个BIGNUM对象
        BN_set_word(e, 456787); // 设置e值
        RSA_generate_key_ex(r, bits, e, NULL); // 生成密钥对

        // 将公钥写入文件
        BIO *pubIO = BIO_new_file(pub.data(), "w");
        PEM_write_bio_RSAPublicKey(pubIO, r);
        BIO_flush(pubIO); // 刷新缓冲区
        BIO_free(pubIO); // 释放BIO对象 

        // 将私钥写入文件
        BIO *priBio = BIO_new_file(pri.data(), "w");
        PEM_write_bio_RSAPrivateKey(priBio, r, NULL, NULL, 0, NULL, NULL);
        BIO_flush(priBio);
        BIO_free(priBio);

        // 从r中复制公钥和私钥
        m_privateKey = RSAPrivateKey_dup(r);
        m_publicKey = RSAPublicKey_dup(r);

        // 释放资源
        BN_free(e);
        RSA_free(r);
    }
    // 公钥加密
    string rsaPubKeyEncrypt(string data) {
        int keyLen = RSA_size(m_publicKey); // 获取公钥长度
        cout << "pubKey len: " << keyLen << endl;
        char *encode = new char[keyLen + 1]; // 分配加密缓冲区
        int ret = RSA_public_encrypt(data.size(), (const unsigned char *) data.data(),
                                     (unsigned char *) encode, m_publicKey, RSA_PKCS1_PADDING); // 公钥加密
        string retStr = string();
        if (ret >= 0) {
            retStr = string(encode, ret); // 将加密结果转换为字符串
        }
        delete[] encode; // 释放加密缓冲区
        return retStr;
    }
    // 私钥解密
    string rsaPriKeyDecrypt(string encData) {
        int keyLen = RSA_size(m_privateKey); // 获取私钥长度
        char *decode = new char[keyLen + 1]; // 分配解密缓冲区
        int ret = RSA_private_decrypt(encData.size(), (const unsigned char *) encData.data(),
                                      (unsigned char *) decode, m_privateKey, RSA_PKCS1_PADDING); // 私钥解密
        string retStr = string();
        if (ret >= 0) {
            retStr = string(decode, ret); // 将解密结果转换为字符串
        }
        delete[] decode; // 释放解密缓冲区
        return retStr;
    }

    // 使用RSA签名
    string rsaSign(string data, SignLevel level = Level3) {
        unsigned int len;
        char *signBuf = new char[RSA_size(m_privateKey) + 1]; // 分配签名缓冲区
        RSA_sign(level, (const unsigned char *) data.data(), data.size(), (unsigned char *) signBuf,
                 &len, m_privateKey); // 私钥签名
        cout << "sign len: " << len << endl;
        string retStr = string(signBuf, len); // 将签名结果转换为字符串
        delete[] signBuf; // 释放签名缓冲区
        return retStr;
    }
    // 使用RSA验证签名
    bool rsaVerify(string data, string signData, SignLevel level = Level3) {
        int ret = RSA_verify(level, (const unsigned char *) data.data(), data.size(),
                             (const unsigned char *) signData.data(), signData.size(), m_publicKey); // 验证签名
        return ret == 1;
    }

private:
    // 初始化公钥
    bool initPublicKey(string pubfile) {
        BIO *pubBio = BIO_new_file(pubfile.data(), "r");
        if (PEM_read_bio_RSAPublicKey(pubBio, &m_publicKey, NULL, NULL) == NULL) {
            ERR_print_errors_fp(stdout);
            return false;
        }
        BIO_free(pubBio);
        return true;
    }
    // 初始化私钥
    bool initPrivateKey(string prifile) {
        BIO *priBio = BIO_new_file(prifile.data(), "r");
        if (PEM_read_bio_RSAPrivateKey(priBio, &m_privateKey, NULL, NULL) == NULL) {
            ERR_print_errors_fp(stdout);
            return false;
        }
        BIO_free(priBio);
        return true;
    }

private:
    RSA *m_publicKey; // 公钥
    RSA *m_privateKey;// 私钥
};
