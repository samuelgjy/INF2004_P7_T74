#pragma once

#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include "common.h"

/**
 * @brief Determines the wheel based on the change in angle.
 * 
 * @param initial_angle The initial angle.
 * @param curr_angle The current angle.
 * @return char Returns 'L' for left wheel or 'R' for right wheel.
 */
char determine_wheel(float initial_angle, float curr_angle);

/**
 * @brief Computes the PID response for speed control.
 * 
 * @param target_spd The target speed.
 * @param curr_spd The current speed.
 * @param integral Pointer to the integral term.
 * @param prev_error Pointer to the previous error term.
 * @return float The computed PID response.
 */
float compute_pid(float target_spd, float curr_spd, float *integral, float *prev_error);

