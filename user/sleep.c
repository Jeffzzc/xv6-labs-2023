#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]){
    if(argc != 2){
        fprintf(2, "sleep <seconds>\n");
        exit(1);
    }
    int seconds = atoi(argv[1]);
    int ret = sleep(seconds);
    exit(ret);
}