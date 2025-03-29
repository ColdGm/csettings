#ifndef CSFILEPRRIVATE_H
#define CSFILEPRRIVATE_H

#include "csfiledefs.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct _CS_NODE_
{
	struct _CS_FILE_* csfile;
	struct _CS_NODE_* next;
};
struct _CS_FILE_LIST_
{
	struct _CS_NODE_* head;
	unsigned int num;
};


#ifdef __cplusplus
extern "C"
{
#endif
	struct _CS_FILE_* _CS_OpenFile(const char* fileName, size_t csObjectSize);
	int _CS_CloseFile(void* handle);

#ifdef __cplusplus
}
#endif

#endif //CSFILEPRRIVATE_H