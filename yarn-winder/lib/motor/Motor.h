#ifndef ARDUINO_PROJECTS_MOTOR_H
#define ARDUINO_PROJECTS_MOTOR_H

#include <Service.h>
#include <Configuration.h>

typedef enum {
    OFF,
    ON
} MotorState;

class Motor : Service {
public:
    virtual uint8_t get_speed() const =0;

    virtual MotorState get_state() const =0;

    virtual void reset()=0;

    virtual unsigned long get_evolution() const =0;

    virtual double get_len() const =0;

    virtual void toggle()=0;

    virtual void increase_speed() =0;

    virtual void decrease_speed() =0;

    virtual void increase_stop_evolution() =0;

    virtual void decrease_stop_evolution() =0;

    virtual unsigned long *get_stop_evolution() const =0;

    virtual void increase_stall_timeout() =0;

    virtual void decrease_stall_timeout() =0;

    virtual uint16_t *get_stall_timeout() const =0;

    virtual PROFILE *get_profile() const =0;

    virtual void next_profile() =0;

    virtual void prev_profile() =0;
};


#endif //ARDUINO_PROJECTS_MOTOR_H
