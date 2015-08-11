/**
@file		pkgoper.h
@date		2014/07/31
@author		WangChunyan
@version	1.0.0
@brief		ȥ���Ӧ�������ݰ������ӿ�

@note		
���ݰ������ӿڣ�������װ���ݰ����������ݰ��ȡ�
*/

#ifndef _PKG_OPER_H_
#define _PKG_OPER_H_

#define HTTP_RESPONSE_HEAD_NOT_FIND							"HTTP/1.1 307 Temporary Redirect\r\n"
#define HTTP_RESPONSE_HEAD_NOT_FIND_LEN							33
#define HTTP_RESPONSE_LOCATION								"Location: "
#define HTTP_RESPONSE_LOCATION_LEN							10
#define HTTP_RESPONSE_END								"\r\n"
#define HTTP_RESPONSE_END_LEN								2
//#define HTTP_RESPONSE_CONNECTION							"Connection: closed"

/* HTTP 404 ���� */
#define HTTP_NOT_FOUND_STR									"HTTP/1.1 404 Not Found\r\n" \
															"Server: QWS\r\n" \
															"Content-Type: text/html\r\n" \
															"Content-Length: 130\r\n" \
															"Connection: keep-alive\r\n\r\n" \
															"<html><head><title>404 Not Found</title></head><body><center><h1>404 Not Found</h1></center><hr><center>QWS</center></body></html>"
#define HTTP_NOT_FOUND_STR_LEN								239

/* HTTP 502 ���� */
#define HTTP_BAD_GATEWAY									"HTTP/1.1 502 Bad Gateway\r\n" \
															"Server: QWS\r\n" \
															"Content-Type: text/html\r\n" \
															"Content-Length: 134\r\n" \
															"Connection: keep-alive\r\n\r\n" \
															"<html><head><title>502 Bad Gateway</title></head><body><center><h1>502 Bad Gateway</h1></center><hr><center>QWS</center></body></html>"
#define HTTP_BAD_GATEWAY_STR_LEN							245

/* HTTP 302 ����*/
#define HTTP_FIND_LOCAL_STR									"HTTP/1.1 302 Found\r\nLocation: http://127.0.0.1/\r\n\r\n"
#define HTTP_FIND_LOCAL_STR_LEN								51

/* lan interface */
#define ETH_CLIENT_LAN0									"eth0.1"
#define ETH_CLIENT_LAN1									"eth0.1"

/* wlan interface */
#define ETH_CLIENT_WLAN0								"ra0"
#define ETH_CLIENT_WLAN1								"ra1"

/**
������Ϣ�ṹ��

��Ҫ���������������ݰ�ʱʹ�ã��������ߺ�����������
*/
struct client_nicname
{
	int index;	///< ��������
	char *name;	///< ��������
};

/**
����tcp�����������ݰ�

����tcp���ݣ��������ݰ�������� mac/ip/tcp ͷ����Ϣ
@param skb ԭʼ��sk_buff�ṹ��ַ
@param names �������ƽṹ�׵�ַ
@param num ��������
@param tcpdata tcp���ݵ�ַ
@param tcpdatalen tcp���ݳ���
@return �ɹ��������ݰ���ַ��ʧ�ܷ���NULL��
*/
struct sk_buff *pkg_skbuff_generate(struct sk_buff *skb, struct client_nicname *names, int num, char *tcpdata, int tcpdatalen);

/**
����tcp���ݰ�

@param skb ԭʼ��sk_buff�ṹ��ַ
@param tcpdata tcp���ݵ�ַ
@param tcpdatalen tcp���ݳ���
@return �ɹ����� ADV_KILL_OK��ʧ�ܷ��� ADV_KILL_FAIL��
*/
int pkg_skbuff_dev_xmit(struct sk_buff *skb, char *tcpdata, int tcpdatalen);

/**
����Host����location�ַ���

@param httplen ���ɵ�location����
@param host Host�ַ�������
@return �ɹ�����location��ַ��ʧ�ܷ���NULL��
*/
char *http_location_str_generate(int *httplen, char *host);

/**
����location����302��Ϣ

@param skb ԭʼ��sk_buff�ṹ��ַ
@param location ��Ҫ���͵�location����
@return �ɹ����� ADV_KILL_OK��ʧ�ܷ��� ADV_KILL_FAIL��
*/
int send_client_location(struct sk_buff *skb, char *location);

/**
����404��Ϣ

@param skb ԭʼ��sk_buff�ṹ��ַ
@return �ɹ����� ADV_KILL_OK��ʧ�ܷ��� ADV_KILL_FAIL��
*/
int send_client_notfound(struct sk_buff *skb);

/**
����502��Ϣ

@param skb ԭʼ��sk_buff�ṹ��ַ
@return �ɹ����� ADV_KILL_OK��ʧ�ܷ��� ADV_KILL_FAIL��
*/
int send_client_bad_gateway(struct sk_buff *skb);

/**
���¶�IPͷ��TCPͷ����У��

@param skb ԭʼ��sk_buff�ṹ��ַ
*/
void refresh_skb_checksum(struct sk_buff *skb);

/*
 ����α���HTTP200��Ӧ
 @param skb ԭʼ��sk_buff�ṹ��ַ CType Content-Type Cont ��ҳ����
 @return �ɹ����� ADV_KILL_OK��ʧ�ܷ��� ADV_KILL_FAIL��
HTTP/1.1 200 OK\r\n
Server: QWS\r\n
Content-Type: text/xml\r\n
Content-Length: 125\r\n
Connection: close\r\n
Accept-Ranges: bytes\r\n\r\n
*/
int send_client_fake_message(struct sk_buff *skb, const char * CType, const char * Cont);
#endif

