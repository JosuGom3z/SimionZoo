#pragma once

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;

class CParameters;


class CActor
{
protected:
	int m_numOutputs;

	virtual void savePolicy(const char* pFilename){};
	virtual void loadPolicy(const char* pFilename){};
public:
	CActor(){}
	virtual ~CActor(){};

	virtual void selectAction(CState *s,CAction *a)= 0;

	virtual void updatePolicy(CState *s, CAction *a, CState *s_p, double r, double td){};

	virtual double getProbability(CState* s, CAction* a){ return 1.0; }

	static CActor *getInstance(CParameters* pParameters);
};
