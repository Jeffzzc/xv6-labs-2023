#include "kernel/types.h"
#include "user/user.h"

void sieve(int readfd){
    int p;
    if(read(readfd, &p, sizeof(p)) == 0){
        close(readfd);
        return;
    }
    printf("prime %d\n", p);

    int fd[2];
    pipe(fd);
    if(fork() == 0){
        //child process: filter multiples of p
        close(fd[1]);
        sieve(fd[0]);
        exit(0);
    }
    else{
        //parent process: continue sieving
        close(fd[0]);// 父进程不需要读新管道
        int x;
        while(read(readfd, &x, sizeof(x)) > 0){
            // 不被当前素数整除，传给子进程
            if(x % p != 0){
                write(fd[1], &x, sizeof(x));
            }
        }
        // 数据处理完毕，关闭文件描述符，等待子进程退出
        close(readfd);
        close(fd[1]);
        wait(0);// 等待子进程结束
    }
}
int main(int argc, char *argv[]) {
    int fd[2];
    pipe(fd);
    if(fork() == 0){
        //child process: generate numbers from 2 to 35
        close(fd[1]);
        sieve(fd[0]);
        exit(0);
    }
    else{
        //parent process: write numbers to pipe
        close(fd[0]);// 父进程只需要写端
        for(int i = 2; i <= 35; i++){
            write(fd[1], &i, sizeof(i));
        }
        close(fd[1]);// 写完后关闭写端
        wait(0);// 等待子进程结束
        exit(0);
    }
}