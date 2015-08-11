/**
@file		strcmd.h
@date		2014/07/31
@author		WangChunyan
@version	1.0.0
@brief		�ַ������ӿ�

@note		
��Ҫ������HTTPЭ������ַ������в���
*/
#ifndef _STR_CMD_H_
#define _STR_CMD_H_
#include "advkill.h"

#define HTTP_METHOD_FLAG							"GET "
#define HTTP_METHOD_FLAG_LEN						4
#define HTTP_URL_END_FLAG							" HTTP/"
#define HTTP_URL_END_LEN							9
#define HTTP_HOST_FLAG								"Host: "
#define HTTP_HOST_FLAG_LEN							6
#define HTTP_REFERER_FLAG							"Referer: "
#define HTTP_REFERER_FLAG_LEN						9
#define HTTP_MIN_LEN								(HTTP_METHOD_FLAG_LEN + HTTP_URL_END_LEN + 2 + HTTP_HOST_FLAG_LEN + 2)

#define HTTP_LOCATION_PREFIX						"http://"

#define XUNLEI_KANKAN_DOMAIN_DIGIT_NUM				8
#define	XUNLEI_KANKAN_DOMAIN_DIGIT_INDEX			1
#define	DOMAIN_SEP_CHAR								'.'
#define DOMAIN_MATCH_CHAR							'*'		///< ����ģ��ƥ���ʶ
#define URL_EXCEPT_CHAR								'!'		///< URL�����ʶ
#define HOST_EXCEPT_CHAR							'!'		///< Host�����ʶ

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
char *get_http_field(char *httpstr, int httpstrlen, char *fieldflag, int fieldflaglen, char *field, int maxfieldlen, int *realfieldlen);

/**
��ȡHTTPЭ����URL����

@param httpstr HTTPЭ���ַ���
@param httpstrlen HTTPЭ���ַ�������
@param field Ҫ�����ֶ����ݵĵ�ַ
@param maxfieldlen Ҫ�����ֶ����ݵ���󳤶�
@param geturllen ��ȡ����URL��ʵ�ʳ���
@return ��ȡ����URL���ݵĵ�ַ
*/
char *get_http_get_url(char *httpstr, int httpstrlen, char *field, int maxfieldlen, int *geturllen);

/**
����Host��URL����location����

@param location Ҫ�����location���ݵĵ�ַ
@param maxlen Ҫ����location���ݵ���󳤶�
@param host location�����õ���Host
@param url location�����õ���URL
*/
void http_location_generate(char *location, int maxlen, char *host, char *url);

/**
���Host�����Ƿ�ȫΪIP��ַ��������

@param host Ҫ����Host����
@return ��IP��ַ���� ADV_KILL_OK�����򷵻� ADV_KILL_FAIL��
*/
int is_digit_host(char *host);

/**
����������Ƿ����ٰ���ָ������������

@param domain Ҫ������������
@param digitnum Ҫ�������������ٸ���
@return ����IP��ַ���� ADV_KILL_OK�����򷵻� ADV_KILL_FAIL��
*/
int domain_contain_digits(char *domain, int digitnum);

/**
��������Ƿ������ƥ��ģʽ

@param domain Ҫ������������
@param fieldindex �����ڼ�������ʼƥ��
@return �ɹ����� ƥ��ģʽ��ַ��ʧ�ܷ��� NULL��
*/
char * domain_regroup_match_mode(char *domain, int fieldindex);

/**
���ָ�����ýڵ����Ƿ����URL����ģʽ

@param surl ԭʼURL
@param node Ҫ�������ýڵ��ַ
@return ���Ƿ��� ADV_KILL_OK�����򷵻� ADV_KILL_FAIL��
*/
int is_contain_except_url(char *surl, struct advconf_hashnode *node);

#endif
