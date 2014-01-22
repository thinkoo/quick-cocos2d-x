#include "CCIni.h"
#include "platform/CCFileUtils.h"
#include <stdlib.h>

#define CC_MAX_PATH          260

NS_CC_BEGIN


CCIniReader::CCIniReader(void)
{
}


CCIniReader::~CCIniReader(void)
{

}

CCIniReader* CCIniReader::createFromData( unsigned char *pData, int nSize )
{
	CCIniReader* pRet=new CCIniReader();
	if (pRet->initWithData(pData,nSize))
	{
		pRet->autorelease();
		return pRet;
	}
	CC_SAFE_RELEASE(pRet);
	return NULL;
}


bool CCIniReader::initWithData( unsigned char *pData, int nSize )
{
	if (!pData || !nSize)
	{
		return false;
	}

	int nLen = nSize;
	char *pChar = (char *)pData;

	std::vector<std::string> EntryVec;
	for (int nSta = 0, nEnd = 0; nEnd < nLen; ++nEnd)
	{
		if (nEnd == nLen - 1 || pChar[nEnd] == '\r' || pChar[nEnd] == '\n')
		{
			std::string strEntry;
			makeString(pChar, nLen, nSta, nEnd, strEntry);
			EntryVec.push_back(strEntry);
			nSta = nEnd;
		}
	}

	std::string sSection;
	std::string sKey;
	std::string sValue;
	for (unsigned int i = 0; i < EntryVec.size(); ++i)
	{
		if (EntryVec[i].empty())
		{
			continue;
		}
		makeSection(EntryVec[i], sSection);
		makeKeyValue(EntryVec[i], sKey, sValue);
		if (!sSection.empty() && !sKey.empty() && !sValue.empty())
		{
			set(sSection.c_str(), sKey.c_str(), sValue.c_str());
			sKey.clear();
			sValue.clear();
		}
	}

	return true;
}


CCIniReader* CCIniReader::createFromFile( const char *szPath )
{
	std::string pathKey = szPath;
	pathKey = CCFileUtils::sharedFileUtils()->fullPathForFilename(pathKey.c_str());
	unsigned long nSize = 0;
	unsigned char *pBuffer = CCFileUtils::sharedFileUtils()->getFileData(pathKey.c_str(), "rb", &nSize);
	if (!pBuffer || nSize <= 0)
	{
		return NULL;
	}

	CCIniReader* pRet= createFromData(pBuffer, nSize);
	
	CC_SAFE_DELETE_ARRAY(pBuffer);

	return pRet;
}


bool CCIniReader::makeString( const char *pData, int nSize, int nSta, int nEnd, std::string &strOut )
{
	if (nSta >= nSize || nEnd >= nSize )
	{
		return false;
	}
	char str[CC_MAX_PATH] = {0};
	for (int i = nSta, j = 0; i <= nEnd; ++i)
	{
		char x = pData[i];
		if (x == ' ' || x == '\t' || x == '\n' || x == '\f' || x == '\b' || x == '\r')
		{
			continue;
		}
		str[j] = x;
		++j;
	}
	if (str[0]=='#')
	{
		str[0]=0;
	}
	strOut = str;
	return true;
}


bool CCIniReader::makeSection( const std::string &strIn , std::string &strSection )
{
	int nSta = strIn.find_first_of('[');
	int nEnd = strIn.find_first_of(']');
	if (nSta >= 0 && nEnd >= 0)
	{
		strSection = strIn.substr(nSta + 1, nEnd - nSta - 1);
		return true;
	}
	return false;
}

bool CCIniReader::makeKeyValue( const std::string &strIn , std::string &strKey, std::string &strValue )
{
	int nSta = strIn.find_first_of('=');
	if (nSta >= 0)
	{
		strKey = strIn.substr(0, nSta);
		strValue = strIn.substr(nSta + 1, strIn.length() - nSta - 1);
		return true;
	}
	return false;
}

int CCIniReader::getValue( const char *szSection, const char *szKey, int nDefault )
{
	std::string val;
	if(!get(szSection, szKey, val))
	{
		return nDefault;
	}

	return atoi(val.c_str());
}

std::string CCIniReader::getValue( const char *szSection, const char *szKey, const char *szDefault )
{
	std::string val;
	if(!get(szSection, szKey, val))
	{
		return szDefault;
	}
	return val;
}

float CCIniReader::getValue( const char *szSection, const char *szKey, float fDefault )
{
	std::string val;
	if(!get(szSection, szKey, val))
	{
		return fDefault;
	}
	return atof(val.c_str());
}

bool CCIniReader::get( const char *szSection, const char *szKey, std::string &strValue )
{
	SectionMapIt it = m_SectionMap.find(szSection);
	if (it != m_SectionMap.end())
	{
		KeyValueMapIt it1 = it->second.find(szKey);
		if (it1 != it->second.end())
		{
			strValue = it1->second;
			return true;
		}
	}

	return false;
}

bool CCIniReader::set( const char *szSection, const char *szKey, const char *strValue )
{
	SectionMapIt it = m_SectionMap.find(szSection);

	if (it != m_SectionMap.end())
	{
		it->second[szKey] = strValue;
	}
	else
	{
		KeyValueMap info;
		info.insert(KeyValueMapType(szKey, strValue));
		m_SectionMap.insert(SectionMapType(szSection, info));
	}

	return true;
}

NS_CC_END
