#define _CRT_SECURE_NO_WARNINGS // 禁用安全警告
#include "csfileprivate.h"

struct _CS_FILE_LIST_ g_FileList = {0};

char CS_IsValidHandle(void* handle)
{
	struct _CS_NODE_* node = g_FileList.head;
	for (unsigned int i = 0; i < g_FileList.num; ++i)
	{
		if (node->csfile == handle)
			return 1;
		node = node->next;
	}
	return 0;
}

struct _CS_FILE_* _CS_OpenFile(const char* fileName, size_t csObjectSize)
{
    assert(csObjectSize >= sizeof(struct _CS_FILE_));
	FILE* file = fopen(fileName, "r");
	if (file == NULL)
	return NULL;
	struct _CS_NODE_** node = &g_FileList.head;
	while(*node)
		node = &(*node)->next;
	struct _CS_NODE_* newNode = malloc(sizeof(struct _CS_NODE_));
	struct _CS_FILE_* csFile = calloc(1, csObjectSize);
	csFile->file = file;
	strcpy(csFile->fileName, fileName);
	csFile->fileName[259] = 0;
	newNode->csfile = csFile;
	newNode->next = NULL;
	*node = newNode;
	g_FileList.num++;
	return csFile;
}

int _CS_CloseFile(void* handle)
{
	if (!CS_IsValidHandle(handle))
		return CS_E_HANDLE;
	struct _CS_NODE_* prev = NULL;
	struct _CS_NODE_* node = g_FileList.head;
	while(node)
	{
		if (node->csfile == handle)
			break;
		prev = node;
		node = node->next;
	}
    if(node == NULL)
        return CS_E_HANDLE;
    if(node->csfile->closeCall)
        node->csfile->closeCall(node->csfile);
	if (prev)
		prev->next = node->next;
	else
		g_FileList.head = node->next;
	free(node->csfile);
	free(node);
	return CS_E_OK;
}