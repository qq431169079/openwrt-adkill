/**
@file		advkill.c
@date		2014/07/31
@author		WangChunyan
@version	1.0.0
@brief		ȥ���Ӧ���е������ļ�

@note
ȥ���Ӧ�������ļ�����Ҫ���̼����ں�ģ�飬ע��netfilter���Ӻ�����
ץȡ���ݰ�������Ԥ���ù������ƥ�䣬��ƥ�䵽�ٸ���Ԥ���ù������
һϵ��Ԥ���������
*/
#include <linux/slab.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netfilter_bridge.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/list.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/inet.h>
#include <linux/in.h>
#include <linux/module.h>
#include <linux/sched.h>   //wake_up_process()
#include <linux/kthread.h> 

#include "advkill.h"
#include "strcmd.h"
#include "pkgoper.h"
#include "advproc.h"
#include "advhash.h"

/**
�ں�ģ������
*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aaron");
MODULE_DESCRIPTION("Aaron's project kill advertisement");
MODULE_VERSION("1.0");

/* advertisement config */
struct advconf_hashtable *g_advconf_hashtable = NULL;	///< ȥ�������ȫ�ֱ�����Ϊ���ù�ϣ���׵�ַ
struct mutex g_advconf_mutex;	///< ȫ�ֻ����������ֹ�ڳ��������ж�̬�޸����������ͻ

static char *g_location = NULL;	///< ȫ�ֱ�������ʱ����HTTP�����е�location����
static char *g_shost = NULL;	///< ȫ�ֱ�������ʱ����HTTP�����е�Host����
static char *g_referer = NULL;	///< ȫ�ֱ�������ʱ����HTTP�����е�Referer�ֶ�
static char *g_surl = NULL;		///< ȫ�ֱ�������ʱ����HTTP�����е�URL����
//static char *g_method = NULL; //< ȫ�ֱ�������ʱ����HTTP����ķ���
//static unsigned long long g_adv_num = 0;

#ifdef ADVKILL_CHECK_MEM
unsigned long long int g_calloc_times = 0;
unsigned long long int g_calloc_size = 0;
unsigned long long int g_free_times = 0;
unsigned long long int g_free_size = 0;
#endif

/**
��ʼ��ȫ�ֲ���

��ҪΪȫ�ֱ�������ռ䣬�Ա����ÿ��ʹ�ö�����/�ͷſռ䣬�����ڴ������
*/
int global_parameter_init(void)
{
	g_location = (char *)ADVKILL_CALLOC(1, ADV_MAX_LOCATION_LEN);
	if (g_location == NULL)
		return ADV_KILL_FAIL;
	g_shost = (char *)ADVKILL_CALLOC(1, ADV_MAX_SHOST_LEN);
	if (g_shost == NULL)
	{
		ADVKILL_FREE(g_location, ADV_MAX_LOCATION_LEN);
		g_location = NULL;
		return ADV_KILL_FAIL;
	}
	
	g_referer = (char *)ADVKILL_CALLOC(1, ADV_MAX_REFERER_LEN);
	if (g_referer == NULL)
	{
		ADVKILL_FREE(g_location, ADV_MAX_LOCATION_LEN);
		g_location = NULL;
		ADVKILL_FREE(g_shost, ADV_MAX_SHOST_LEN);
		g_shost = NULL;
		return ADV_KILL_FAIL;
	}

	g_surl = (char *)ADVKILL_CALLOC(1, ADV_MAX_SURL_LEN);
	if (g_surl == NULL)
	{
		ADVKILL_FREE(g_location, ADV_MAX_LOCATION_LEN);
		g_location = NULL;
		ADVKILL_FREE(g_shost, ADV_MAX_SHOST_LEN);
		g_shost = NULL;
		ADVKILL_FREE(g_referer, ADV_MAX_REFERER_LEN);
		g_referer = NULL;
		return ADV_KILL_FAIL;
	}

	return ADV_KILL_OK;
}

