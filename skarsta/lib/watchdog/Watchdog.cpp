#include "Watchdog.h"

Watchdog::Watchdog(Motor *m, Display *d, Calibration *c) {
    this->motor = m;
    this->display = d;
    this->calibration = c;
}

void Watchdog::cycle(unsigned long now) {
    static unsigned long last_tick = now;
    if (get_period(last_tick, now) >= WATCHDOG_TIMEOUT) {
        unsigned int pos_diff = motor->get_position_change();
        MotorState state = motor->get_state();

        if (pos_diff <= WATCHDOG_DEADLOCK_CHANGE && state != OFF) {
            if (calibration->is_calibrating()) {
                motor->off();
            }
            error(1);
#ifdef __DEBUG__
            Serial.print("w-1 stop d:");
            Serial.println(pos_diff);
#endif
        } else if (pos_diff > WATCHDOG_OTHER_CHANGE && state == OFF) {
            error(2);
#ifdef __DEBUG__
            Serial.print("w-2 stop d:");
            Serial.println(pos_diff);
#endif
        } else {
            error_count = 0;
#ifdef __DEBUG__
            Serial.print("w d:");
            Serial.println(pos_diff);
#endif
        }

        last_tick = now;
    }
}

void Watchdog::error(uint8_t cause) {
    if (error_count++ < WATCHDOG_TOLERANCE - 1) {
#ifdef __DEBUG__
        Serial.print("w ec:");
        Serial.println(error_count);
#endif
        return;
    }
    motor->disable();
    display->disable(cause);
}
