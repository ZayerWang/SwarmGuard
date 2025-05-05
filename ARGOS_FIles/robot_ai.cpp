#include <argos3/core/control_interface/ci_controller.h>
#include <argos3/core/control_interface/ci_differential_steering_actuator.h>
#include <argos3/core/control_interface/ci_proximity_sensor.h>
#include <argos3/core/control_interface/ci_positioning_sensor.h>
#include <argos3/core/control_interface/ci_battery_sensor.h>
#include <argos3/core/control_interface/ci_communication.h>
#include <argos3/utility/math/rng.h>
#include <openssl/sha.h>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace argos;

class CRobotAI : public CCI_Controller {
public:
  void Init(TConfigurationNode &node) override {
    // Sensors & actuators
    m_pcWheels    = GetActuator<CCI_DifferentialSteeringActuator>("differential_steering");
    m_pcProximity = GetSensor<CCI_ProximitySensor>("proximity");
    m_pcPosition  = GetSensor<CCI_PositioningSensor>("positioning");
    m_pcBattery   = GetSensor<CCI_BatterySensor>("battery");
    m_pcComm      = GetActuator<CCI_CommunicationActuator>("communication");
    m_pcCommSense = GetSensor<CCI_CommunicationSensor>("communication");

    // Flag compromised
    m_bIsCompromised = false;
    GetNodeAttribute(node, "compromised", m_bIsCompromised);

    // Open log
    GetNodeAttribute(node, "log_folder", m_strLogFolder);
    std::ostringstream fn;
    fn << m_strLogFolder << "/robot_" << GetId() << "_log.csv";
    m_cLog.open(fn.str());
    m_cLog << "tick,type,header,payload_hash,sender_id,rssi,hop_count," \
            "x,y,prox0,prox1,prox2,token_seen,battery,role,is_compromised" << std::endl;
  }

  void ControlStep() override {
    Wander();
    bool sawToken = SeeToken();
    if(sawToken) ++m_unTokensSeen;

    // Log local state
    LogState("state","","",GetId(),0,0,sawToken,m_pcBattery->GetBatteryLevel(),m_strRole);

    // Inbound
    for(auto *msg : m_pcCommSense->GetPackets()) {
      std::string data((char*)msg->Data, msg->DataSize);
      std::string hash = Hash(data);
      LogState("inbound", msg->Header.ToString(), hash, msg->SenderId, msg->RSSI, msg->HopCount);
    }

    // Outbound every interval
    if(m_unTime % m_unReportInterval == 0) {
      CByteArray pkt;
      pkt << m_unTokensSeen;
      if(m_bIsCompromised) pkt[0] += RNG::GetUniformInt(1,5);
      m_pcComm->Send(pkt);
      std::string str((char*)pkt.ToCArray(), pkt.Size());
      LogState("outbound","PREPARE",Hash(str),GetId(),0,0);
    }

    ++m_unTime;
  }

  void Reset() override { if(m_cLog.is_open()) m_cLog.close(); }

private:
  void Wander();
  bool SeeToken();
  std::string Hash(const std::string &d) {
    unsigned char md[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)d.c_str(), d.size(), md);
    std::ostringstream oss;
    for(int i=0;i<SHA256_DIGEST_LENGTH;++i)
      oss << std::hex << std::setw(2) << std::setfill('0') << (int)md[i];
    return oss.str();
  }

  void LogState(const std::string &type,
                const std::string &header,
                const std::string &payload_hash,
                const std::string &sender,
                Real rssi, UInt8 hop,
                bool token=false,
                Real batt=0,
                const std::string &role="") {
    auto pos = m_pcPosition->GetReading().Position;
    auto prox = m_pcProximity->GetReadings();
    m_cLog << m_unTime << ","
           << type << ","
           << header << ","
           << payload_hash << ","
           << sender << ","
           << rssi << ","
           << hop << ","
           << pos.GetX() << ","
           << pos.GetY() << ","
           << prox[0] << ","
           << prox[1] << ","
           << prox[2] << ","
           << token << ","
           << batt << ","
           << role << ","
           << m_bIsCompromised << std::endl;
  }

  CCI_DifferentialSteeringActuator *m_pcWheels;
  CCI_ProximitySensor *m_pcProximity;
  CCI_PositioningSensor *m_pcPosition;
  CCI_BatterySensor *m_pcBattery;
  CCI_CommunicationActuator *m_pcComm;
  CCI_CommunicationSensor *m_pcCommSense;
  bool m_bIsCompromised;
  UInt32 m_unTokensSeen=0, m_unTime=0, m_unReportInterval=50;
  std::string m_strRole="replica";
  std::string m_strLogFolder;
  std::ofstream m_cLog;
};
REGISTER_CONTROLLER(CRobotAI, "robot_ai");