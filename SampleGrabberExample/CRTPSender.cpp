#include "CRTPSender.h"

//rtp
CRTPSender::CRTPSender():m_InitFlag(FALSE)
{
}
CRTPSender::~CRTPSender()
{
	Uninitialize();
}
BOOL CRTPSender::Initialize(int port, const char* destIP)
{
	if(m_InitFlag)
	{
		return TRUE;
	}
	if(!InitWinsock())
	{
		printf("socket init failed!");
		return FALSE;
	}
	struct sockaddr_in dest_addr;
	int len = sizeof(dest_addr);
	float framerate =  25;
	unsigned int timestamp_increse = 0, ts_current = 0;
	timestamp_increse = (unsigned int)(90000.0 / framerate); //+0.5);

	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(port);          
	dest_addr.sin_addr.s_addr = inet_addr(destIP); 
	m_Socket = socket(AF_INET,SOCK_DGRAM,0);
	if(INVALID_SOCKET == m_Socket)  
    {  
        printf("socket failed!");  
        WSACleanup();//释放套接字资源;  
		return FALSE;  
    } 
	//申请UDP套接字
	int ret;
	ret = connect(m_Socket, (const sockaddr *)&dest_addr, len);
	if(ret < 0)  
    {  
        printf("connect failed!");       
        closesocket(m_Socket);   //关闭套接字  
        WSACleanup();           //释放套接字资源;  
        return FALSE;   
    }
	//填充rtp header
	FillRTPHeader();


	m_InitFlag = TRUE;
	return TRUE;
}
BOOL CRTPSender::Uninitialize()
{
	if (!m_Socket)
	{
		closesocket(m_Socket);
	}
	WSACleanup();
	return TRUE;
}
BOOL CRTPSender::FillRTPHeader()
{
	m_RTPHeader.version = 2;
	m_RTPHeader.padding = 0;
	m_RTPHeader.extension = 0;
	m_RTPHeader.csrc_len = 0;
	m_RTPHeader.marker = 0;
	m_RTPHeader.payload = MPTS;
	m_RTPHeader.timestamp = 1000;
	m_RTPHeader.seq_no = 0;
	m_RTPHeader.ssrc = 10;
	return TRUE;
}
BOOL CRTPSender::InitWinsock()
{
	int Error;
	WORD VersionRequested;
	WSADATA WsaData;
	VersionRequested=MAKEWORD(2,2);
	Error=WSAStartup(VersionRequested,&WsaData); //WinSock2
	if(Error!=0)
	{
		return FALSE;
	}
	else
	{
		if(LOBYTE(WsaData.wVersion)!=2||HIBYTE(WsaData.wHighVersion)!=2)
		{
			WSACleanup();
			return FALSE;
		}

	}
	return TRUE;
}
int CRTPSender::Send(BYTE *buf, int len)
{
	char sendBuf[MTU];
	//DumpRaw(buf, len);
	m_RTPHeader.seq_no++;
	memcpy(sendBuf, &m_RTPHeader, sizeof(RTP_FIXED_HEADER));
	memcpy(sendBuf+sizeof(RTP_FIXED_HEADER), buf, len);
	return send(m_Socket, sendBuf, sizeof(RTP_FIXED_HEADER)+len, 0);
}
int CRTPSender::DumpRaw(BYTE *buf, int len)
{
	int iRet;
	FILE *File;
	File = fopen("dump.dat", "ab+");
	iRet = fwrite(buf, len, 1, File);
	fclose(File);
	return iRet;
}