#pragma once
#include <iostream>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
using namespace std;

const char RandX = 'x';

class BaseShm {
public:
    // 通过key打开共享内存
    BaseShm(int key) {
        getShmID(key, 0, 0);
    }
    // 通过传递进来的key创建/打开共享内存
    BaseShm(int key, int size) {
        getShmID(key, size, IPC_CREAT | 0664);
    }
    // 通过路径打开共享内存
    BaseShm(string name) {
        key_t key = ftok(name.data(), RandX);
        getShmID(key, 0, 0);
    }
    // 通过路径创建/打开共享内存
    BaseShm(string name, int size) {
        key_t key = ftok(name.data(), RandX);
        // 创建共享内存
        getShmID(key, size, IPC_CREAT | 0664);
    }
    void *mapShm() {
        // 关联当前进程和共享内存
        m_shmAddr = shmat(m_shmID, nullptr, 0);
        if (m_shmAddr == (void *) -1) {
            return nullptr;
        }
        return m_shmAddr;
    }
    int unmapShm() {
        int ret = shmdt(m_shmAddr);
        return ret;
    }
    int delShm() {
        int ret = shmctl(m_shmID, IPC_RMID, NULL);
        return ret;
    }
    ~BaseShm() {}

private:
    int getShmID(key_t key, int shmSize, int flag) {
        cout << "share memory size: " << shmSize << endl;
        m_shmID = shmget(key, shmSize, flag);
        if (m_shmID == -1) {
            // 写log日志
            cout << "shmget 失败" << endl;
        }
        return m_shmID;
    }

private:
    int m_shmID;

protected:
    void *m_shmAddr = nullptr;
};
