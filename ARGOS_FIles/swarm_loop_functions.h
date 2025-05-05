#ifndef SWARM_LOOP_FUNCTIONS_H
#define SWARM_LOOP_FUNCTIONS_H

#include <argos3/core/simulator/loop_functions.h>

using namespace argos;

class SwarmLoopFunctions : public CLoopFunctions {
public:
  SwarmLoopFunctions();
  ~SwarmLoopFunctions();
  void Init(TConfigurationNode &node) override;
  void PreExperiment() override;
  void PostExperiment() override;
  void PreStep() override;
  void PostStep() override;
  void Reset() override;

private:
  UInt32 m_fieldX, m_fieldY;
  UInt32 m_numTokens, m_numRobots, m_numCompromised;
  std::string m_strLogFolder;
  std::vector<std::string> m_CompromisedIDs;
  CRandom::CRNG* m_pcRNG;
};

#endif