#pragma once
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
using namespace std;

/* 用于通信的套接字类 */
// 超时的时间
static const int TIMEOUT = 10000;
class TcpSocket {
public:
    enum ErrorType {
        ParamError = 3001,
        TimeoutError,
        PeerCloseError,
        MallocError
    };
    TcpSocket() {
    }
    // 使用一个可以用于通信的套接字实例化套接字对象
    TcpSocket(int connfd) {
        socket_fd = connfd;
    }
    ~TcpSocket() {
    }

private:
    // 设置I/O为非阻塞模式
    /*  fcntl用于获取和设置文件描述符的标志位，包括非阻塞模式、文件状态标志
    int fcntl(int __fd, int __cmd, ...)
    fd：文件描述符，表示要操作的文件或套接字。
    cmd：表示要执行的操作，F_GETFL获取标志位，F_SETFL设置标志位。
    arg：可选参数，根据不同的操作而变化。 */
    int setNonBlock(int fd) {
        //Args: fd(int):文件描述符    returns：成功返回0，失败返回-1
        //获取当前文件描述符的标志位
        int flags = fcntl(fd, F_GETFL);//获取标志位
        if (flags == -1) {
            return flags;
        }
        // 设置 O_NONBLOCK 标志位
        flags |= O_NONBLOCK;
        int ret = fcntl(fd, F_SETFL, flags);//设置标志位flags给fd文件描述符
        return ret;
    }

    // 设置I/O为阻塞模式
    int setBlock(int fd) {
        int ret = 0;
        int flags = fcntl(fd, F_GETFL);
        if (flags == -1) {
            return flags;
        }

        flags &= ~O_NONBLOCK;//清楚标志位的O_NONBLOCK属性
        ret = fcntl(fd, F_SETFL, flags);
        return ret;
    }

    /**
    * 函数功能：设置一个读取超时的定时器
    * 参数：wait_seconds - 等待的秒数
    * 返回值：
    *     -1：发生错误或超时
    *      0：读取事件发生
    * 
    * 注：socket_fd是一个全局变量或者作为参数传入的套接字描述符
    */
    int readTimeout(unsigned int wait_seconds) {
        int ret = 0;
        if (wait_seconds > 0) {
            fd_set read_fdset;     // 读文件描述符集合
            struct timeval timeout;// 超时时间结构体

            FD_ZERO(&read_fdset);          // 初始化文件描述符集合
            FD_SET(socket_fd, &read_fdset);// 添加要监听的文件描述符到集合中

            timeout.tv_sec = wait_seconds;// 设置超时秒数
            timeout.tv_usec = 0;          // 设置微秒数

            // 循环调用select函数直到成功或者遇到非中断错误
            // select返回值三态:
            // 1. 若timeout时间到（超时），没有检测到读事件，ret返回=0
            // 2. 若ret返回<0 &&  errno == EINTR 说明select的过程中被别的信号中断（可中断睡眠原理）:
            //    2-1. 若返回-1，select出错
            // 3. 若ret返回值>0 表示有read事件发生，返回事件发生的个数
            do {
                ret = select(socket_fd + 1, &read_fdset, NULL, NULL, &timeout);
            } while (ret < 0 && errno == EINTR);

            // 根据select的返回值判断是否发生错误、超时或者有读取事件发生
            if (ret == 0) {
                ret = -1;         // 超时
                errno = ETIMEDOUT;//ETIMEDOUT 是一个错误码（errno），表示操作超时
            } else if (ret == 1) {
                ret = 0;// 有读取事件发生
            }
        }

        return ret;
    }

