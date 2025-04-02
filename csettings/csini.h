#ifndef CSINI_H
#define CSINI_H
#include "csfiledefs.h"

#define CS_FOREACH_INI(_Def, _FileHandle) \
	if(CS_IsValidHandle(_FileHandle)) \
		for(const struct _CSINI_PAIR_LISTNODE_* _node_ = ((struct _CSINI_FILE_*)_FileHandle)->pairList.head; _node_ != NULL; _node_ = _node_->next) \
			if(_node_->element->lineType == LINE_PAIR) \
				for(const struct _CSINI_PAIR_* _Def = &_node_->element->content.pair; _Def; _Def = NULL)
//
struct _CSINI_PAIR_
{
	char* group;//��ΪNULL��ʾȫ�ֽڵ�
	char* key;	//��
	char* value;//ֵ
};

enum _CSINI_LINE_TYPE_
{
	LINE_UNDEFINE,
	LINE_COMMENT,	//ע��
	LINE_BLANK,		//����
	LINE_GROUP,		//��
	LINE_PAIR,		//��ֵ
};
				
struct _CSINI_ELEMENT_
{
	enum _CSINI_LINE_TYPE_ lineType;
	union
	{
		char* commit;	//ע�ͻ������ַ���
		struct _CSINI_PAIR_ pair;
	}content;
};
				
struct _CSINI_PAIR_LISTNODE_
{
	struct _CSINI_ELEMENT_* element;
	struct _CSINI_PAIR_LISTNODE_* next;
};
struct _CSINI_PAIR_LIST_
{
	struct _CSINI_PAIR_LISTNODE_* head;
	int length;
};
				
struct _CSINI_FILE_
{
	struct _CS_FILE_ csfile;
	struct _CSINI_PAIR_LIST_ pairList;
};

#ifdef __cplusplus
extern "C"
{
#endif
	int	CS_INI_Open(OUT void** pHandle, const char* fileName);
	int CS_INI_Close(void* handle);
	char* CS_INI_Value(void* handle, const char* key);
	int	CS_INI_SetValue(void* handle, const char* key, const char* value);
	int CS_INI_Sync(void* handle, const char* fileName);
#ifdef __cplusplus
}
#endif
#endif	// CSINI_H