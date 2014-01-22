#pragma once


#ifndef _H_CCMEMINI_H_
#define _H_CCMEMINI_H_

#include <map>
#include "CCObject.h"


NS_CC_BEGIN

typedef std::map<std::string, std::string> KeyValueMap;
typedef KeyValueMap::iterator KeyValueMapIt;
typedef KeyValueMap::value_type KeyValueMapType;

typedef std::map<std::string, KeyValueMap> SectionMap;
typedef SectionMap::iterator SectionMapIt;
typedef SectionMap::value_type SectionMapType;

class CC_DLL CCIniReader: public CCObject
{
public:
	CCIniReader(void);
	~CCIniReader(void);

	static CCIniReader* createFromData(unsigned char *pData, int nSize);
	static CCIniReader* createFromFile( const char *szPath );

	bool initWithData(unsigned char *pData, int nSize);

	std::string getValue(const char *szSection, const char *szKey, const char *szDefault);
	int getValue(const char *szSection, const char *szKey, int nDefault);
	float getValue(const char *szSection, const char *szKey, float nDefault);

private:
	bool set( const char *szSection, const char *szKey, const char *strValue );
	bool get( const char *szSection, const char *szKey, std::string &strValue );
	bool makeString(const char *pData, int nSize, int nSta, int nEnd, std::string &strOut);
	bool makeSection(const std::string &strIn , std::string &strSection);
	bool makeKeyValue(const std::string &strIn , std::string &strKey, std::string &strValue);
private:
	SectionMap m_SectionMap;
};

NS_CC_END

#endif
