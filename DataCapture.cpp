#include "StdAfx.h"
#include "DataCapture.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CDataCapture::CDataCapture(void)
{
	m_pDataProcess=NULL;
	m_iCount=0;		
	m_iRowIndex=0;		
	m_bFindDbFive=FALSE;	
	m_pInData=NULL;		
	m_pOutData=NULL;		
}

CDataCapture::~CDataCapture(void)
{
	Close();
}
int CDataCapture::Open( CDataProcess *pProcess )
{
	ASSERT(pProcess!=NULL);
	m_pDataProcess=pProcess;
	m_pOutData=new byte[g_height*g_width_L];
	m_pInData=new byte[(ReadDataBytes+g_width_L+3)];
	memset(m_pInData,0,(ReadDataBytes+g_width_L+3)*sizeof(byte));
	return 0;
}
int CDataCapture::Close()
{
	if(m_pOutData!=NULL)
	{
		delete[] m_pOutData;
		m_pOutData=NULL;
	}
	if(m_pInData!=NULL)
	{
		delete[] m_pInData;
		m_pInData=NULL;
	}
	return 0;
}

int CDataCapture::Input( const LPVOID lpData,const DWORD dwSize )
{
	int iBytes=0;
	iBytes=dwSize+m_iCount;//m_iCount上一次拷贝剩余数据
	memcpy(m_pInData+m_iCount,lpData,dwSize);
	for(int i=0;i<iBytes;++i)
	{
		if(!m_bFindDbFive)
		{
			if(m_pInData[i]==0x55)
			{
				m_bFindDbFive=TRUE;
			}
			continue;
		}
		if(0xaa==m_pInData[i])
		{
			if((i+g_width_L+2)>=iBytes)//如果剩下的最后几个数据长度小于video_width*2+2行号个，不足以构成完整一行，拷贝到下一缓存
			{
				m_iCount=iBytes-i;
				memcpy(m_pInData,m_pInData+i,m_iCount);
				return 0;
			}
			m_iRowIndex=m_pInData[i+1];		//行号高8位
			m_iRowIndex<<=8;				 
			m_iRowIndex+=m_pInData[i+2];	//行号低8位
			if(m_iRowIndex>g_height+2){
				//AfxMessageBox(L"行号出错");
				return 0;
				//exit(1);
			}
			memcpy(m_pOutData+m_iRowIndex*g_width_L,m_pInData+i+3,g_width_L);
			//int tmp1 = m_pInData[i+3];
			//int tmp2 = m_pInData[i+4];
			//int tmp3 = m_pInData[i+5];
			if(m_iRowIndex>=(g_height-1))
			{
				m_pDataProcess->Input(m_pOutData,g_height*g_width_L);
				memset(m_pOutData,0,sizeof(byte)*g_height*g_width_L);
			}
			i=i+g_width_L+2;
		}
		m_bFindDbFive=FALSE;//找到0x55后，无论下个数据是不是0xaa都置状态位为FALSE,然后重新找0x55
	}
	return 0;
}
