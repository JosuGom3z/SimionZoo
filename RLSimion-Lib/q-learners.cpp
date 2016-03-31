#include "stdafx.h"
#include "q-learners.h"
#include "named-var-set.h"
#include "vfa.h"
#include "globals.h"
#include "parameters.h"
#include "parameters-numeric.h"
#include "noise.h"
#include "etraces.h"
#include "world.h"


///////////////////////////////////////
//Q-function-based POLICIES
CLASS_FACTORY(CQPolicy)
{
	CHOICE("Policy", "The exploration policy used to learn");
	CHOICE_ELEMENT("Epsilon-Greedy", CQEGreedyPolicy, "Epsilon-greedy: with probability epsilon the action with the highest Q-value. With probability 1-epsilon a random action");
	CHOICE_ELEMENT("Soft-Max", CQSoftMaxPolicy, "Soft-Max policy: higher probability to actions with higher Q-values. The temperature parameter tau balances the difference of probabilities.");
	END_CHOICE();
	END_CLASS();
}

//Epsilon-greedy
CLASS_CONSTRUCTOR(CQEGreedyPolicy)
{
	NUMERIC_VALUE(m_pEpsilon, "Epsilon", "The epsilon parameter that balances exploitation and exploration");
	END_CLASS();
}

CQEGreedyPolicy::~CQEGreedyPolicy()
{
	delete m_pEpsilon;
}

void CQEGreedyPolicy::selectAction(CLinearStateActionVFA* pQFunction, const CState* s, CAction* a)
{
	double epsilon = m_pEpsilon->getValue();
	double randomValue = getRandomValue();

	if (randomValue < epsilon)
		pQFunction->argMax(s, a);
	else
	{
		unsigned int numActionWeights= pQFunction->getNumActionWeights();
		unsigned int randomActionWeight = rand() % numActionWeights;
		pQFunction->getFeatureStateAction(randomActionWeight, 0, a);
	}
}

//Soft-Max
CLASS_CONSTRUCTOR(CQSoftMaxPolicy)
{
	NUMERIC_VALUE(m_pTau, "Tau", "Temperature parameter");
	END_CLASS();
}

CQSoftMaxPolicy::~CQSoftMaxPolicy()
{
	delete m_pTau;
}

void CQSoftMaxPolicy::selectAction(CLinearStateActionVFA* pQFunction, const CState* s, CAction* a)
{
	//TODO
	assert(0);
}

///////////////////////////////////
//Q-Learning

CLASS_CONSTRUCTOR(CQLearning)
{
	CHILD_CLASS(m_pQFunction, "Q-Function", "The parameterization of the Q-Function", false, CLinearStateActionVFA);
	CHILD_CLASS_FACTORY(m_pQPolicy, "Policy", "The policy to be followed", false, CQPolicy);
	CHILD_CLASS(m_eTraces, "E-Traces", "E-Traces", true, CETraces, "Q-Learning/traces");
	END_CLASS();
}
CQLearning::~CQLearning()
{
	delete m_pQFunction;
	delete m_pQPolicy;
	delete m_eTraces;
}

void CQLearning::updateValue(const CState *s, const CAction *a, const CState *s_p, double r)
{
	//https://webdocs.cs.ualberta.ca/~sutton/book/ebook/node78.html
	m_eTraces->update();
	double td = r + m_pGamma->getValue()*m_pQFunction->max(s_p) - m_pQFunction->getValue(s, a);
	m_pQFunction->add(m_eTraces, td*m_pAlpha->getValue());

}

void CQLearning::selectAction(const CState *s, CAction *a)
{
	m_pQPolicy->selectAction(m_pQFunction, s, a);
}

/////////////////////////////////////////////////
//SARSA
CLASS_CONSTRUCTOR(CSARSA) : EXTENDS(CQLearning,pParameters)
{
	m_bNextActionSelected = false;
	m_nextA = RLSimion::World.getDynamicModel()->getActionInstance();
	END_CLASS();
}

CSARSA::~CSARSA()
{
	delete m_nextA;
}

void CSARSA::selectAction(const CState *s, CAction *a)
{
	if (m_bNextActionSelected)
	{
		a->copy(m_nextA);
		m_bNextActionSelected = false;
	}
	else
		m_pQPolicy->selectAction(m_pQFunction, s, a);
}

void CSARSA::updateValue(const CState* s, const CAction* a, const CState* s_p, double r)
{
	//https://webdocs.cs.ualberta.ca/~sutton/book/ebook/node77.html
	m_eTraces->update();

	//select a_t+1
	m_pQPolicy->selectAction(m_pQFunction, s_p, m_nextA);
	m_bNextActionSelected = true;

	double td = r + m_pGamma->getValue()*m_pQFunction->getValue(s_p,m_nextA) - m_pQFunction->getValue(s, a);
	m_pQFunction->add(m_eTraces, td*m_pAlpha->getValue());
}