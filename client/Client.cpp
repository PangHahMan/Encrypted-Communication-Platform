#include "ClientOP.hpp"
#include <iostream>
#include <string>
using namespace std;

int usage();
int main() {
     cout << "开始选择" << endl;
    // 创建客户端操作类对象
    ClientOP op("resource/client.json");
    while (true) {
        int sel = usage();
        cout << "开始选择" << endl;
        switch (sel) {
            case 1:
                // 秘钥协商
                op.seckeyAgree();
                break;
            case 2:
                op.seckeyCheck();
                // 秘钥校验
                break;
            case 3:
                // 秘钥注销
                op.seckeyLogOut();
            default:
                break;
        }
    }
    cout << "客户端退出, bye,byte..." << endl;
    return 0;
}

int usage() {
    int nSel = -1;
    printf("\n  /*************************************************************/");
    printf("\n  /*************************************************************/");
    printf("\n  /*     1.密钥协商                                            */");
    printf("\n  /*     2.密钥校验                                            */");
    printf("\n  /*     3.密钥注销                                            */");
    printf("\n  /*     4.密钥查看                                            */");
    printf("\n  /*     0.退出系统                                            */");
    printf("\n  /*************************************************************/");
    printf("\n  /*************************************************************/");
    printf("\n\n  选择:");

    scanf("%d", &nSel);
    while (getchar() != '\n')
        ;

    return nSel;
}