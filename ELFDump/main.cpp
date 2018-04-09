#include "elfdump.cpp"

int main(int argc, char *argv[]) {

    int fd;
    struct stat sb;
    char *head;

    fd = open(argv[1],O_RDONLY);

    if (fd <0) {
        //異常終了
        exit(1);
    }

    fstat(fd,&sb);
    head = (char *)mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);

    //ELF出力処理実行
    elfdump(head);

    munmap(head,sb.st_size);

    close(fd);

    return 0;
}