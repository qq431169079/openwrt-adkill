/**
@file		advproc.c
@date		2014/07/31
@author		WangChunyan
@version	1.0.0
@brief		proc�ļ�ϵͳ��ز����ӿ�

@note
����proc�ļ���ɾ��proc�ļ�����дproc�ļ�����
*/

#include "advkill.h"
#include "advproc.h"
#include "advconfparse.h"
#include "advhash.h"

extern struct advconf_hashtable *g_advconf_hashtable;

#ifdef ADVKILL_CHECK_MEM
extern unsigned long long int g_calloc_times;
extern unsigned long long int g_calloc_size;
extern unsigned long long int g_free_times;
extern unsigned long long int g_free_size;
#endif

static struct proc_dir_entry *proc_dir = NULL;	///< proc�ļ����ļ���
static struct proc_dir_entry *proc_advkill_conf = NULL; ///< proc�ļ����ļ�
static int advkill_conf_index;  
static int advkill_conf_next;
static char *advkillconfdata = NULL; ///< �ں���ȥ������û�������ַ

/**
���û��ռ佫���ݶ�ȡ���ں˿ռ䣬�����󱣴��ڹ�ϣ����

@param filp �ļ��ṹ��ָ�룬���ӿڶԴ�û�в�����Ϊ�ں˺���������ʽ
@param buff �û��ռ����ݵ�ַ
@param len �û��ռ����ݳ���
@param data ���ӿڶԴ�û�в�����Ϊ�ں˺���������ʽ
@return ���ض�ȡ�ĳ���
*/
#if (LINUX_VERSION_CODE < KERNEL_VERSION (3, 10, 0))
static ssize_t advkill_conf_write( struct file *filp, const char __user *buff, unsigned long len, void *data);
#else
static ssize_t advkill_conf_write( struct file *filp, const char __user *buff, size_t len, loff_t *data);
#endif

/**
���ں˿ռ佫ȥ��������ڴ濽�����û��ռ�

@param page �û��ռ�������ݵĵ�ַ
@param start ���ӿڶԴ�û�в�����Ϊ�ں˺���������ʽ
@param off ���ӿڶԴ�û�в�����Ϊ�ں˺���������ʽ
@param count ���ӿڶԴ�û�в�����Ϊ�ں˺���������ʽ
@param eof ���ӿڶԴ�û�в�����Ϊ�ں˺���������ʽ
@param data ���ӿڶԴ�û�в�����Ϊ�ں˺���������ʽ
@return ����д��ĳ���
*/
#if (LINUX_VERSION_CODE < KERNEL_VERSION (3, 10, 0))
static int advkill_conf_read( char *page, char **start, off_t off, int count, int *eof, void *data);
#endif
/**
���û��ռ佫���ݶ�ȡ���ں˿ռ䣬�����󱣴��ڹ�ϣ����

@param filp �ļ��ṹ��ָ�룬���ӿڶԴ�û�в�����Ϊ�ں˺���������ʽ
@param buff �û��ռ����ݵ�ַ
@param len �û��ռ����ݳ���
@param data ���ӿڶԴ�û�в�����Ϊ�ں˺���������ʽ
@return ���ض�ȡ�ĳ���
*/
#if (LINUX_VERSION_CODE < KERNEL_VERSION (3, 10, 0))
static ssize_t advkill_conf_write( struct file *filp, const char __user *buff, unsigned long len, void *data)
#else
static ssize_t advkill_conf_write( struct file *filp, const char __user *buff, size_t len, loff_t *data)
#endif
{
	int space_available = (MAX_ADVKILL_CONF_LEN-advkill_conf_index)+1;

	if (len > space_available) 
	{
		memset(advkillconfdata, 0, MAX_ADVKILL_CONF_LEN);
		advkill_conf_index = 0;
		advkill_conf_next = 0;
	}
	if(advkill_conf_index>0)advkillconfdata[advkill_conf_index-1] = '\n';
	if (copy_from_user(&advkillconfdata[advkill_conf_index], buff, len )) 
	{
		return -EFAULT;
	}

#ifdef ADVKILL_PRINT_DEBUG_INFO
	ADV_PRINT_INFO(&advkillconfdata[advkill_conf_index]);
#endif

	//parse advconf
	parse_advconf_line_data(&advkillconfdata[advkill_conf_index], g_advconf_hashtable, HOST_HASH_SIZE);

	advkill_conf_index += len;
	advkillconfdata[advkill_conf_index-1] = '\0';

	return len;
}

