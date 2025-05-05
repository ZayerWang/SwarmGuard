#include "swarm_loop_functions.h"
#include <argos3/core/simulator/simulator.h>
#include <argos3/core/utility/configuration/argos_configuration.h>
#include <argos3/core/utility/math/rng.h>
#include <argos3/plugins/robots/e-puck/simulator/epuck_entity.h>

using namespace argos;

SwarmLoopFunctions::SwarmLoopFunctions() {}
SwarmLoopFunctions::~SwarmLoopFunctions() {}

void SwarmLoopFunctions::Init(TConfigurationNode &node) {
  GetNodeAttribute(node, "field_size_x", m_fieldX);
  GetNodeAttribute(node, "field_size_y", m_fieldY);
  GetNodeAttribute(node, "num_tokens", m_numTokens);
  GetNodeAttribute(node, "num_robots", m_numRobots);
  GetNodeAttribute(node, "num_compromised", m_numCompromised);
  GetNodeAttribute(node, "log_folder", m_strLogFolder);
  m_pcRNG = CRandom::CreateRNG("argos");
}

void SwarmLoopFunctions::PreExperiment() {
  for(UInt32 i = 0; i < m_numTokens; ++i) {
    CVector3 pos(
      m_pcRNG->Uniform(CRange<Real>(0, m_fieldX)),
      m_pcRNG->Uniform(CRange<Real>(0, m_fieldY)),
      0
    );
    CSimulator::GetInstance().GetSpace().AddEntity("token", "", pos);
  }
  for(UInt32 i = 0; i < m_numRobots; ++i) {
    std::ostringstream id;
    id << "e-puck_" << i;
    CVector3 pos(
      m_pcRNG->Uniform(CRange<Real>(0, m_fieldX)),
      m_pcRNG->Uniform(CRange<Real>(0, m_fieldY)),
      0
    );
    CEPuckEntity &bot = dynamic_cast<CEPuckEntity&>(
      CSimulator::GetInstance().GetSpace().AddEntity("e-puck", id.str(), pos)
    );
    if(i < m_numCompromised)
      m_CompromisedIDs.push_back(id.str());
  }
}

void SwarmLoopFunctions::PreStep() {}
void SwarmLoopFunctions::PostStep() {}
void SwarmLoopFunctions::Reset() {}

void SwarmLoopFunctions::PostExperiment() {
  // Optional: global summary
}

REGISTER_LOOP_FUNCTIONS(SwarmLoopFunctions, "SwarmLoopFunctions");