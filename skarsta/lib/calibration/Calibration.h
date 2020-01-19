#ifndef SKARSTA_CALIBRATION_H
#define SKARSTA_CALIBRATION_H

#include <Service.h>
#include <Motor.h>

#define END_STOP_OFFSET 50

#ifdef __EEPROM__
const size_t ADDRESS_PRESETS[3] = {
        (ADDRESS_MODE + sizeof(unsigned int)),
        (ADDRESS_MODE + 2 * sizeof(unsigned int)),
        (ADDRESS_MODE + 3 * sizeof(unsigned int))};
#endif

class Calibration : public Service {
private:
    Motor *motor = nullptr;
    bool calibrating = false, offset = false;
protected:

    void uncalibrated();

    void semi_calibrated();

    void calibrated(unsigned int offset = 0);

    bool end_stop();

public:
    Calibration(Motor *motor);

    bool is_calibrating();

    void calibrate();

    void auto_calibrate();

    void set_preset(uint8_t i);

    unsigned int get_preset(uint8_t i);

    void cycle(unsigned long now) override;
};

#endif //SKARSTA_CALIBRATION_H
