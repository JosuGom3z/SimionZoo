#pragma once

class CFeatureMap;
class CFeatureList;
class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
class CParameters;

#include "parameterized-object.h"
//CLinearVFA////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

class CLinearVFA: public CParamObject
{
protected:
	CFeatureMap* m_pFeatureMap;
	double* m_pWeights;
	unsigned int m_numWeights;
	CFeatureList *m_pAux;

	bool m_bSaturateOutput;
	double m_minOutput, m_maxOutput;
public:

	CLinearVFA(CParameters* pParameters);
	~CLinearVFA();

	void saturateOutput(double min, double max);

	double getValue(CFeatureList *features);
	double getValue(CState *s, CAction *a);

	double *getWeightPtr(){return m_pWeights;}
	unsigned int getNumWeights(){return m_numWeights;}

	void getFeatures(CState* s,CAction* a,CFeatureList* outFeatures);
	void getFeatureStateAction(unsigned int feature, CState* s, CAction* a);
	void add(CFeatureList* pFeatures,double alpha= 1.0);

	void save(void* pFile);
	void load(void* pFile);
};

