#include "Calibration.h"

unsigned int preset_values[3] = {0u};

Calibration::Calibration(Motor *motor) {
    this->motor = motor;

    for (uint8_t i = 0; i < 3; i++) {
#ifdef __EEPROM__
        EEPROM.get(ADDRESS_PRESETS[i], preset_values[i]);

#ifdef __DEBUG__
        Serial.print("preset ");
        Serial.print(i);
        Serial.print(" ");
        Serial.print(preset_values[i]);
        Serial.println();
#endif
#endif
    }
}

void Calibration::uncalibrated() {
    for (int i = 0; i < 3; i++) {
        preset_values[i] = 0u;
#ifdef __EEPROM__
        updateEEPROM(ADDRESS_PRESETS[i], preset_values[i]);
#endif
    }
    motor->set_mode(UNCALIBRATED);
    motor->set_end_stop(~0u);
    motor->reset_position();
}

void Calibration::semi_calibrated() {
    motor->set_mode(SEMICALIBRATED);
    motor->reset_position();
}

void Calibration::calibrated(unsigned int offset) {
    motor->set_mode(CALIBRATED);
    unsigned int end_stop = motor->get_position() - offset;
    motor->set_end_stop(end_stop);
    if (offset) {
        motor->set_position(end_stop);
    }
}

void Calibration::calibrate() {
    calibrating = false;
    switch (motor->get_mode()) {
        case UNCALIBRATED:
            semi_calibrated();
            break;
        case SEMICALIBRATED:
            calibrated();
            break;
        default:
            uncalibrated();
            break;
    }
}

void Calibration::auto_calibrate() {
    uncalibrated();
    motor->dir_ccw();
    calibrating = true;
}

bool Calibration::is_calibrating() {
    return calibrating;
};

void Calibration::set_preset(uint8_t i) {
    if (motor->get_mode() != CALIBRATED)
        return;
    preset_values[i] = motor->get_position();
#ifdef __EEPROM__
    updateEEPROM(ADDRESS_PRESETS[i], preset_values[i]);
#endif
#ifdef __DEBUG__
    Serial.print("set preset: ");
    Serial.print(i);
    Serial.print(" ");
    Serial.print(preset_values[i]);
    Serial.println();
#endif
}

unsigned int Calibration::get_preset(uint8_t i) {
    return preset_values[i];
}

void Calibration::cycle(unsigned long now) {
    if (!calibrating) {
        return;
    }

    if (motor->get_state() == OFF) {
        switch (motor->get_mode()) {
            case UNCALIBRATED:
                semi_calibrated();
                motor->dir_cw();
                offset = true;
                break;
            case SEMICALIBRATED:
                calibrated(END_STOP_OFFSET);
                calibrating = false;
                break;
        }
    }
}