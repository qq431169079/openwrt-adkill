/**
@file		advkill.h
@date		2014/07/31
@author		WangChunyan
@version	1.0.0
@brief		ȥ���Ӧ�����õ�����Ҫ���ݽṹ

@note		
ȥ���Ӧ������Ҫ���ݽṹ����¼����Ҫ�޸����ݰ�����վ���޸Ĺ������Ϣ��
*/

#ifndef _ADV_KILL_H_
#define _ADV_KILL_H_

#include <linux/types.h>
#include <linux/mm.h>
#include <linux/string.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/version.h>
#define _BO_TONG_								1

//#define	ADVKILL_PRINT_DEBUG_INFO				1
//#define ADVKILL_CHECK_MEM						1

#define ADV_KILL_OK								0
#define ADV_KILL_FAIL							-1

#define ADV_MAX_LOCATION_LEN					256
#define ADV_MAX_SHOST_LEN						128
#define ADV_MAX_REFERER_LEN						128
#define ADV_MAX_SURL_LEN						1024
#define ADV_MAX_METHOD_LEN					4


#ifndef ADVKILL_CHECK_MEM
#define ADVKILL_CALLOC(n, size)				kcalloc(n, size, GFP_KERNEL)
#define ADVKILL_FREE(addr, size)			kfree(addr)
#else

#define ADVKILL_CALLOC(n, size)				kcalloc(n, size, GFP_KERNEL);(g_calloc_times++);(g_calloc_size+=((n)*(size)))
#define ADVKILL_FREE(addr, size)			kfree(addr);(g_free_times++);(g_free_size+=(size))
#endif

#define ADV_PRINT_INFO(info)				printk(KERN_ALERT "%s\n", info)
#define ADV_PRINT_ERROR(error)				printk(KERN_ERR "%s\n", error)
#define EnterFunction()						//printk(KERN_INFO "Enter function[%s],line[%d]\n", __FUNCTION__, __LINE__); PrintCurrentTime()
#define LeaveFunction()						//PrintCurrentTime(); printk(KERN_ALERT "Leave function[%s],line[%d]\n", __FUNCTION__, __LINE__)
#define LeaveFunction_fail()				//PrintCurrentTime(); printk(KERN_ALERT "----Failed,Leave function[%s],line[%d]\n", __FUNCTION__, __LINE__)

#define ADVKILL_MUTEX_LOCK(mutex)			mutex_lock(mutex)
#define ADVKILL_MUTEX_UNLOCK(mutex)			mutex_unlock(mutex)

/**
��¼ȥ������ù���

��Ҫ��¼Ҫƥ���ԴURL��Ҫ�޸ĳɵ�URL
*/
struct advconf_hostmap
{
	char *surl;		///< ƥ�䵽��ԴURL
	char *durl;		///< Ҫ�޸ĳɵ�URL
	int surllen;	///< ԴRUL����
	int durllen;	///< Ҫ�޸ĳɵ�URL����
};

/**
ȥ�������ģʽ

adv_redirect_player: �ض��򲥷�����һ�㷵��302
adv_drop_request: �����������һ�㷵��404
adv_modify_url: �޸�ԴURL����
adv_bad_gw: ���ش�������� 502
*/
typedef enum {adv_redirect_player=0, adv_drop_request=1, adv_modify_url=2, adv_bad_gw=3, adv_fake_pack=4} ADV_CMD;

/**
ȥ���Ӧ���е�������Ϣ�ṹ��

��Ҫ��¼�������ͣ�ԴHost���޸ĳɵ�Ŀ��Host���Լ�URL����
����URl��������Ƕ��ס�
*/
struct advconf_hashnode
{
	struct hlist_node node;	///< ��ϣ����ڵ�
	ADV_CMD type;	///< ���ò�������
	int index;	///< ��������
	unsigned char *s_host;	///< Ҫƥ���ԴHost
	unsigned char *d_host;	///< Ҫ�޸ĳɵ�Ŀ��Host
	unsigned short int s_host_len;	///< Ҫƥ���ԴHostռ���ڴ��С
	unsigned short int d_host_len;	///< Ҫ�޸ĳɵ�Ŀ��Hostռ���ڴ��С
	struct advconf_hostmap *map;	///< ���ù�����URL���ýṹ�׵�ַ
	int mapnum;	///< ���ù�����URl���õĸ���(ͬһ��Host����URl����)
};

/**
ȥ��������й�ϣ����ͷ

*/
struct advconf_hashtable
{
	struct hlist_head head;  ///< ��ϣ����ͷ�������ϣ���е�����һ����ϣ����
};

#endif
