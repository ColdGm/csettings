#define _CRT_SECURE_NO_WARNINGS // ���ð�ȫ����

#include "stdio.h"
#include "csini.h"

int main(int argc, char* argv[])
{
    void* handle = NULL;
    CS_INI_Open(&handle, "../../TestDemo/config.ini");
    printf("���ǰ----------\n");
    CS_FOREACH_INI(i, handle)
	{
        printf("%s\t%s\t\t%s\n", i->group, i->key, i->value);
	}
    printf("\n----------���ú�\n");
    CS_INI_SetValue(handle, "lisi/ID", "ikunb");
    CS_INI_SetValue(handle, "lisiID", "bubi666");
    CS_INI_SetValue(handle, "lisi/PWD", "163666");
    CS_INI_SetValue(handle, "xiaowuID", "666");
    CS_FOREACH_INI(i, handle)
	{
        printf("%s\t%s\t\t%s\n", i->group, i->key, i->value);
	}
    CS_INI_Close(handle);
    //getchar();
    return 0;
}