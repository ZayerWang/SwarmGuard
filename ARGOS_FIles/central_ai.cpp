#include <argos3/core/control_interface/ci_controller.h>
#include <argos3/core/control_interface/ci_communication.h>
#include <fstream>

using namespace argos;

struct TEvent { std::string type; UInt32 view, seq; std::string sender, detail; };

class CCentralAI : public CCI_Controller {
public:
  void Init(TConfigurationNode &node) override {
    m_pcComm      = GetActuator<CCI_CommunicationActuator>("communication");
    m_pcCommSense = GetSensor<CCI_CommunicationSensor>("communication");
    GetNodeAttribute(node, "log_folder", m_strLogFolder);
    std::ostringstream fn;
    fn << m_strLogFolder << "/central_log.csv";
    m_cLog.open(fn.str());
    m_cLog << "tick,event,view,seq,sender_id,detail" << std::endl;
    // init PBFT state
  }

  void ControlStep() override {
    auto evts = ReceiveAndProcess();
    for(auto &e : evts) {
      m_cLog << m_unElapsed << ","
             << e.type << ","
             << e.view << ","
             << e.seq << ","
             << e.sender << ","
             << e.detail << std::endl;
    }
    if(ConsensusReached()) {
      LOG << "Final count: " << m_nAgreedCount << std::endl;
      LOG << "Coverage:    " << ComputeCoverage() << std::endl;
      DumpDistributionMap();
    }
    ++m_unElapsed;
  }

  void Reset() override { if(m_cLog.is_open()) m_cLog.close(); }

private:
  std::vector<TEvent> ReceiveAndProcess();
  bool ConsensusReached();
  Real ComputeCoverage();
  void DumpDistributionMap();

  CCI_CommunicationActuator *m_pcComm;
  CCI_CommunicationSensor   *m_pcCommSense;
  std::string m_strLogFolder;
  std::ofstream m_cLog;
  UInt32 m_unElapsed=0, m_nAgreedCount=0;
};
REGISTER_CONTROLLER(CCentralAI, "central_ai");