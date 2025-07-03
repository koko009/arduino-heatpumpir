#include <SharpHeatpumpIR.h>

SharpHeatpumpIR::SharpHeatpumpIR() : HeatpumpIR()
{
  static const char model[] PROGMEM = "sharp";
  static const char info[]  PROGMEM = "{\"mdl\":\"sharp\",\"dn\":\"Sharp AY-ZP40KR\",\"mT\":18,\"xT\":32,\"fs\":3,\"maint\":[10]}}";

  _model = model;
  _info = info;

  _sharpModel = MODEL_SHARP;
}


void SharpHeatpumpIR::send(IRSender& IR, uint8_t powerModeCmd, uint8_t operatingModeCmd, uint8_t fanSpeedCmd, uint8_t temperatureCmd, uint8_t swingVCmd, uint8_t swingHCmd)
{
  (void)swingVCmd;
  (void)swingHCmd;

  uint8_t powerMode     = _sharpModel == MODEL_SHARP ? SHARP_AIRCON1_MODE_ON : SHARP_AIRCON2_MODE_ON;
  uint8_t operatingMode = SHARP_AIRCON1_MODE_HEAT;
  uint8_t fanSpeed      = SHARP_AIRCON1_FAN_AUTO;
  uint8_t temperature   = 23;

  // إذا المستخدم أرسل أمر إيقاف فقط
  if (powerModeCmd == 0) {
    powerMode = SHARP_AIRCON1_MODE_OFF;
    _isOn = false;
  } else {
    // وإلا، فهو بيطلب تشغيل، فنسجل الحالة
    _isOn = true;
  }

  switch (operatingModeCmd)
  {
    case MODE_HEAT:
      operatingMode = SHARP_AIRCON1_MODE_HEAT;
      break;
    case MODE_COOL:
      operatingMode = SHARP_AIRCON1_MODE_COOL;
      break;
    case MODE_DRY:
      operatingMode = SHARP_AIRCON1_MODE_DRY;
      temperatureCmd = 10;
      break;
    case MODE_FAN:
      operatingMode = SHARP_AIRCON1_MODE_COOL;
      temperatureCmd = 32;
      break;
    case MODE_MAINT:
      operatingMode = SHARP_AIRCON1_MODE_HEAT;
      temperature = 10;
      fanSpeedCmd = FAN_5;
      break;
  }

  _lastMode = operatingModeCmd;

  switch (fanSpeedCmd)
  {
    case FAN_AUTO:
      fanSpeed = SHARP_AIRCON1_FAN_AUTO;
      break;
    case FAN_1:
      fanSpeed = SHARP_AIRCON1_FAN1;
      break;
    case FAN_2:
      fanSpeed = SHARP_AIRCON1_FAN2;
      break;
    case FAN_3:
      fanSpeed = SHARP_AIRCON1_FAN3;
      break;
  }

  if (temperatureCmd > 16 && temperatureCmd < 32) {
    temperature = temperatureCmd;
  }

  _lastTemp = temperature;

  sendSharp(IR, powerMode, operatingMode, fanSpeed, temperature);
}


void SharpHeatpumpIR::sendSharp(IRSender& IR, uint8_t powerMode, uint8_t operatingMode, uint8_t fanSpeed, uint8_t temperature)
{
  uint8_t SharpTemplate[] = { 0xAA, 0x5A, 0xCF, 0x10, 0x00, 0x00, 0x00, 0x06, 0x08, 0x80, 0x04, 0xF0, 0x01 };

  uint8_t checksum = 0x00;

  SharpTemplate[5] = powerMode;
  SharpTemplate[6] = fanSpeed | operatingMode;

  SharpTemplate[4] = (temperature == 10) ? 0x00 : (temperature - 17);

  for (int i = 0; i < 12; i++) {
    checksum ^= SharpTemplate[i];
  }

  checksum ^= SharpTemplate[12] & 0x0F;
  checksum ^= (checksum >> 4);
  checksum &= 0x0F;

  SharpTemplate[12] |= (checksum << 4);

  IR.setFrequency(38);
  IR.mark(SHARP_AIRCON1_HDR_MARK);
  IR.space(SHARP_AIRCON1_HDR_SPACE);

  for (unsigned int i = 0; i < sizeof(SharpTemplate); i++) {
    IR.sendIRbyte(SharpTemplate[i], SHARP_AIRCON1_BIT_MARK, SHARP_AIRCON1_ZERO_SPACE, SHARP_AIRCON1_ONE_SPACE);
  }

  IR.mark(SHARP_AIRCON1_BIT_MARK);
  IR.space(0);
}
