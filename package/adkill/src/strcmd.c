/**
@file		strcmd.c
@date		2014/07/31
@author		WangChunyan
@version	1.0.0
@brief		�ַ������ӿ�

@note		
��Ҫ������HTTPЭ������ַ������в���
*/

#include "strcmd.h"

#ifdef ADVKILL_CHECK_MEM
extern unsigned long long int g_calloc_times;
extern unsigned long long int g_calloc_size;
extern unsigned long long int g_free_times;
extern unsigned long long int g_free_size;
#endif

/**
�����ֶα�ʶ��ȡHTTPЭ�����ֶ�����

@param httpstr HTTPЭ���ַ���
@param httpstrlen HTTPЭ���ַ�������
@param fieldflag Ҫ��ȡ�ֶεı�ʶ
@param fieldflaglen Ҫ��ȡ�ֶεı�ʶ����
@param field Ҫ�����ֶ����ݵĵ�ַ
@param maxfieldlen Ҫ�����ֶ����ݵ���󳤶�
@param realfieldlen ��ȡ�����ֶ����ݵ�ʵ�ʳ���
@return ��ȡ�����ֶ����ݵĵ�ַ
*/
char *get_http_field(char *httpstr, int httpstrlen, char *fieldflag, int fieldflaglen, char *field, int maxfieldlen, int *realfieldlen)
{
	char *beginpos = NULL;
	char *endpos = NULL;
	char *tmppos = NULL;
	int len = 0;

	if (!httpstr || !fieldflag || !field || !realfieldlen)
		return NULL;
	endpos = (httpstr + httpstrlen);
	beginpos = strstr(httpstr, fieldflag);
	if (beginpos == NULL)
		return NULL;
	beginpos += fieldflaglen;
	tmppos = beginpos;
	while(*tmppos != '\r' && tmppos < endpos)
	{
		len++;
		tmppos++;
	}
	if (len <= 0)
		return NULL;
	if (len > (maxfieldlen -1))
	{
		*realfieldlen = (maxfieldlen -1);
		memcpy(field, beginpos, maxfieldlen -1);
		return field;
	}

	*realfieldlen = len;
	memcpy(field, beginpos, len);
	return field;
}

/**
��ȡHTTPЭ����URL����

@param httpstr HTTPЭ���ַ���
@param httpstrlen HTTPЭ���ַ�������
@param field Ҫ�����ֶ����ݵĵ�ַ
@param maxfieldlen Ҫ�����ֶ����ݵ���󳤶�
@param geturllen ��ȡ����URL��ʵ�ʳ���
@return ��ȡ����URL���ݵĵ�ַ
*/
char *get_http_get_url(char *httpstr, int httpstrlen, char *field, int maxfieldlen, int *geturllen)
{
	char *beginpos = NULL;
	char *endpos = NULL;
	int len = 0;

	if (!httpstr || !geturllen)
		return NULL;

	beginpos = httpstr;
	beginpos += HTTP_METHOD_FLAG_LEN;
	endpos = strstr(beginpos, HTTP_URL_END_FLAG);

	if (endpos == NULL)
		return NULL;
	len = (endpos - beginpos);
	if(len <= 0 || len > (maxfieldlen - 1))
		return NULL;

	*geturllen = len;
	memcpy(field, beginpos, len);
	return field;
}

/**
����Host��URL����location����

@param location Ҫ�����location���ݵĵ�ַ
@param maxlen Ҫ����location���ݵ���󳤶�
@param host location�����õ���Host
@param url location�����õ���URL
*/
void http_location_generate(char *location, int maxlen, char *host, char *url)
{
	if(!location || !host || !url)
		return;
	snprintf(location, maxlen, "%s%s%s", HTTP_LOCATION_PREFIX, host, url);
}

/**
���Host�����Ƿ�ȫΪIP��ַ��������

@param host Ҫ����Host����
@return ��IP��ַ���� ADV_KILL_OK�����򷵻� ADV_KILL_FAIL��
*/
int is_digit_host(char *host)
{
	char *tmphost = NULL;
	
	if(!host)
		return ADV_KILL_FAIL;
	tmphost = host;
	while(*tmphost != '\0')
	{
		if (*tmphost < '.' || *tmphost > '9')
		{
			return ADV_KILL_FAIL;
		}
		tmphost++;
	}
	return ADV_KILL_OK;
}

/**
����������Ƿ����ٰ���ָ������������

@param domain Ҫ������������
@param digitnum Ҫ�������������ٸ���
@return ����IP��ַ���� ADV_KILL_OK�����򷵻� ADV_KILL_FAIL��
*/
int domain_contain_digits(char *domain, int digitnum)
{
	char *tmpdomain = NULL;
	int tmpnum = 0;
	int maxlen = 0;
	int i = 0;

	if (!domain || digitnum <= 0)
		return ADV_KILL_FAIL;
	tmpdomain = domain;
	maxlen = strlen(tmpdomain);

	for (i=0; i<maxlen; i++)
	{
		if (*tmpdomain >= '0' && *tmpdomain <= '9')
		{
			tmpnum++;
		}
		else
		{
			tmpnum = 0;
		}
		if (tmpnum >= digitnum)
			return ADV_KILL_OK;
		tmpdomain++;
	}
	return ADV_KILL_FAIL;
}

/**
��������Ƿ������ƥ��ģʽ

@param domain Ҫ������������
@param fieldindex �����ڼ�������ʼƥ��
@return �ɹ����� ƥ��ģʽ��ַ��ʧ�ܷ��� NULL��
*/
char * domain_regroup_match_mode(char *domain, int fieldindex)
{
	int index = 0;
	int maxlen = 0;
	int fieldnum = 0;

	if (!domain || fieldindex <= 0)
	{
		return NULL;
	}
	maxlen = strlen(domain);
	for (index=0; index<maxlen; index++)
	{
		if (*domain == DOMAIN_SEP_CHAR)
			fieldnum++;
		if (fieldnum == fieldindex)
		{
			domain--;
			*domain = DOMAIN_MATCH_CHAR;
			return domain;
		}
		domain++;
	}
	return NULL;
}

/**
���ָ�����ýڵ����Ƿ����URL����ģʽ

@param surl ԭʼURL
@param node Ҫ�������ýڵ��ַ
@return ���Ƿ��� ADV_KILL_OK�����򷵻� ADV_KILL_FAIL��
*/
int is_contain_except_url(char *surl, struct advconf_hashnode *node)
{
	int i = 0;
	char *tmppos = NULL;

	if(!node || !surl || node->mapnum <= 0)
		return ADV_KILL_FAIL;
	for (i=0; i<node->mapnum; i++)
	{
		tmppos = strchr(node->map[i].durl, URL_EXCEPT_CHAR);
		if (tmppos != NULL)
		{
			tmppos++;
			if (strstr(surl, tmppos) != NULL)
				return ADV_KILL_OK;
		}
	}
	return ADV_KILL_FAIL;
}
