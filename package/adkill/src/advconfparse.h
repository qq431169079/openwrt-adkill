/**
@file		advconfparse.h
@date		2014/07/31
@author		WangChunyan
@version	1.0.0
@brief		ȥ������ý����ӿ�

@note
ȥ�������Ӧ���н����û����õ���ؽӿ�
*/
#ifndef _ADV_CONF_PARSE_H_
#define _ADV_CONF_PARSE_H_

#include "advkill.h"

#define ADV_CONF_FIELD_2_LEN					16
#define ADV_CONF_FIELD_3_LEN					128
#define ADV_CONF_FIELD_4_LEN					256
#define ADV_CONF_FIELD_5_LEN					128
#define ADV_CONF_FIELD_6_LEN					256

#define ADV_CONF_DATA_SEPC_CHAR					' '
#define ADV_TAG_SEPARATIR_CHAR					','
#define ADV_CONF_REDIRECT_PLAYER				'R'		///< �ض���ģʽ 302
#define ADV_CONF_DROP_REQUEST					'D'		///< dropģʽ 404
#define ADV_CONF_MODIFY_URL						'M'		///< �޸�Դ���ݰ�
#define ADV_CONF_BAD_GW							'G'		///< ���ش��� 502
#define ADV_CONF_FAKE_PACK						'F'		///< ����α�����ݰ�

/**
����һ�����ã�������ɺ����ô���ڽṹ�� advconf_hashtable ��

@param linedata ����������
@param table ���ù�ϣ���׵�ַ
@param tablenum ���ù�ϣ���С
@return �ɹ�����ADV_KILL_OK��ʧ�ܷ���ADV_KILL_FAIL
*/
int parse_advconf_line_data(char *linedata, struct advconf_hashtable *table, int tablenum);

#endif
