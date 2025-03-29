#ifndef CSFILEDEFS_H
#define CSFILEDEFS_H
#include <stdio.h>
// #include <stddef.h>
#ifndef OUT
#define OUT
#endif

#define CS_LINE_BUFFER_MAX	4096

typedef enum _CS_E_CODE_
{
	CS_E_OK			= 0x00000000,	//成功
	
	CS_E_HANDLE		= 0x80000000,	//不是有效的句柄
	CS_E_FILE		= 0x80000001,	//路径错误或文件不存在
	CS_E_PARAMETER	= 0x80000002,	//传入参数无效
}CS_E_CODE;

struct _CS_FILE_
{
	FILE* file;
	char fileName[260];
	void(*closeCall)(struct _CS_FILE_* csfile);
};


#ifdef __cplusplus
extern "C"
{
#endif
char 	    CS_IsValidHandle(void* handle);
#ifdef __cplusplus
}
#endif
#endif // CSFILEDEFS_H