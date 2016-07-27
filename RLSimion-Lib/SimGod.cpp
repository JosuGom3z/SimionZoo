#include "stdafx.h"
#include "SimGod.h"
#include "globals.h"
#include "experiment.h"
#include "named-var-set.h"
#include "parameters.h"
#include "simion.h"
#include "app.h"
#include "delayed-load.h"
#include "utils.h"
#include "featuremap.h"
#include "experience-replay.h"

CLASS_INIT(CSimGod)
{
	if (!pParameters) return;

	//the global parameterizations of the state/action spaces
	CHILD_CLASS_FACTORY(m_pGlobalStateFeatureMap, "State-Feature-Map", "The state feature map", false, CStateFeatureMap);
	CHILD_CLASS_FACTORY(m_pGlobalActionFeatureMap, "Action-Feature-Map", "The action feature map", true, CActionFeatureMap);
	CHILD_CLASS(m_pExperienceReplay, "Experience-Replay", "The experience replay parameters", true, CExperienceReplay);

	m_numSimions = pParameters->countChildren("Simion");
	m_pSimions = new CSimion*[m_numSimions];
	CParameters* pChild = pParameters->getChild("Simion");
	for (int i = 0; i < m_numSimions; i++)
	{
		m_pSimions[i] = 0;
		MULTI_VALUED_FACTORY(m_pSimions[i], "Simion", "Simions: agents and controllers",CSimion, pChild);
		pChild = pChild->getNextChild("Simion");
	}
	
	END_CLASS();
}

CSimGod::CSimGod()
{
	m_numSimions = 0;
	m_pSimions = 0;
}


CSimGod::~CSimGod()
{
	if (m_pGlobalStateFeatureMap) delete m_pGlobalStateFeatureMap;
	if (m_pGlobalActionFeatureMap) delete m_pGlobalActionFeatureMap;
	if (m_pExperienceReplay) delete m_pExperienceReplay;
	if (m_pSimions)
	{
		for (int i = 0; i < m_numSimions; i++)
		{
			if (m_pSimions[i]) delete m_pSimions[i];
		}
		delete[] m_pSimions;
	}
	for (auto it = m_inputFiles.begin(); it != m_inputFiles.end(); it++) delete (*it);
	m_inputFiles.clear();
	for (auto it = m_outputFiles.begin(); it != m_outputFiles.end(); it++) delete (*it);
	m_outputFiles.clear();
}


void CSimGod::selectAction(CState* s, CAction* a)
{
	for (int i = 0; i < m_numSimions; i++)
		m_pSimions[i]->selectAction(s, a);
}

void CSimGod::update(CState* s, CAction* a, CState* s_p, double r)
{
	CExperienceTuple* pExperienceTuple;

	if (CApp::get()->Experiment.isEvaluationEpisode()) return;

	m_pExperienceReplay->addTuple(s, a, s_p, r);

	int updateBatchSize = m_pExperienceReplay->getUpdateBatchSize();
	for (int tuple = 0; tuple < updateBatchSize; ++tuple)
	{
		pExperienceTuple = m_pExperienceReplay->getRandomTupleFromBuffer();
		//update critic
		for (int i = 0; i < m_numSimions; i++)
			m_pSimions[i]->updateValue(pExperienceTuple->s, pExperienceTuple->a, pExperienceTuple->s_p, pExperienceTuple->r);

		//update actor: might be the controller
		for (int i = 0; i < m_numSimions; i++)
			m_pSimions[i]->updatePolicy(pExperienceTuple->s, pExperienceTuple->a, pExperienceTuple->s_p, pExperienceTuple->r);
	}
}

void CSimGod::registerDelayedLoadObj(CDeferredLoad* pObj,unsigned int loadOrder)
{
	m_delayedLoadObjects.push_back(std::pair<CDeferredLoad*,unsigned int>(pObj,loadOrder));
}

bool myComparison(const std::pair<CDeferredLoad*, int> &a, const std::pair<CDeferredLoad*, int> &b)
{
	return a.second<b.second;
}

void CSimGod::delayedLoad()
{
	std::sort(m_delayedLoadObjects.begin(), m_delayedLoadObjects.end(),myComparison);

	for (auto it = m_delayedLoadObjects.begin(); it != m_delayedLoadObjects.end(); it++)
	{
		(*it).first->deferredLoadStep();
	}
}

void CSimGod::registerInputFile(const char* filepath)
{
	char* copy = new char[strlen(filepath) + 1];
	strcpy_s(copy, strlen(filepath) + 1, filepath);
	m_inputFiles.push_back(copy);
}

void CSimGod::getInputFiles(CFilePathList& filepathList)
{
	for (auto it = m_inputFiles.begin(); it != m_inputFiles.end(); it++)
	{
		filepathList.addFilePath(*it);
	}
}

void CSimGod::registerOutputFile(const char* filepath)
{
	char* copy = new char[strlen(filepath) + 1];
	strcpy_s(copy, strlen(filepath) + 1, filepath);
	m_outputFiles.push_back(copy);
}

void CSimGod::getOutputFiles(CFilePathList& filepathList)
{
	for (auto it = m_outputFiles.begin(); it != m_outputFiles.end(); it++)
	{
		filepathList.addFilePath(*it);
	}
}