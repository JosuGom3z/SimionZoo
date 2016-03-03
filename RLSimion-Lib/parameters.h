#pragma once
#include <list>

class INumericValue
{
public:
	virtual double getValue() = 0;
};

//this class is a simplified interface of CParameters
class CParameters: private tinyxml2::XMLElement
{
	//We use downcasting, so don't add any non-static attributes here!!
	static std::list<INumericValue*> m_handlers;
public:
	CParameters();
	~CParameters();

	INumericValue* getNumericHandler(const char* paramName);

	bool getConstBoolean(const char* paramName, bool defaultValue= true);
	int getConstInteger(const char* paramName, int defaultValue= 0);
	double getConstDouble(const char* paramName, double defaultValue= 0.0);
	const char* getConstString(const char* paramName= 0, const char* defaultValue= (const char*)0);

	CParameters* getChild(const char* paramName = (const char*)0);
	CParameters* getNextChild(const char* paramName = (const char*)0);
	int countChildren(const char* paramName = 0);

	const char* getName();

	static void freeHandlers();
};