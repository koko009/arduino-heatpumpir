#ifndef SharpHeatpumpIR_h
#define SharpHeatpumpIR_h

#include <HeatpumpIR.h>

#define MODEL_SHARP 0
#define MODEL_IVT   1

// Sharp timing constants
#define SHARP_AIRCON1_HDR_MARK   3540
#define SHARP_AIRCON1_HDR_SPACE  1720
#define SHARP_AIRCON1_BIT_MARK   460
#define SHARP_AIRCON1_ONE_SPACE  1400
#define SHARP_AIRCON1_ZERO_SPACE 430

// Sharp codes
#define SHARP_AIRCON1_MODE_HEAT  0x01
#define SHARP_AIRCON1_MODE_COOL  0x02
#define SHARP_AIRCON1_MODE_DRY   0x03
#define SHARP_AIRCON1_MODE_OFF   0x21
#define SHARP_AIRCON1_MODE_ON    0x31
#define SHARP_AIRCON1_FAN_AUTO   0x20
#define SHARP_AIRCON1_FAN1       0x30
#define SHARP_AIRCON1_FAN2       0x50
#define SHARP_AIRCON1_FAN3       0x70

// IVT codes
#define SHARP_AIRCON2_MODE_ON    0x11

class SharpHeatpumpIR : public HeatpumpIR
{
  public:
    SharpHeatpumpIR();
    void send(IRSender& IR, uint8_t powerModeCmd, uint8_t operatingModeCmd, uint8_t fanSpeedCmd, uint8_t temperatureCmd, uint8_t swingVCmd, uint8_t swingHCmd);

    bool isOn() const { return _isOn; }
    uint8_t getMode() const { return _lastMode; }
    uint8_t getTemp() const { return _lastTemp; }

  protected:
    void sendSharp(IRSender& IR, uint8_t powerMode, uint8_t operatingMode, uint8_t fanSpeed, uint8_t temperature);
    uint8_t _sharpModel;

  private:
    bool _isOn = false;
    uint8_t _lastMode = MODE_HEAT;
    uint8_t _lastTemp = 23;
};

#endif