/**
�ͷ�ȫ�ֱ���ռ���ڴ�ռ�

�����˳�ʱ���ͷ����б������ڴ�ռ䡣
*/
void global_parameter_destroy(void)
{
	if (g_location != NULL)
	{
		ADVKILL_FREE(g_location, ADV_MAX_LOCATION_LEN);
		g_location = NULL;
	}
	if (g_shost != NULL)
	{
		ADVKILL_FREE(g_shost, ADV_MAX_SHOST_LEN);
		g_shost = NULL;
	}
	if(g_referer != NULL)
	{
		ADVKILL_FREE(g_referer, ADV_MAX_REFERER_LEN);
		g_referer = NULL;
	}
	if (g_surl != NULL)
	{
		ADVKILL_FREE(g_surl, ADV_MAX_SURL_LEN);
		g_surl = NULL;
	}
}

/**
��������ҵ�����ǰ��׼������

������ʼ��ȫ�ֱ����������ڴ�ռ䣬����proc�ļ�ϵͳ����ʼ�����ù�ϣ��
*/
int advkill_prepare(void)
{
	int ret = ADV_KILL_OK;
	
	mutex_init(&g_advconf_mutex);
	ret = global_parameter_init();
	if (ret != ADV_KILL_OK)
	{
		ADV_PRINT_ERROR("global_parameter_init failed");
		return ADV_KILL_FAIL;
	}
	ret = create_proc_file();
	if(ret != ADV_KILL_OK)
	{
		ADV_PRINT_ERROR("create_proc_file failed");
		return ADV_KILL_FAIL;
	}
	ret = advconf_hashtable_init(&g_advconf_hashtable, HOST_HASH_SIZE);
	if (ret != ADV_KILL_OK)
	{
		ADV_PRINT_ERROR("advconf_hashtable_init failed");
		return ADV_KILL_FAIL;
	}
	return ret;
}

/**
�����˳�ǰ�Ĳ���

����ɾ��proc�ļ�ϵͳ���ͷ�ȫ�ֹ�ϣ��(����),�ͷ�ȫ�ֱ����ġ�
*/
void advkill_finish(void)
{
	destroy_proc_file();
	advconf_hashtable_release(g_advconf_hashtable, HOST_HASH_SIZE);
	global_parameter_destroy();
	mutex_destroy(&g_advconf_mutex);
}

#define IPPROTO_TCP  6
#define IPPROTO_UDP 17
#define IPPROTO_ICMP 1

static inline int payload_offset(const struct sk_buff *skb)
{
	int ip_hl = 4*ip_hdr(skb)->ihl;

	if( ip_hdr(skb)->protocol == IPPROTO_TCP ) {
		int tcp_hl = 4*(skb->data[ip_hl + 12] >> 4);
		return ip_hl + tcp_hl;
	} else if( ip_hdr(skb)->protocol == IPPROTO_UDP  ) {
		return ip_hl + 8; 
	} else if( ip_hdr(skb)->protocol == IPPROTO_ICMP ) {
		return ip_hl + 8;
	} else {
		return ip_hl + 8;
	}
}


/**
netfilterע�ṳ�Ӻ���������ԭ��Ϊ�ں�ȷ��

�ڸù��Ӻ����ڽ���ȥ���ҵ����
*/


#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,22)
static u_int32_t hook_func(unsigned int hook,
						    struct sk_buff **pskb,
					           const struct net_device *in,
					           const struct net_device *out,
					           int (*okfn)(struct sk_buff *))
#else
static u_int32_t hook_func(unsigned int hook,
						    struct sk_buff *pskb,
					           const struct net_device *in,
					           const struct net_device *out,
					           int (*okfn)(struct sk_buff *))
