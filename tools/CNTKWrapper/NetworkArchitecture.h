#pragma once
#include <vector>
#include "../../3rd-party/tinyxml2/tinyxml2.h"

using namespace std;

class Chain;
class Link;
class Problem;

class NetworkArchitecture
{
protected:
	vector<Chain*> m_chains;
	Problem* m_pParentProblem;
	NetworkArchitecture(tinyxml2::XMLElement* pNode);
	
public:
	NetworkArchitecture();
	~NetworkArchitecture();

	const vector<Chain*> getChains() { return m_chains; }

	void setParentProblem(Problem* pParentProblem) { m_pParentProblem = pParentProblem; }
	const Problem* getParentProblem() const { return m_pParentProblem; }

	static NetworkArchitecture* getInstance(tinyxml2::XMLElement* pNode);

	const Link* getLinkById(const char* id) const;
};

