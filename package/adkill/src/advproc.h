/**
@file		advproc.h
@date		2014/07/31
@author		WangChunyan
@version	1.0.0
@brief		proc�ļ�ϵͳ��ز����ӿ�

@note
����proc�ļ���ɾ��proc�ļ�����дproc�ļ�����
*/

#ifndef _ADV_PROC_H_
/**
@file		advproc.h
@date		2014/07/31
@author		WangChunyan
@version	1.0.0
@brief		ȥ���Ӧ�õ�proc�ļ�

@note
����proc�ļ���ɾ��proc�ļ�
*/
#define _ADV_PROC_H_

#include <linux/kernel.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION (3, 10, 0))
#include <linux/fs.h>
#include <linux/seq_file.h>
#endif
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <asm/uaccess.h>

#define MAX_ADVKILL_CONF_LEN					1024			///< proc�ļ�ϵͳ��ÿ��������󳤶�
#define ADV_KILL_PROC_DIRECTORY					"advkill"		///< proc�ļ�Ŀ¼����
#define ADV_KILL_PROC_FILE						"advconf"		///< proc�ļ�����

/**
������Ҫ�õ���proc�ļ�

@return �ɹ����� RR_OK��ʧ�ܷ��� RR_FAIL��
*/
int create_proc_file(void);

/**
ɾ��proc�ļ�
*/
void destroy_proc_file(void);

#endif
