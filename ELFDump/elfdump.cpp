//
// Created by hhoshino on 18/04/06.
//
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <elf.h>
#include <sys/stat.h>
#include <sys/mman.h>

//http://kozos.jp/kozos/linux.htmlを参考に以下を追加-----
typedef Elf64_Ehdr Elf_Ehdr;
typedef Elf64_Phdr Elf_Phdr;

#define IS_ELF(ehdr) ( \
	((ehdr).e_ident[EI_MAG0] == ELFMAG0) && \
	((ehdr).e_ident[EI_MAG1] == ELFMAG1) && \
	((ehdr).e_ident[EI_MAG2] == ELFMAG2) && \
	((ehdr).e_ident[EI_MAG3] == ELFMAG3))
//ここまで追加-------------------------------------------

static int elfdump(char *head) {

    //使用変数定義
    Elf64_Ehdr  *ehdr;
    Elf64_Shdr  *shdr, *shstr, *sym, *rel;
    Elf64_Phdr  *phdr;
    Elf64_Sym   *symp;
    Elf64_Rel   *relp;
    int         i, j, size;
    char        *sname;

    ehdr = (Elf64_Ehdr *)head;

    if ( !IS_ELF(*ehdr) ) {
        fprintf(stderr, "This is not ELF file.\n");
        return (1);

    }

    if (ehdr->e_ident[EI_CLASS] != ELFCLASS64) {
        fprintf(stderr, "unknown class. (%d)\n", (int)ehdr->e_ident[EI_CLASS]);
        return (1);
    }

    //ここの比較定数(ELFDATA2LSB)は要確認!!
    if (ehdr->e_ident[EI_DATA] != ELFDATA2LSB) {
        fprintf(stderr,"unknown endian. (%d)\n", (int)ehdr->e_ident[EI_DATA]);
        return (1);
    }

    //セクション名格納用セクション(.shstrtab)の取得
    shstr = (Elf64_Shdr *)(head + ehdr->e_shoff + ehdr->e_shentsize * ehdr->e_shstrndx);

    //セクション名一覧を表示
    printf("Sections name list: \n");
    for (i = 0; i < ehdr->e_shnum; i++) {
        shdr = (Elf64_Shdr *)(head + ehdr->e_shoff + ehdr->e_shentsize * i);
        sname = (char *)(head + shstr->sh_offset + shdr->sh_name);
        printf("\t[%d]\t%s\n", i, sname);
        //以下のロジックは不要な気がする...
//        if (!strcmp(sname,".strtab")) {
//            str = shdr;
//        }
    }

    //セグメント一覧を表示
    printf("Segments name list: \n");
    for (i = 0; i < ehdr->e_phnum; i++) {
        phdr = (Elf64_Phdr *)(head + ehdr->e_phentsize * i);
        printf("\t[%d]\t", i);

        for (j = 0; j < ehdr->e_shnum; j++) {
            shdr = (Elf64_Shdr *)(head + ehdr->e_shoff + ehdr->e_shentsize * j);
            size = (shdr->sh_type != SHT_NOBITS) ? shdr->sh_size : 0;

            if (shdr->sh_offset < phdr->p_offset) {
                continue;
            }

            if (shdr->sh_offset + size > phdr->p_offset + phdr->p_filesz) {
                continue;
            }

            sname = (char *)(head + shstr->sh_offset + shdr->sh_name);
            printf("%s", sname);

        }

        printf("\n");
    }

    //シンボル名一覧を表示
    printf("Symbol name list: \n");



}
