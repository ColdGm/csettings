#define _CRT_SECURE_NO_WARNINGS // 禁用安全警告
#include "csini.h"
#include "csfileprivate.h"

int _CS_INI_Load(struct _CSINI_FILE_* csfile);
void _CS_CloseFileCall(struct _CS_FILE_* csfile);

int CS_INI_Open(OUT void** pHandle, const char* fileName)
{
	struct _CS_FILE_* csfile = _CS_OpenFile(fileName, sizeof(struct _CSINI_FILE_));
	if (csfile == NULL)
		return CS_E_FILE;
	int res = _CS_INI_Load((struct _CSINI_FILE_*)csfile);
	if (res == CS_E_OK)
		*pHandle = csfile;
	csfile->closeCall = &_CS_CloseFileCall;
	fclose(csfile->file);
	csfile->file = NULL;
	return res;
}

int CS_INI_Close(void *handle)
{
    return _CS_CloseFile(handle);
}

char* CS_INI_Value(void* handle, const char* key)
{
	if (!CS_IsValidHandle(handle))
		return NULL;
	if (key == NULL || key[0] == ' ' || key[0] == '#' || key[0] == ';' || key[0] == '=' || key[0] == '/')
		return NULL;
	int right = 0;
	int len = strlen(key);
	for(int i=0; i<len; ++i)
	{
		if(key[i] == ' ')
			return NULL;
		else if(key[i] == '/')
			right = i;
	}
	if(right == len - 1)
		return NULL;
	struct _CSINI_FILE_* inifile = (struct _CSINI_FILE_*)handle;
	struct _CSINI_PAIR_LISTNODE_* node = inifile->pairList.head;
	const char* elegroup = NULL;
	const char* elekey = NULL;
	while (node)
	{
		if(node->element->lineType == LINE_PAIR)
		{
			elegroup = node->element->content.pair.group;
			elekey = node->element->content.pair.key;
			if(right > 0)
			{
				if(0 == strncmp(elegroup, key, right - 1))
					if(0 == strcmp(elekey, key + right + 1))
						return node->element->content.pair.value;
			}
			else
			{
				if(0 == strcmp(elekey, key))
					return node->element->content.pair.value;
			}
		}
		node = node->next;
	}
	
	return NULL;
}

int CS_INI_SetValue(void* handle, const char* key, const char* value)
{
	if (!CS_IsValidHandle(handle))
		return CS_E_HANDLE;
	if (key == NULL || key[0] == ' ' || key[0] == '#' || key[0] == ';' || key[0] == '=' || key[0] == '/')
		return CS_E_PARAMETER;
	int right = 0;
	int len = strlen(key);
	for(int i=0; i<len; ++i)
	{
		if(key[i] == ' ')
			return CS_E_PARAMETER;
		else if(key[i] == '/')
			right = i;
	}
	if(right == len - 1)
		return CS_E_PARAMETER;
	struct _CSINI_FILE_* inifile = (struct _CSINI_FILE_*)handle;
	struct _CSINI_PAIR_LISTNODE_* node = inifile->pairList.head;
	struct _CSINI_PAIR_LISTNODE_* current = NULL;
	struct _CSINI_PAIR_LISTNODE_* insertPos = NULL;
	struct _CSINI_PAIR_LISTNODE_* lastNode = node;
	char* elegroup = NULL;
	char* elekey = NULL;
	while (node)
	{
		if(node->element->lineType == LINE_PAIR)
		{
			elegroup = node->element->content.pair.group;
			elekey = node->element->content.pair.key;
			if(right == 0)
			{
				if(node->element->content.pair.group == NULL)
					insertPos = node;
				if(elekey && 0 == strcmp(elekey, key))
				{
					current = node;
					break;
				}
			}
			else
			{
				if(elegroup && 0 == strncmp(elegroup, key, right - 1))
				{
					insertPos = node;
					if(0 == strcmp(elekey, key + right + 1))
					{
						current = node;
						break;
					}
				}
			}
		}
		if(node->next == NULL)
			lastNode = node;
		node = node->next;
	}
	//没有找到节点创建
	if(current == NULL)
	{
		//没有找到组创建
		if(insertPos == NULL)
		{
			node = malloc(sizeof(struct _CSINI_PAIR_LISTNODE_));
			node->element = calloc(1, sizeof(struct _CSINI_ELEMENT_));
			node->element->lineType = LINE_GROUP;
			if(right > 0)
			{
				node->element->content.pair.group = malloc(right + 1);
				strncpy(node->element->content.pair.group, key, right);
				node->element->content.pair.group[right] = 0;
			}
			insertPos = node;
			if(lastNode)
				lastNode->next = insertPos;
			else
				inifile->pairList.head = insertPos;
			inifile->pairList.length++;
		}
		node = malloc(sizeof(struct _CSINI_PAIR_LISTNODE_));
		node->element = calloc(1, sizeof(struct _CSINI_ELEMENT_));
		node->next = NULL;
		node->element->lineType = LINE_PAIR;
		node->element->content.pair.group = insertPos->element->content.pair.group;

		node->element->content.pair.key = malloc(len - right + 2);
		strcpy(node->element->content.pair.key, key + right + (right == 0 ? 0 : 1));
		inifile->pairList.length++;
		insertPos->next = node;
		current = node;
	}
	if(current->element->content.pair.value)
		free(current->element->content.pair.value);
	if(value)
	{
		len = strlen(value);
		node->element->content.pair.value = malloc(len + 1);
		strcpy(node->element->content.pair.value, value);
		node->element->content.pair.value[len] = 0;
	}
    return CS_E_OK;
}