#endif
{	


	struct iphdr *iph = NULL;
	struct tcphdr *tcph = NULL;
	#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,22)
		struct sk_buff *sb = *pskb;
	#else
		struct sk_buff *sb = pskb;
	#endif
	char *httpcontent = NULL;
	char *tmp_payload = NULL;
	int httpcontentlen = 0;
	char *shost = NULL;//, *method = NULL;
	char *referer = NULL;
	int refererlen = 0;
	int shostlen = 0;//, methodlen = 0;
	char *surl = NULL;
	int surllen = 0;
	int ret = ADV_KILL_OK;
	struct advconf_hashnode *tmphost = NULL;
	struct advconf_hostmap *tmpmap = NULL;
	int skbuffoper = 0;
	char *urlflagpos = NULL;
	int i = 0;
	//for temp domain,eg: *.logic.cpm.cm.sandai.net
	char *tmpdomain = NULL;

	//printk(KERN_INFO "----begin enter hook function--\n\n");
	/* is not ip protocol, return accept */
	iph = ip_hdr(sb);
	if (iph == NULL)
	{
		goto exit_no_cmd;
	}
	//printk( "sip[%08x],dip[%08x],proto[%d]\n", iph->saddr, iph->daddr, iph->protocol);

	/* is not tcp protocol,return accept */
	if(iph->protocol != IPPROTO_TCP)
	{
		goto exit_no_cmd;
	}
	tmp_payload = sb->data;
	/* find tcp header and tcp data len */
	tcph = (struct tcphdr *) ((unsigned char *)iph + (iph->ihl * 4));
	if (0 != skb_linearize(sb)) 
	{
		goto exit_no_cmd;
	}
	//printk("tcp sport[%d],dport[%d]\n", tcph->source, tcph->dest);
	httpcontentlen = (ntohs(iph->tot_len) - iph->ihl*4 - tcph->doff*4);
	if (httpcontentlen <= HTTP_MIN_LEN)
	{
		//printk(KERN_INFO "http content len[%d]\n", httpcontentlen);
		goto exit_no_cmd;
	}

	httpcontent = (char *) ((unsigned char *)tcph + tcph->doff*4);
	if (strncmp(httpcontent, HTTP_METHOD_FLAG, HTTP_METHOD_FLAG_LEN) != 0)
	{
		goto exit_no_cmd;
	}
	//if(strstr(httpcontent, "GET /livemsg?")) {     //for tencent live   ��Ѷ��Ƶ�ֻ���
		//printk("#####____________ httpcontent:[%s]_____________####\n", httpcontent);
		/*ret = send_client_notfound(sb);	
		if (ret != ADV_KILL_OK) {
			ADV_PRINT_ERROR("send_client_notfound failed\n");
			goto exit_no_cmd;
		}
		skbuffoper = 1;
		goto exit_free;*/
	//}
	memset(g_shost, 0, ADV_MAX_SHOST_LEN);
	shost = get_http_field(httpcontent, httpcontentlen, HTTP_HOST_FLAG, HTTP_HOST_FLAG_LEN, g_shost, ADV_MAX_SHOST_LEN, &shostlen);
	if (shost == NULL)
	{
		if(strstr(httpcontent, "GET /livemsg?")) {     //for tencent live   ��Ѷ��Ƶ�ֻ���
			//printk("#####____________ httpcontent:[%s]_____________####\n", httpcontent);
			ret = send_client_notfound(sb);	
			if (ret != ADV_KILL_OK) {
				ADV_PRINT_ERROR("send_client_notfound failed\n");
				goto exit_no_cmd;
			}
			skbuffoper = 1;
			goto exit_free;
		}
		goto exit_no_cmd;
	}

	memset(g_surl, 0, ADV_MAX_SURL_LEN);
	surl = get_http_get_url(httpcontent, httpcontentlen, g_surl, ADV_MAX_SURL_LEN, &surllen);
	if (surl == NULL)
	{
		goto exit_no_cmd;
	}

	memset(g_referer, 0, ADV_MAX_REFERER_LEN);
	referer = get_http_field(httpcontent, httpcontentlen, HTTP_REFERER_FLAG, HTTP_REFERER_FLAG_LEN, g_referer, ADV_MAX_REFERER_LEN, &refererlen);
	if (refererlen == 0)
	{
		referer = NULL;
	}
	
	//printk( "surl[%s]\n", surl);
#if 0
	/* check host is digit or not,eg:192.168.1.1 */
	if (is_digit_host(shost) == ADV_KILL_OK)
	{
		ADVKILL_MUTEX_LOCK(&g_advconf_mutex);
		tmphost = advconf_hashnode_find_by_url(g_advconf_hashtable, HOST_HASH_SIZE, surl);
		if (tmphost == NULL)
		{
			ADVKILL_MUTEX_UNLOCK(&g_advconf_mutex);
			goto exit_no_cmd;
		}
		//g_adv_num++;
		//printk(KERN_ALERT "find digit host\n");
		//if (g_adv_num % 4 == 1)
			goto find_url;
		/*else
		{
			send_client_notfound(sb);
			skbuffoper = 1;
			ADVKILL_MUTEX_UNLOCK(&g_advconf_mutex);
			goto exit_free;
		}*/
	}
#endif
	/* check domain is contain date or not,eg: 20131223.logic.cpm.cm.sandai.net */
	if (domain_contain_digits(shost, XUNLEI_KANKAN_DOMAIN_DIGIT_NUM) == ADV_KILL_OK)
	{
		tmpdomain = domain_regroup_match_mode(shost, XUNLEI_KANKAN_DOMAIN_DIGIT_INDEX);
		if (tmpdomain != NULL)
		{
			ADVKILL_MUTEX_LOCK(&g_advconf_mutex);
			tmphost = advconf_hashnode_find_by_host(g_advconf_hashtable, HOST_HASH_SIZE, tmpdomain, referer);
			if (tmphost == NULL)
			{
				ADVKILL_MUTEX_UNLOCK(&g_advconf_mutex);
				goto exit_no_cmd;
			}
			goto find_url;
		}
	}
		

	ADVKILL_MUTEX_LOCK(&g_advconf_mutex);
	tmphost = advconf_hashnode_find_by_host(g_advconf_hashtable, HOST_HASH_SIZE, shost, referer);
	if (tmphost == NULL)
	{
		ADVKILL_MUTEX_UNLOCK(&g_advconf_mutex);
		goto exit_no_cmd;
	}

	//printk(KERN_ALERT "find shost:[%s],surl[%s]\n", shost, surl);
	//advconf_hashtable_print_all(g_advconf_hashtable, HOST_HASH_SIZE);
find_url:
	tmpmap = advconf_hostmap_find_by_url(tmphost, surl);
	//printk("______________%d_______________\n",__LINE__);
	if (tmpmap == NULL)
	{
		ADVKILL_MUTEX_UNLOCK(&g_advconf_mutex);
		//printk("______________%d_______________\n",__LINE__);
		goto exit_no_cmd;
	}
#ifdef ADVKILL_PRINT_DEBUG_INFO
	printk(KERN_ALERT "find surl:%s,dev[%s]\n", surl, sb->dev->name);
#endif
	if (tmphost->type == adv_redirect_player)
	{
		
		
		//printk(KERN_ALERT "redirect url\n");
		memset(g_location, 0, ADV_MAX_LOCATION_LEN);
		http_location_generate(g_location, ADV_MAX_LOCATION_LEN, tmphost->d_host, tmpmap->durl);
		ADVKILL_MUTEX_UNLOCK(&g_advconf_mutex);
		printk(KERN_ALERT "location [%s]\n", g_location);
#ifdef ADVKILL_PRINT_DEBUG_INFO
		printk(KERN_ALERT "begin send_client_location\n");
#endif

		ret = send_client_location(sb, g_location);
#ifdef ADVKILL_PRINT_DEBUG_INFO
		printk(KERN_ALERT "send_client_location ret[%d]\n", ret);
#endif
		if (ret != ADV_KILL_OK)
		{
			ADV_PRINT_ERROR("send_client_location failed\n");
			goto exit_no_cmd;
		}

		skbuffoper = 1;
		goto exit_free;
	}
	else if (tmphost->type == adv_drop_request)
	{
		
		//printk("drop request,url=%s\n",surl);
		ADVKILL_MUTEX_UNLOCK(&g_advconf_mutex);
		if (is_contain_except_url(g_surl, tmphost) == ADV_KILL_OK)
		{
			goto exit_no_cmd;
		}
		ret = send_client_notfound(sb);	

		if (ret != ADV_KILL_OK)
		{
			ADV_PRINT_ERROR("send_client_notfound failed\n");
			goto exit_no_cmd;
		}

		skbuffoper = 1;
		goto exit_free;
	}
	else if (tmphost->type == adv_modify_url)
	{
		
		//printk("modify url ,url=%s\n",surl);

		ADVKILL_MUTEX_UNLOCK(&g_advconf_mutex);

#ifdef ADVKILL_PRINT_DEBUG_INFO
		printk(KERN_ALERT "host[%s] modify url,begin find map\n", tmphost->s_host);
#endif
		if (tmphost->mapnum > 0)
		{
			for (i=0; i<tmphost->mapnum; i++)
			{
				urlflagpos = strstr(httpcontent, tmphost->map[i].surl);
				if (urlflagpos != NULL)
				{
					urlflagpos += strlen(tmphost->map[i].surl);
					memcpy(urlflagpos, tmphost->map[i].durl, strlen(tmphost->map[i].durl));
					httpcontent = urlflagpos;
				}
			}
			refresh_skb_checksum(sb);
		}
		goto exit_no_cmd;
	}
	else if (tmphost->type == adv_bad_gw)
	{
		ADVKILL_MUTEX_UNLOCK(&g_advconf_mutex);
		if (is_contain_except_url(g_surl, tmphost) == ADV_KILL_OK)
		{
			goto exit_no_cmd;
		}
		ret = send_client_bad_gateway(sb);
		if (ret != ADV_KILL_OK)
		{
			ADV_PRINT_ERROR("send_client_bad_gateway failed\n");
			goto exit_no_cmd;
		}

		skbuffoper = 1;
		goto exit_free;
	}
	else if (tmphost->type == adv_fake_pack)
	{
		ADVKILL_MUTEX_UNLOCK(&g_advconf_mutex);
		if (is_contain_except_url(g_surl, tmphost) == ADV_KILL_OK)
		{
			goto exit_no_cmd;
		}
		//printk("fake request,url=%s\n",surl);
		ret = send_client_fake_message(sb,tmphost->d_host,tmphost->map[0].durl);
		if (ret != ADV_KILL_OK)
		{
			ADV_PRINT_ERROR("send_client_bad_gateway failed\n");
			goto exit_no_cmd;
		}

		skbuffoper = 1;
		goto exit_free;
	}
		 
	ADVKILL_MUTEX_UNLOCK(&g_advconf_mutex);

exit_no_cmd:
	skbuffoper = 0;

exit_free:
	
	if (skbuffoper == 0)
		return NF_ACCEPT;
	else if(skbuffoper == 1)
		return NF_DROP;
	else
		return NF_ACCEPT;
}

