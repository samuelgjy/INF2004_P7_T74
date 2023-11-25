#pragma once

#include <stdio.h>
#include "common.h"


// Motor Controller code to be remvoed after driver end
// 
// #define CLK_CYCLE_NO 62500  

// #define LEFT_MOTOR 21
// #define L_REVERSE_PIN 2
// #define L_CLOCKWISE_PIN 3

// #define RIGHT_MOTOR 20
// #define R_REVERSE_PIN 10
// #define R_CLOCKWISE_PIN 11


void initWheelEncoder();

float get_dst(float start_time, float prev_time,float dir_triggered);