/**
@file		advhash.h
@date		2014/07/31
@author		WangChunyan
@version	1.0.0
@brief		���ù�ϣ����ز����ӿ�

@note
��ϣ����ز�����������ϣ�㷨����ʼ�����ڵ����/ɾ�������ң��ͷŵȡ�
*/
#ifndef _ADV_HASH_H_
#define _ADV_HASH_H_
#include "advkill.h"

#define MAX_HASH_SIZE				0xFFFFFFFF

#define HOST_HASH_SEED				13
#define HOST_HASH_SIZE_BIT			5
#define HOST_BIT_MOVE				(32-HOST_HASH_SIZE_BIT)
#define HOST_HASH_SIZE_MASK			(MAX_HASH_SIZE >> HOST_BIT_MOVE)
#define HOST_HASH_SIZE				(HOST_HASH_SIZE_MASK+1)		///< ���ù�ϣ��������

/**
�����ַ�����hashֵ����������λ��

@param str �����ַ���
@return �����ַ����ڹ�ϣ���е�����
*/
unsigned int host_bkdr_hash(unsigned char *str);

/**
�ͷ�URL����ռ�õ��ڴ�ռ�

@param map URL���ýṹ�׵�ַ
@param mapnum URL���ø���
*/
void advconf_hostmap_free(struct advconf_hostmap *map, int mapnum);

/**
�ͷ����ù�ϣ�ڵ���ڴ�ռ�

@param node Ҫ�ͷŵĹ�ϣ�ڵ�
*/
void advconf_hashnode_free(struct advconf_hashnode *node);

/**
������ù�ϣ�ڵ㵽��ϣ��

@param table ���ù�ϣ���׵�ַ
@param tablenum ��ϣ���С
@param node Ҫ��ӵ����ýṹ
*/
void advconf_hashnode_add(struct advconf_hashtable *table, int tablenum, struct advconf_hashnode *node);

/**
����Host�ӹ�ϣ����ɾ�����ýڵ�

@param table ���ù�ϣ���׵�ַ
@param tablenum ��ϣ���С
@param host Host����(�ַ���)
@return �ɹ����� ADV_KILL_FAIL��ʧ�ܷ��� ADV_KILL_OK��
*/
int advconf_hashnode_del_by_host(struct advconf_hashtable *table, int tablenum, char *host);

/**
����Host�ӹ�ϣ���в������ýڵ�

@param table ���ù�ϣ���׵�ַ
@param tablenum ��ϣ���С
@param host Host����(�ַ���)
@param referer HTTPЭ���е�referer�ַ���������������������ѡ���host����referer�е��������ƥ��
@return �ɹ����� ������ṹ���ַ��ʧ�ܷ��� NULL��
*/
struct advconf_hashnode * advconf_hashnode_find_by_host(struct advconf_hashtable *table, int tablenum, char *host, char *referer);

/**
����URL�ӹ�ϣ���в������ýڵ�

@param table ���ù�ϣ���׵�ַ
@param tablenum ��ϣ���С
@param url ԴURL�ַ���
@return �ɹ����� ������ṹ���ַ��ʧ�ܷ��� NULL��
*/
struct advconf_hashnode * advconf_hashnode_find_by_url(struct advconf_hashtable *table, int tablenum, char *url);

/**
ɾ��һ����ϣ�����е��������ýڵ㲢�ͷ��ڴ�

@param table ���ù�ϣ����ͷ
*/
void web_conf_data_hashtable_del_all(struct advconf_hashtable *table);

/**
����URL�ӹ�ϣ���ýڵ��в���URL����

@param node ���ýڵ��ַ
@param surl ԴURL�ַ���
@return �ɹ����� URL���õ�ַ��ʧ�ܷ��� NULL��
*/
struct advconf_hostmap *advconf_hostmap_find_by_url(struct advconf_hashnode *node, char *surl);

/**
��ʼ�����ù�ϣ��

@param table ��ϣ���׵�ַ��Ҫ����ĵ�ַ
@param tablenum ��ϣ���С
@return �ɹ����� ADV_KILL_OK��ʧ�ܷ��� ADV_KILL_FAIL��
*/
int advconf_hashtable_init(struct advconf_hashtable **table, int tablenum);

/**
�ͷ����ù�ϣ��

@param table ��ϣ���׵�ַ
@param tablenum ��ϣ���С
*/
void advconf_hashtable_release(struct advconf_hashtable *table, int tablenum);

/**
��չ�ϣ������������

@param table ��ϣ���׵�ַ
@param tablenum ��ϣ���С
*/
void advconf_hashtable_empty(struct advconf_hashtable *table, int tablenum);

/**
��ӡ��������������Ϣ(���ڵ���)

@param node ���ýڵ��ַ
*/
void advconf_hashnode_print(struct advconf_hashnode *node);

/**
����Host�ӹ�ϣ���в������ýڵ㲢��ӡ����Ϣ

@param table ��ϣ���׵�ַ
@param tablenum ��ϣ���С
@param host Ҫ��ӡ��Host�ַ���
*/
void advconf_hashnode_print_by_host(struct advconf_hashtable *table, int tablenum, char *host);

/**
��ӡһ����ϣ����������������Ϣ

@param table ���ù�ϣ�����ַ
*/
void advconf_hashtable_print(struct advconf_hashtable *table);

/**
��ӡ���ù�ϣ��������������Ϣ

@param table ��ϣ���׵�ַ
@param tablenum ��ϣ���С
*/
void advconf_hashtable_print_all(struct advconf_hashtable *table, int tablenum);

#ifdef ADVKILL_PRINT_DEBUG_INFO
struct advconf_hashnode *advconf_hashnode_generate(int flag, int mapnum);
#endif
#endif
