#include "motor_app.h"
uint8_t test;


void Motor_Task(void)
{
    if (test < 15) {
      Stepper_Set_Speed(&MotorX, 10 * test);
      Stepper_Set_Speed(&MotorY, 10 * test);
    }
    test++;
    if (test >= 15) {
      Stepper_Set_Speed(&MotorX,0);
      Stepper_Set_Speed(&MotorY,0);
    }
}