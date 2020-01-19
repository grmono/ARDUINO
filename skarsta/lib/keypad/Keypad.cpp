#include "Keypad.h"

static Keypad *keypad = nullptr;
static Motor *motor = nullptr;
static Display *display = nullptr;
static Calibration *calibration = nullptr;

#define SET_PRESET_BUTTON(p, on) preset_buttons[p]->long_press(SAVE_BUTTON_TIMEOUT)\
                                    ->on_short_press([]() { if (!keypad->stop_motor()) keypad->goto_preset(p, calibration->get_preset(p)); })\
                                    ->on_long_press([]() { calibration->set_preset(p); })\
                                    ->on_press(on)

Keypad::Keypad(Motor *_motor, Display *_display, Calibration *_calibration) :
        down(BUTTON_DOWN), up(BUTTON_UP), rst(BUTTON_RST),
        preset_0(BUTTON_P0), preset_1(BUTTON_P1), preset_2(BUTTON_P2),
        preset_buttons({&preset_0, &preset_1, &preset_2}),
        buttons({&preset_0, &preset_1, &preset_2, &down, &up, &rst}) {
    motor = _motor;
    display = _display;
    calibration = _calibration;

    keypad = this;
    auto motor_off = []() { motor->off(); };
    // up/down
    down.on_press([]() { if (!keypad->stop_motor()) motor->dir_ccw(); });
    down.on_release(motor_off);
    up.on_press([]() { if (!keypad->stop_motor()) motor->dir_cw(); });
    up.on_release(motor_off);
    // display light-up callback
    auto light_up = []() { display->light_up(); };
    // presets
    SET_PRESET_BUTTON(0, light_up);
    SET_PRESET_BUTTON(1, light_up);
    SET_PRESET_BUTTON(2, light_up);
    // calibration
    rst.long_press(RST_BUTTON_TIMEOUT);
    rst.on_short_press([]() { keypad->stop_motor(); });
    rst.on_long_press([]() { calibration->calibrate(); });
    rst.on_llong_press([]() { calibration->auto_calibrate(); });
    rst.on_press(light_up);
}

void Keypad::begin() {
    for (auto &button : buttons)
        button->cycle();
}

void Keypad::cycle() {
    for (auto &button : buttons)
        button->cycle();

    if (rst.held()) {
        display->set_blink(false);
        display->print("-rst");
        return;
    }
    for (auto &preset_button : preset_buttons) {
        if (preset_button->held() && motor->get_mode() == CALIBRATED) {
            display->set_blink(false);
            display->print("-set");
            return;
        }
    }

    static unsigned int pos_watch = 0;
    if (pos_watch != motor->get_position()) {
        pos_watch = motor->get_position();
        display->set_blink(false);
    } else if (motor->get_mode() == SEMICALIBRATED)
        display->set_blink(true);

    if (motor->get_mode() != UNCALIBRATED)
        display->print(motor->get_position());
    else if (motor->get_mode() == UNCALIBRATED) {
        display->set_blink(true);
        display->print(DISPLAY_NONE);
    }
}

void Keypad::goto_preset(uint8_t i, unsigned int pos) {
    if (motor->get_mode() != CALIBRATED)
        return;
    motor->set_position(pos);
}

bool Keypad::stop_motor() {
    if (motor->get_state() == OFF)
        return false;
    motor->off();
    return true;
}