    // 写超时检测函数, 不包含写操作
    int writeTimeout(unsigned int wait_seconds) {
        int ret = 0;
        if (wait_seconds > 0) {
            fd_set write_fdset;
            struct timeval timeout;

            FD_ZERO(&write_fdset);
            FD_SET(socket_fd, &write_fdset);
            timeout.tv_sec = wait_seconds;
            timeout.tv_usec = 0;
            do {
                ret = select(socket_fd + 1, NULL, &write_fdset, NULL, &timeout);
            } while (ret < 0 && errno == EINTR);

            // 超时
            if (ret == 0) {
                ret = -1;
                errno = ETIMEDOUT;//ETIMEDOUT 是一个错误码（errno），表示操作超时
            } else if (ret == 1) {
                ret = 0;// 没超时
            }
        }

        return ret;
    }
    /* 带连接超时的connect函数
    * connectTimeout - connect
    * @addr: 要连接的对方地址
    * @wait_seconds: 等待超时秒数，如果为0表示正常模式
    * 成功（未超时）返回0，失败返回-1，超时返回-1并且errno = ETIMEDOUT
    */
    int connectTimeout(struct sockaddr_in *addr, unsigned int wait_seconds) {
        int ret;
        socklen_t addrlen = sizeof(struct sockaddr_in);

        if (wait_seconds > 0) {
            setNonBlock(socket_fd);// 设置非阻塞IO
        }

        ret = connect(socket_fd, (struct sockaddr *) addr, addrlen);
        // 非阻塞模式连接, 返回-1, 并且errno为EINPROGRESS, 表示连接正在进行中
        if (ret < 0 && errno == EINPROGRESS) {
            fd_set connect_fdset;
            struct timeval timeout;
            FD_ZERO(&connect_fdset);
            FD_SET(socket_fd, &connect_fdset);
            timeout.tv_sec = wait_seconds;
            timeout.tv_usec = 0;
            do {
                // 一但连接建立，则套接字就可写 所以connect_fdset放在了写集合中
                ret = select(socket_fd + 1, NULL, &connect_fdset, NULL, &timeout);
            } while (ret < 0 && errno == EINTR);

            if (ret == 0) {
                // 超时
                ret = -1;
                errno = ETIMEDOUT;
            } else if (ret < 0) {
                return -1;
            } else if (ret == 1) {
                /* ret返回为1（表示套接字可写），可能有两种情况，一种是连接建立成功，一种是套接字产生错误，*/
                /* 此时错误信息不会保存至errno变量中，因此，需要调用getsockopt来获取。 */
                int err;
                socklen_t sockLen = sizeof(err);
                int sockoptret = getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, &err, &sockLen);
                if (sockoptret == -1) {
                    return -1;
                }
                if (err == 0) {
                    ret = 0;// 成功建立连接
                } else {
                    // 连接失败
                    errno = err;
                    ret = -1;
                }
            }
        }
        if (wait_seconds > 0) {
            setBlock(socket_fd);// 套接字设置回阻塞模式
        }
        return ret;
    }
    /**
    * 函数功能：从套接字缓冲区中读取指定数量的字符
    * 参数：buf - 存放读取结果的缓冲区
    *      count - 需要读取的字符数量
    * 返回值：
    *     -1：读取出错
    *     其他：实际读取的字符数量
    */
    int readn(void *buf, int count) {
        size_t nleft = count;     // 剩余需要读取的字符数量
        ssize_t nread;            // 实际读取的字符数量
        char *bufp = (char *) buf;// 缓冲区指针
        //循环读取
        while (nleft > 0) {
            // 读取数据,从fd中读取nleft字节数据到bufp缓冲区中
            // read函数返回值： >0 读取具体的字节数 ==0表示读取到了文件结束（EOF），即到达文件末尾。 <0 发生错误
            if ((nread = read(socket_fd, bufp, nleft)) < 0) {
                if (errno == EINTR) {// 如果被中断则继续读取
                    continue;
                }
                return -1;// 读取出错
            } else if (nread == 0) {
                return count - nleft;// 读取完毕，返回实际读取的字符数量
            } else {
                bufp += nread; // 更新缓冲区指针位置
                nleft -= nread;// 更新剩余需要读取的字符数量
            }
        }

        return count;// 返回读取的字符数量
    }
    /**
    * 函数功能：向套接字中写入指定数量的字符
    * 参数：buf - 需要写入的数据缓冲区
    *      count - 需要写入的字符数量
    * 返回值：
    *     -1：写入出错
    *     其他：实际写入的字符数量
    * 
    */
    int writen(const void *buf, int count) {
        size_t nleft = count;                 // 剩余需要写入的字符数量
        ssize_t nwritten;                     // 实际写入的字符数量
        const char *bufp = (const char *) buf;// 缓冲区指针
        //循环写入
        while (nleft > 0) {
            // 写入数据，write函数从bufp中写入nleft字节的数据到socket_fd中
            if ((nwritten = write(socket_fd, bufp, nleft)) < 0) {
                if (errno == EINTR) {// 如果被中断则继续写入
                    continue;
                }
                return -1;// 写入出错
            } else if (nwritten == 0) {
                continue;// 没有写入任何数据，继续写入
            }

            bufp += nwritten; // 更新缓冲区指针位置
            nleft -= nwritten;// 更新剩余需要写入的字符数量
        }

        return count;// 返回写入的字符数量
    }