int CS_INI_Sync(void *handle, const char *fileName)
{
	if (!CS_IsValidHandle(handle))
		return CS_E_HANDLE;
	struct _CSINI_FILE_* inifile = (struct _CSINI_FILE_*)(handle);
	if(fileName == NULL)
		fileName = inifile->csfile.fileName;
	inifile->csfile.file = fopen(fileName, "w");
	if(inifile->csfile.file == NULL)
		return CS_E_FILE;
	struct _CSINI_PAIR_LISTNODE_* node = inifile->pairList.head;
	char isFirstGloblePar = 1;
	while(node)
	{
		switch (node->element->lineType)
		{
		case LINE_GROUP:
			if(node->element->content.pair.group)
				fprintf(inifile->csfile.file, "[%s]\n", node->element->content.pair.group);
			break;
		case LINE_PAIR:
			if(node->element->content.pair.group == NULL && isFirstGloblePar) 
			{
				fputc('\n', inifile->csfile.file);
				isFirstGloblePar = 0;
			}
			fprintf(inifile->csfile.file, "%s = %s\n"
			, node->element->content.pair.key
			, node->element->content.pair.value ? node->element->content.pair.value : "");
			break;
		case LINE_COMMENT:
		case LINE_BLANK:
			fprintf(inifile->csfile.file, "%s\n", node->element->content.commit);
			break;
		default:
			break;
		}
		node = node->next;
	}
	fclose(inifile->csfile.file);
	return CS_E_OK;
}

int _CS_INI_Load(struct _CSINI_FILE_* inifile)
{
	struct _CSINI_PAIR_LISTNODE_** node = &inifile->pairList.head;

	char buffer[CS_LINE_BUFFER_MAX];
	int strLen = 0;
	char* curGroup = NULL;
	while (fgets(buffer, CS_LINE_BUFFER_MAX, inifile->csfile.file))
	{
		if (buffer[0] == ' ' || buffer[0] == '=')//非INI格式上下文跳过
			continue;
		buffer[CS_LINE_BUFFER_MAX - 1] = 0;	//防止没有\0导致strlen异常
		int length = strlen(buffer);

		struct _CSINI_PAIR_LISTNODE_* node = malloc(sizeof(struct _CSINI_PAIR_LISTNODE_));
		node->element = calloc(1, sizeof(struct _CSINI_ELEMENT_));
		node->next = NULL;
		int left = 0, right = length - 1;
		while (left <= right && (buffer[right] == ' ' || buffer[right] == '\n' || buffer[right] == '\r'))
			right--;// 去尾部空格和换行
		length = right + 1;
		if(buffer[0] == '#' || buffer[0] == ';') //注释
		{
			node->element->lineType = LINE_COMMENT;
			node->element->content.commit = malloc(length + 1);
			strncpy(node->element->content.commit, buffer, length);
			node->element->content.commit[length] = 0;
		}
		else if(buffer[0] == '\r' || buffer[0] == '\n')//空行
		{
			node->element->lineType = LINE_BLANK;
			node->element->content.commit = malloc(length + 1);
			strncpy(node->element->content.commit, buffer, length);
			node->element->content.commit[length] = 0;
			curGroup = NULL;
		}
		else //解析INI PAIR
		{
			if(buffer[0] == '[' && buffer[right] == ']') //解析组
			{
				left++; right--;
				while(left <= right && buffer[left] == ' ') left++;
				while(left <= right && buffer[right] == ' ') right--;
				right++; //去空格拿到的是下标
				if(right <= length)
				{
					node->element->lineType = LINE_GROUP;
					node->element->content.pair.group = malloc(right - left + 1);
					strncpy(node->element->content.pair.group, buffer + left, right - left);
					node->element->content.pair.group[right - left] = 0;
					curGroup = node->element->content.pair.group;
				}	
			}
			else //解析键值
			{
				right = left;
				while(right < 256 && buffer[right] != '=') right++;
				if(right == 255)
				continue;
				const int index = right;
				//解析键
				right--;
				while(left <= right && buffer[right] == ' ') right--;
				right++;
				if(left <= right)
				{
					node->element->lineType = LINE_PAIR;
					node->element->content.pair.group = curGroup;
					node->element->content.pair.key = malloc(right - left + 1);
					strncpy(node->element->content.pair.key, buffer, right - left);
					node->element->content.pair.key[right - left] = 0;
					//解析值
					left = index + 1;
					right = length - 1;
					while(left <= right && buffer[left] == ' ') left++;
					right++;
					if(left <= right)
					{
						node->element->content.pair.value = malloc(right - left + 1);
						strncpy(node->element->content.pair.value, buffer + left, right - left);
						node->element->content.pair.value[right - left] = 0;
					}
				}
			}
		}
		if(node->element->lineType != LINE_UNDEFINE)
		{
			struct _CSINI_PAIR_LISTNODE_** tail = &inifile->pairList.head;
			while (*tail)
				tail = &(*tail)->next;
			*tail = node;
			inifile->pairList.length++;
		}
		else
		{
			free(node->element);
			free(node);
		}
	}
	return CS_E_OK;
}

void _CS_CloseFileCall(struct _CS_FILE_* csfile)
{
	struct _CSINI_FILE_* iniFile = (struct _CSINI_FILE_*)csfile;
	struct _CSINI_PAIR_LISTNODE_* node = iniFile->pairList.head;
	struct _CSINI_PAIR_LISTNODE_* current = NULL;
	while (node)
	{
		current = node;
		node = node->next;

		if(current->element->lineType == LINE_PAIR)
		{
			free(current->element->content.pair.key);
			free(current->element->content.pair.value);
		}
		else if(current->element->lineType == LINE_GROUP)
			free(current->element->content.pair.group);
		else if(current->element->lineType == LINE_COMMENT
			|| current->element->lineType == LINE_BLANK)
			free(current->element->content.commit);
		
		free(current->element);
		free(current);
	}
}