#pragma once
#include "DataProcess.h"

class CDataCapture
{
public:
	CDataCapture(void);
	virtual ~CDataCapture(void);

public:
	int Open(CDataProcess *pProcess);
	int Close();
	int Input(const LPVOID lpData,const DWORD dwSize);

private:
	CDataProcess *m_pDataProcess;
	int			m_iCount;		//数据计数器
	int			m_iRowIndex;	//行索引
	bool        m_bFindDbFive;	//标记是否找到55
	byte*		m_pInData;		//接收数据缓冲
	byte*		m_pOutData;		//输出数据缓冲
};