/**
���ں˿ռ佫ȥ��������ڴ濽�����û��ռ�

@param page �û��ռ�������ݵĵ�ַ
@param start ���ӿڶԴ�û�в�����Ϊ�ں˺���������ʽ
@param off ���ӿڶԴ�û�в�����Ϊ�ں˺���������ʽ
@param count ���ӿڶԴ�û�в�����Ϊ�ں˺���������ʽ
@param eof ���ӿڶԴ�û�в�����Ϊ�ں˺���������ʽ
@param data ���ӿڶԴ�û�в�����Ϊ�ں˺���������ʽ
@return ����д��ĳ���
*/
#if (LINUX_VERSION_CODE < KERNEL_VERSION (3, 10, 0))
static int advkill_conf_read( char *page, char **start, off_t off, int count, int *eof, void *data)
{
	int len;

	if (off > 0) 
	{
		*eof = 1;
		return 0;
	}
	/* Wrap-around */
	if (advkill_conf_next >= advkill_conf_index) 
		advkill_conf_next = 0;
	len = sprintf(page, "%s\n", &advkillconfdata[advkill_conf_next]);
	advkill_conf_next += len;

	return len;
}
#else
static int advkill_conf_seq_read(struct seq_file *seq, void *v)
{
	return seq_printf(seq, "%s\n", advkillconfdata);
}
static int advkill_conf_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, advkill_conf_seq_read, NULL);
}
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION (3, 10, 0))
static const struct file_operations advkill_fops = {
	.owner = THIS_MODULE,
	.open		= advkill_conf_seq_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.write 		= advkill_conf_write,
	.release	= single_release,
};
#endif
/**
������Ҫ�õ���proc�ļ�

@return �ɹ����� RR_OK��ʧ�ܷ��� RR_FAIL��
*/
int create_proc_file(void)
{
	int ret = ADV_KILL_OK;
	//web config
	advkillconfdata = (char *)ADVKILL_CALLOC(1, MAX_ADVKILL_CONF_LEN);
	if (advkillconfdata == NULL)
		return ADV_KILL_FAIL;
	
	proc_dir = proc_mkdir(ADV_KILL_PROC_DIRECTORY, NULL);
	if (proc_dir == NULL) 
	{
		ret = -ENOMEM;
		printk(KERN_ERR "Couldn't create proc dir[/proc/%s]\n", ADV_KILL_PROC_DIRECTORY);
		goto exit_fail;
	}
	else
	{
#if (LINUX_VERSION_CODE < KERNEL_VERSION (3, 10, 0))
		proc_advkill_conf = create_proc_entry(ADV_KILL_PROC_FILE, 0644, proc_dir);
		if (proc_advkill_conf == NULL)
		{
			printk(KERN_ERR "Couldn't create proc entry[/proc/%s/%s]\n", ADV_KILL_PROC_DIRECTORY, ADV_KILL_PROC_FILE);
			ret = -ENOMEM;
			goto exit_fail;
		}
		proc_advkill_conf->read_proc = advkill_conf_read;
		proc_advkill_conf->write_proc = advkill_conf_write;
#else
		proc_advkill_conf = proc_create(ADV_KILL_PROC_FILE, 0644, proc_dir, &advkill_fops);
		if (proc_advkill_conf == NULL)
		{
			printk(KERN_ERR "Couldn't create proc entry[/proc/%s/%s]\n", ADV_KILL_PROC_DIRECTORY, ADV_KILL_PROC_FILE);
			ret = -ENOMEM;
			goto exit_fail;
		}
#endif
	}

	return ret;

exit_fail:
	if (advkillconfdata != NULL)
	{
		ADVKILL_FREE(advkillconfdata, MAX_ADVKILL_CONF_LEN);
		advkillconfdata = NULL;
	}
	if (proc_dir != NULL)
	{
		remove_proc_entry(ADV_KILL_PROC_FILE, proc_dir);
		proc_dir = NULL;
	}
	return ret;	
}

/**
ɾ��proc�ļ�
*/
void destroy_proc_file(void)
{
	if (proc_advkill_conf != NULL)
	{
		remove_proc_entry(ADV_KILL_PROC_FILE, proc_dir);
		proc_advkill_conf = NULL;
	}
	if (proc_dir != NULL)
	{
		remove_proc_entry(ADV_KILL_PROC_DIRECTORY, NULL);
		proc_dir = NULL;
	}
	if (advkillconfdata != NULL)
	{
		ADVKILL_FREE(advkillconfdata, MAX_ADVKILL_CONF_LEN);
		advkillconfdata = NULL;
	}
}