public:
    /**
    * 函数功能：连接到指定的主机
    * 参数：ip - 主机的IP地址
    *      port - 主机的端口号
    *      timeout - 连接超时时间（默认为10秒）
    * 返回值：
    *      0：连接成功
    *      ParamError：参数错误
    *      errno：连接过程中出现的错误码
    *      TimeoutError：连接超时
    */
    int connectToHost(std::string ip, unsigned short port, int timeout = TIMEOUT) {
        int ret = 0;

        // 检查参数合法性
        if (port <= 0 || port > 65535 || timeout < 0) {
            ret = ParamError;
            return ret;
        }

        // 创建套接字
        socket_fd = socket(AF_INET, SOCK_STREAM, 0);// TCP
        if (socket_fd < 0) {
            ret = errno;
            printf("func socket() err:%d\n", ret);
            return ret;
        }

        // 设置服务器地址结构
        struct sockaddr_in servaddr;
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(port);
        servaddr.sin_addr.s_addr = inet_addr(ip.data());

        // 进行连接，并设置超时
        ret = connectTimeout((struct sockaddr_in *) (&servaddr), (unsigned int) timeout);
        if (ret < 0) {
            // 超时
            if (ret == -1 && errno == ETIMEDOUT) {
                ret = TimeoutError;
                return ret;
            } else {
                printf("connectTimeout 调用异常, 错误号: %d\n", errno);
                return errno;
            }
        }
        //ret > 0
        return ret;
    }
    /**
    * 函数功能：发送消息到服务器
    * 参数：sendData - 待发送的数据
    *      timeout - 发送超时时间
    * 返回值：
    *      0：发送成功
    *      -1：发送超时或发生其他错误
    *      MallocError：内存分配错误
    *      TimeoutError：发送超时
    * 
    */
    int sendMsg(std::string sendData, int timeout = TIMEOUT) {
        // 返回0->没超时, 返回-1->超时
        int ret = writeTimeout(timeout);
        if (ret == 0) {
            int writed = 0;
            int dataLen = sendData.size() + 4;
            // 添加的4字节作为数据头, 存储数据块长度
            unsigned char *netdata = (unsigned char *) malloc(dataLen);
            if (netdata == NULL) {
                ret = MallocError;
                printf("func sckClient_send() mlloc Err:%d\n ", ret);
                return ret;
            }
            // 转换为网络字节序
            int netlen = htonl(sendData.size());
            memcpy(netdata, &netlen, 4);
            memcpy(netdata + 4, sendData.data(), sendData.size());

            // 没问题返回发送的实际字节数, 应该 == 第二个参数: dataLen
            // 失败返回: -1
            writed = writen(netdata, dataLen);
            if (writed < dataLen)// 发送失败
            {
                if (netdata != NULL) {
                    free(netdata);
                    netdata = NULL;
                }
                return writed;
            }
            // 释放内存
            if (netdata != NULL) {
                free(netdata);
                netdata = NULL;
            }
        } else {
            //失败返回-1，超时返回-1并且errno = ETIMEDOUT
            if (ret == -1 && errno == ETIMEDOUT) {
                ret = TimeoutError;
                printf("func sckClient_send() mlloc Err:%d\n ", ret);
            }
        }

        return ret;
    }
    /**
    * 函数功能：接收数据
    * 参数：timeout - 接收超时时间
    * 返回值：
    *      接收到的数据字符串
    *      如果超时或发生异常，则返回空字符串
    */
    std::string recvMsg(int timeout = TIMEOUT) {
        // 返回0 -> 没超时就接收到了数据, -1, 超时或有异常
        int ret = readTimeout(timeout);
        if (ret != 0) {
            if (ret == -1 || errno == ETIMEDOUT) {
                printf("readTimeout(timeout) err: TimeoutError \n");
                return std::string();
            } else {
                printf("readTimeout(timeout) err: %d \n", ret);
                return std::string();
            }
        }

        // 读取包头
        int netdatalen = 0;
        ret = readn(&netdatalen, 4);// 读取4个字节的包头
        if (ret == -1) {
            printf("func readn() err:%d \n", ret);
            return std::string();
        } else if (ret < 4) {
            printf("func readn() err peer closed:%d \n", ret);
            return std::string();
        }

        int n = ntohl(netdatalen);// 将网络字节序转换为主机字节序

        // 根据包头中记录的数据大小申请内存, 接收数据
        char *tmpBuf = (char *) malloc(n + 1);// 多分配一个字节用于存放字符串结束符 '\0'
        if (tmpBuf == NULL) {
            ret = MallocError;
            printf("malloc() err \n");
            return std::string();
        }

        ret = readn(tmpBuf, n);// 根据长度读取数据
        if (ret == -1) {
            printf("func readn() err:%d \n", ret);
            return std::string();
        } else if (ret < n) {
            printf("func readn() err peer closed:%d \n", ret);
            return std::string();
        }

        tmpBuf[n] = '\0';// 将接收到的数据转换为字符串，添加字符串结束符 '\0'
        std::string data = std::string(tmpBuf);
        // 释放内存
        free(tmpBuf);

        return data;
    }
    // 断开连接
    void disConnect() {
        if (socket_fd >= 0) {
            close(socket_fd);
        }
    }

private:
    int socket_fd;// 用于通信的套接字
};