static struct nf_hook_ops nfho_forward = 
{
	.hook		= hook_func,
	.owner		= THIS_MODULE,
	.pf		= PF_INET,
#if (LINUX_VERSION_CODE < KERNEL_VERSION (2, 6, 26))
	.hooknum	= NF_IP_FORWARD,
#else
	.hooknum	= NF_INET_FORWARD,
#endif
	.priority	= NF_IP_PRI_FIRST,
};

#ifdef ADVKILL_PRINT_DEBUG_INFO
void advconf_hashnode_add_by_number(int num)
{
	//int i = 0;
	struct advconf_hashnode *tmpnode = NULL;
	//for (i=1; i<=num; i++)
	{
		tmpnode = advconf_hashnode_generate(1, 1);

		if (tmpnode == NULL)
		{
			printk(KERN_ALERT "advconf_hashnode_generate [%d] failed\n", 1);
			return;
			//break;
		}
		advconf_hashnode_add(g_advconf_hashtable, HOST_HASH_SIZE, tmpnode);
	}
}
#endif

/**
�ں�ģ����ڣ���ʼ������ע�ṳ�Ӻ���
*/

static int __init adv_kill_module_init(void)
{
#ifdef ADVKILL_PRINT_DEBUG_INFO
	printk(KERN_ALERT "kill advertisement module load\n");
#endif

	/* init system */
	if (advkill_prepare() != ADV_KILL_OK)
	{
		ADV_PRINT_ERROR("advkill_prepare failed");
		return ADV_KILL_FAIL;
	}
	/* register package receiver */

	nf_register_hook(&nfho_forward);

	return 0;
}

/**
�ں�ģ����ڣ�����β������ע�����Ӻ���
*/
static void __exit adv_kill_module_exit(void)
{
#ifdef ADVKILL_PRINT_DEBUG_INFO
	printk(KERN_ALERT "kill advertisement module unload\n");
#endif

	nf_unregister_hook(&nfho_forward);
	/* destroy system */
	advkill_finish();

#ifdef ADVKILL_CHECK_MEM
	printk(KERN_ALERT "calloc times[%llu],size[%llu],free times[%llu],size[%llu]\n", g_calloc_times, g_calloc_size, g_free_times, g_free_size);
#endif
}

module_init(adv_kill_module_init);
module_exit(adv_kill_module_exit);
