#include "panasonic_heatpump_switch.h"

namespace esphome {
namespace panasonic_heatpump {
static const char* const TAG = "panasonic_heatpump.switch";

void PanasonicHeatpumpSwitch::dump_config() {
  LOG_SWITCH("", "Panasonic Heatpump Switch", this);
  delay(10);
}

void PanasonicHeatpumpSwitch::write_state(bool state) {
  int value = state ? 1 : 0;

  switch (this->id_) {
  case SwitchIds::CONF_SET1:
    this->parent_->set_command_byte(PanasonicCommand::setPlus1(value), 4);
    break;
  case SwitchIds::CONF_SET10:
    this->parent_->set_command_byte(PanasonicCommand::setPlus1Multiply64(value), 4);
    break;
  case SwitchIds::CONF_SET12:
    this->parent_->set_command_byte(PanasonicCommand::setMultiply2(value), 8);
    break;
  case SwitchIds::CONF_SET13:
    this->parent_->set_command_byte(PanasonicCommand::setMultiply4(value), 8);
    break;
  case SwitchIds::CONF_SET14:
    this->parent_->set_command_byte(PanasonicCommand::setPlus1Multiply16(value), 4);
    break;
  case SwitchIds::CONF_SET24:
    this->parent_->set_command_byte(PanasonicCommand::setPlus1Multiply64(value), 5);
    break;
  case SwitchIds::CONF_SET25:
    this->parent_->set_command_byte(PanasonicCommand::setPlus1Multiply16(value), 20);
    break;
  case SwitchIds::CONF_SET28:
    this->parent_->set_command_byte(PanasonicCommand::setPlus1Multiply4(value), 24);
    break;
  case SwitchIds::CONF_SET30:
    this->parent_->set_command_byte(PanasonicCommand::setPlus1(value), 23);
    break;
  case SwitchIds::CONF_SET31:
    this->parent_->set_command_byte(PanasonicCommand::setPlus1Multiply16(value), 23);
    break;
  case SwitchIds::CONF_SET32:
    this->parent_->set_command_byte(PanasonicCommand::setPlus1Multiply64(value), 23);
    break;
  case SwitchIds::CONF_SET33:
    this->parent_->set_command_byte(PanasonicCommand::setPlus1Multiply4(value), 23);
    break;
  case SwitchIds::CONF_SET34:
    this->parent_->set_command_byte(PanasonicCommand::setPlus1(value), 26);
    break;
  case SwitchIds::CONF_ERROR:
    this->parent_->set_command_byte(value, 8);
    break;
  case SwitchIds::CONF_FORCE_HEATER: {
    // Byte 5 is shared with holiday mode, main schedule and dry concrete.
    // Preserve the other fields instead of zeroing the whole byte.
    int current = this->parent_->get_response_byte(5);
    if (current < 0)
      return;
    uint8_t value_byte = (static_cast<uint8_t>(current) & 0b11110011) | (state ? 0b00001000 : 0b00000100);
    this->parent_->set_command_byte(value_byte, 5);
    break;
  }
  case SwitchIds::CONF_QUIET_MODE_SCHEDULE: {
    int current = this->parent_->get_response_byte(7);
    if (current < 0)
      return;
    uint8_t quiet = (static_cast<uint8_t>(current) & 0b00111111) | (state ? 0x80 : 0x40);
    this->parent_->set_command_byte(quiet, 7);
    break;
  }
  default:
    return;
  };

  this->publish_state(state);
  this->keep_state_ = KEEP_STATE;
}

void PanasonicHeatpumpSwitch::publish_new_state(const std::vector<uint8_t>& data) {
  if (this->keep_state_ > 0) {
    this->keep_state_--;
    return;
  }
  if (data.empty())
    return;

  bool new_state;
  switch (this->id_) {
  case SwitchIds::CONF_SET1:
    new_state = PanasonicDecode::getBinaryState(PanasonicDecode::getBit7and8(data[4]));
    if (this->state == new_state)
      return;
    break;
  case SwitchIds::CONF_SET10:
    new_state = PanasonicDecode::getBinaryState(PanasonicDecode::getBit1and2(data[4]));
    if (this->state == new_state)
      return;
    break;
  case SwitchIds::CONF_SET12:
    new_state = PanasonicDecode::getBinaryState(PanasonicDecode::getBit5and6(data[111]));
    if (this->state == new_state)
      return;
    break;
  case SwitchIds::CONF_SET13:
    new_state = PanasonicDecode::getBinaryState(PanasonicDecode::getBit5and6(data[117]));
    if (this->state == new_state)
      return;
    break;
  case SwitchIds::CONF_SET24:
    new_state = PanasonicDecode::getBinaryState(PanasonicDecode::getBit1and2(data[5]));
    if (this->state == new_state)
      return;
    break;
  case SwitchIds::CONF_SET25:
    new_state = PanasonicDecode::getBinaryState(PanasonicDecode::getBit3and4(data[20]));
    if (this->state == new_state)
      return;
    break;
  case SwitchIds::CONF_SET28:
    new_state = PanasonicDecode::getBinaryState(PanasonicDecode::getBit5and6(data[24]));
    if (this->state == new_state)
      return;
    break;
  case SwitchIds::CONF_SET30:
    new_state = PanasonicDecode::getBinaryState(PanasonicDecode::getBit7and8(data[23]));
    if (this->state == new_state)
      return;
    break;
  case SwitchIds::CONF_SET31:
    new_state = PanasonicDecode::getBinaryState(PanasonicDecode::getBit3and4(data[23]));
    if (this->state == new_state)
      return;
    break;
  case SwitchIds::CONF_SET32:
    new_state = PanasonicDecode::getBinaryState(PanasonicDecode::getBit1and2(data[23]));
    if (this->state == new_state)
      return;
    break;
  case SwitchIds::CONF_SET33:
    new_state = PanasonicDecode::getBinaryState(PanasonicDecode::getBit5and6(data[23]));
    if (this->state == new_state)
      return;
    break;
  case SwitchIds::CONF_SET34:
    new_state = PanasonicDecode::getBinaryState(PanasonicDecode::getBit7and8(data[26]));
    if (this->state == new_state)
      return;
    break;
  case SwitchIds::CONF_ERROR:
    new_state = PanasonicDecode::getBinaryState(data[8]);
    if (this->state == new_state)
      return;
    break;
  case SwitchIds::CONF_FORCE_HEATER:
    new_state = PanasonicDecode::getBit5and6(data[5]) == 1;
    if (this->state == new_state)
      return;
    break;
  case SwitchIds::CONF_QUIET_MODE_SCHEDULE:
    new_state = PanasonicDecode::getBit1and2(data[7]) == 1;
    if (this->state == new_state)
      return;
    break;
  default:
    return;
  };

  this->publish_state(new_state);
}
}  // namespace panasonic_heatpump
}  // namespace esphome
