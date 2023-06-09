#ifndef UTIL_H
#define UTIL_H

#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <signal.h>
#include <cstring>

//util类
class Util {

public:
    /*
    * 文件描述符操作
    */

    /*设置文件描述符非阻塞*/
    static void setNonBlock(int fd) {
        int old_flag = fcntl(fd, F_GETFL);
        int new_flag = old_flag | O_NONBLOCK;
        fcntl(fd, F_SETFL, new_flag);
    }

    /*添加文件描述符到epoll 监听读事件*/
    static void addFd(int epoll_fd, int fd, bool one_shot) {
        epoll_event event;
        event.data.fd = fd;
        // 监测读事件/对端关闭写端事件，并设置为边缘触发
        event.events = EPOLLIN | EPOLLRDHUP | EPOLLET;

        if (one_shot) {
            event.events |= EPOLLONESHOT;
        }

        // 添加文件描述符到epoll中
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);
        // 设置文件描述符非阻塞
        setNonBlock(fd);
    }

    /*从epoll中移除文件描述符*/
    static void removeFd(int epoll_fd, int fd) {
        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, 0);
        close(fd);
    }

    /*修改文件描述符,重置EPOLLONESHOT事件，使得下次的读事件可以触发*/
    static void modifyFd(int epoll_fd, int fd, int ev) {
        epoll_event event;
        event.data.fd = fd;
        // ev为传入事件的类型，比如EPOLLIN、EPOLLOUT
        // 重置EPOLLONESHOT事件
        event.events = ev | EPOLLONESHOT | EPOLLRDHUP;
        //设置
        epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &event);
    }

    /*添加信号捕捉*/
    static void addSig(int sig, void (*handler) (int)) {
        struct sigaction sa;
        memset(&sa, '\0', sizeof(sa));
        sa.sa_handler = handler;    // 设置sa处理的handler(函数指针，使用 SIG_IGN 表示忽略该信号)
        sigfillset(&sa.sa_mask);    // 是否应改为 sigemptyset
        sigaction(sig, &sa, NULL);  // 信号捕捉函数，sig是需要捕捉的信号，sa是描述如何处理
    }

    /*生成10位sessionid*/
    static std::string makeSesId() {
        std::string res = "";
        srand(time(NULL));

        for (int i = 0; i < 10; i ++ ) {
            int type = rand() % 3;
            if (type == 0) {
                res += '0' + rand() % 10;
            } else if (type == 1) {
                res += 'a' + rand() % 26;
            } else if (type == 2) {
                res += 'A' + rand() % 26;
            }
        }
        return res;
    }


};

#endif