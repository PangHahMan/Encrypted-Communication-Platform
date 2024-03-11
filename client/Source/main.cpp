#include <iostream>
#include <string>
using namespace std;

int usage() {
    int selection = -1;
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

    scanf("%d", &selection);
    //清空缓冲区
    while (getchar() != '\n')
        ;

    return selection;
}

int main() {
    while (1) {
        int selection = usage();
        switch (selection) {
            case 1:

                break;
            case 2:

                break;
            case 3:

            default:
                break;
        }
    }
    cout << "客户端退出, bye,byte..." << endl;

    return 0;
}