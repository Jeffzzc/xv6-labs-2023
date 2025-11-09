#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int main(int argc, char *argv[]) {
    // 缓冲区用于存储从标准输入读取的一行内容
    char buf[1024];
    // 存储要执行的命令及其参数，MAXARG是内核定义的最大参数数量
    char *args[MAXARG];
    int i, n;         // i用于循环，n记录当前读取的字符数
    char c;           // 临时存储读取的字符
    int r;            // 存储read系统调用的返回值

    // 检查是否提供了要执行的命令，xargs至少需要一个命令参数
    if (argc < 2) {
        fprintf(2, "xargs: missing command\n");  // 向标准错误输出错误信息
        exit(1);                                 // 异常退出
    }

    // 复制命令参数：将xargs的参数（除了xargs本身）作为要执行命令的初始参数
    // 例如"xargs echo bye"中，args会先存入["echo", "bye"]
    for (i = 1; i < argc; i++) {
        args[i-1] = argv[i];
    }

    // 主循环：持续从标准输入读取内容并处理
    while (1) {
        n = 0;          // 重置当前行的字符计数
        // 读取一行输入：一次读一个字符，直到换行或文件结束
        while (1) {
            // 从标准输入(文件描述符0)读取一个字符到c
            r = read(0, &c, 1);
            if (r <= 0) {
                // 读取失败或到达文件末尾(EOF)，退出内部循环
                break;
            }
            if (c == '\n') {
                // 遇到换行符，当前行结束，退出内部循环
                break;
            }
            // 确保缓冲区不会溢出，预留一个位置给终止符'\0'
            if (n < sizeof(buf) - 1) {
                buf[n++] = c;  // 将字符存入缓冲区并递增计数
            }
        }
        
        // 判断是否应该退出主循环：没有读取到内容且已到达输入末尾
        if (n == 0 && r <= 0) {
            break;
        }
        
        // 只有当读取到实际内容时才执行命令
        if (n > 0) {
            buf[n] = '\0';  // 为字符串添加终止符

            // 构建完整的命令参数列表：将读取的行作为最后一个参数
            // 例如原命令是["echo", "bye"]，添加"hello too"后变为["echo", "bye", "hello too", 0]
            args[argc-1] = buf;    // 输入内容作为新参数
            args[argc] = 0;        // 参数列表结束标志（NULL）

            // 创建子进程执行命令
            if (fork() == 0) {
                // 子进程：执行命令
                exec(args[0], args);
                // 如果exec失败（例如命令不存在），输出错误信息
                fprintf(2, "xargs: exec %s failed\n", args[0]);
                exit(1);  // 子进程异常退出
            } else {
                // 父进程：等待子进程执行完毕
                wait(0);
            }
        }

        // 如果已经到达输入末尾，退出主循环
        if (r <= 0) {
            break;
        }
    }

    exit(0);  // 正常退出
}
    