#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "motor_setup.h"
#include "code39_barcode.h"

// GPIO Pin Configurations
#define barcode_IR 5
#define encoderRightPin 6
#define encoderLeftPin 7
#define right_IR 8
#define left_IR 9

typedef enum {
    NORMAL,
    FRONT_BLOCKED,
    RIGHT_EDGE,
    LEFT_EDGE,
} DirectionState;

DirectionState direction = NORMAL;

// Constants
#define DISTANCE_PER_PULSE 1.05  // Per divit in the wheel encoder. 21 cm diameter and 20 divits in the wheel encoder
#define TIMEOUT_INTERVAL_MS 5000 // 10 seconds

// variables for encoder
volatile uint32_t pulse_count = 0;
volatile uint64_t encoder_prev_time = 0;
volatile uint64_t timer_start_time = 0;
volatile bool timer_enabled = false;
int loop_count = 0, ext_count = 0, reserve_count = 0;
bool deadend = false, rightwallhit = false, leftwallhit = false, rightIFflag = false, leftIFflag = false;

void encoderISR() {
    printf("encoderISR triggered");
    if(direction == FRONT_BLOCKED){
        loop_count++;
    }
    else{
        // Increment the pulse count on each encoder pulse
        pulse_count++;

        // Calculate time elapsed since the last pulse
        uint64_t encoder_current_time = time_us_64();
        uint64_t time_elapsed = encoder_current_time - encoder_prev_time;

        // Calculate speed (distance traveled per unit time)
        float speed = DISTANCE_PER_PULSE / (time_elapsed / 1e6);  // Convert time to seconds
        printf("Speed: %.2f cm/second\n", speed);

        // Update the previous time
        encoder_prev_time = encoder_current_time;

        // Enable the timer
        timer_enabled = true;
        timer_start_time = encoder_current_time;
    }
}

void barcodeISR() {
    current_pin_state = gpio_get(barcode_IR);
    if (current_pin_state != previous_interrupt_pin_state) {
        current_time = time_us_32();
        printf("Current Time: %lu  ", current_time);
        switch (barcode_state) {
            case FIRST_STATE:
                printf("FIRST STATE \n");
                start_time = current_time;
                barcode_state = READING_STATE;
                break;

            case READING_STATE:
                printf("READING STATE ");
                duration = current_time - start_time;
                start_time = current_time;
                if(!skip){
                    //printf("Current bit count: %d  Char count: %d  ", bit_count, char_count);
                    if (bit_count == 0 && char_count == 0) {threshold = duration * 1.32;}// First bar, set threshold
                    printf("Threshold: %lu, Duration: %lu  ", threshold, duration);
                    is_thin = (duration < threshold);
                    cumulative_barcode_value = (cumulative_barcode_value << 1) | (is_thin ? 1 : 0);
                    printf("Cumulative_barcode_value: %lu  \n", cumulative_barcode_value);
                    bit_count++;

                    if (bit_count > 8){
                        //printf("Char Counter: %d \n", char_count);
                        printf("Cumulative_barcode_value is binary is: ");
                        printBinary(cumulative_barcode_value);
                        printf("\n");
                        barcode[char_count] = cumulative_barcode_value;
                        printf("barcode[%d] is %lu  \n", char_count, barcode[char_count]);
                        cumulative_barcode_value = 0;
                        if(char_count == 2){
                            barcode_state = DECODING_STATE;
                            printf("\n"); printf("\n"); 
                        }else{
                            char_count++;
                            skip = true;
                        }
                        bit_count = 0;
                    }
                }else{skip = false;}
                break;
            case DECODING_STATE:
                printf("DECODING STATE \n");
                break;
        }
        previous_interrupt_pin_state = current_pin_state;
    }
    gpio_acknowledge_irq (barcode_IR, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL);
}

int main() {

    printf("Initialize standard I/O \n");
    // Initialize standard I/O
    stdio_init_all();
    gpio_init(barcode_IR);
    gpio_init(encoderLeftPin);
    gpio_init(encoderRightPin);
    gpio_init(right_IR);
    gpio_init(left_IR);
    gpio_set_dir(barcode_IR, GPIO_IN);
    gpio_set_dir(encoderLeftPin, GPIO_IN);
    gpio_set_dir(encoderRightPin, GPIO_IN);
    gpio_set_dir(right_IR, GPIO_IN);
    gpio_set_dir(left_IR, GPIO_IN);
    gpio_pull_down(barcode_IR);
    gpio_pull_down(right_IR);
    gpio_pull_down(left_IR);
    // Delay to before moving off
    for(int i = 8; i > 0; i--) {sleep_ms(1000); printf("%d Sec reminding \n", i);}
    current_pin_state = gpio_get(barcode_IR);
    previous_interrupt_pin_state = current_pin_state;
    gpio_set_irq_enabled_with_callback(encoderLeftPin, GPIO_IRQ_EDGE_FALL, true, &encoderISR);
    motor_int();

    // Main control loop
    while(1) {
        if(leftIFflag || rightIFflag){
            if(ext_count > 15){
                leftIFflag = false;
                rightIFflag = false;
                ext_count = 0;
            }
            else{
                ext_count++;
                sleep_us(10);
            }
        }
        printf("States of both left %d and right %d IR sensor, ext_count: %i \n", gpio_get(left_IR),gpio_get(right_IR), ext_count);
        if((gpio_get(right_IR) && gpio_get(left_IR))||(leftIFflag && rightIFflag)){
            direction = FRONT_BLOCKED;
            deadend = true;
            leftIFflag = false;
            rightIFflag = false;
        }
        else if(!gpio_get(right_IR) && gpio_get(left_IR)){
            leftIFflag = true;
            direction = LEFT_EDGE;
            sleep_us(10);
        }
        else if(gpio_get(right_IR) && !gpio_get(left_IR)){
            rightIFflag = true;
            direction = RIGHT_EDGE;
            sleep_us(10);
        }
        else{
            direction = NORMAL;
        }
        if(direction == FRONT_BLOCKED){
            move_stop();
            sleep_ms(30);
            move_backward();
            sleep_ms(350);
            while(deadend){
                spin_right();
                loop_count = 0;
                while(loop_count < 4){
                    if(gpio_get(right_IR)){
                        rightwallhit = true;
                        printf("right wall hit \n");
                    }
                    printf("Loop counter: %i \n", loop_count);
                }
                move_stop();
                sleep_ms(30);
                spin_left();
                loop_count = 0;
                while(loop_count < 8){
                    if(gpio_get(left_IR)){
                        leftwallhit = true;
                        printf("left wall hit \n");
                    }
                    printf("Loop counter: %i \n", loop_count);
                }
                move_stop();
                sleep_ms(30);
                spin_right();
                loop_count = 0;
                while(loop_count <4){
                    printf("Loop counter: %i \n", loop_count);
                }
                move_stop();
                sleep_ms(30);
                move_forward();
                sleep_ms(40);
                move_stop();
                sleep_ms(30);
                if((!leftwallhit && !rightwallhit)||(leftwallhit && !rightwallhit)){ //Right justified or left wall hit
                    gpio_set_irq_enabled_with_callback(encoderLeftPin, GPIO_IRQ_EDGE_FALL, true, &encoderISR);
                    printf("Turning right \n");
                    move_turn_right();
                    loop_count = 0;
                    while(loop_count < 20){
                        printf("Loop counter: %i \n", loop_count);
                    };
                    move_backward();
                    sleep_ms(400);
                    move_stop();
                    sleep_ms(30);
                    deadend = false;
                    loop_count = 0;
                    break;
                }
                else if(!leftwallhit && rightwallhit){ //Left wall hit
                    gpio_set_irq_enabled_with_callback(encoderRightPin, GPIO_IRQ_EDGE_FALL, true, &encoderISR);
                    printf("Turning Left \n");
                    move_turn_left();
                    loop_count = 0;
                    while(loop_count < 20){
                        printf("Loop counter: %i \n", loop_count);
                    };
                    move_backward();
                    sleep_ms(400);
                    move_stop();
                    sleep_ms(30);
                    deadend = false;
                    loop_count = 0;
                    break;
                }
                else if(leftwallhit && rightwallhit){ //both walls hit
                    printf("both walls hit \n");
                    move_backward();
                    loop_count = 0;
                    while(loop_count < 7){ 
                        printf("Loop counter: %i \n", loop_count);
                    };
                    move_stop();
                    sleep_ms(30);
                    loop_count = 0;
                    
                    rightwallhit = false;
                    leftwallhit = false;
                }
            }
            gpio_set_irq_enabled_with_callback(encoderLeftPin, GPIO_IRQ_EDGE_FALL, true, &encoderISR);
            ext_count = 0;
            loop_count = 0;
            reserve_count = 0;
            direction = NORMAL;
            rightwallhit = false;
            leftwallhit = false;
        }
        else if(direction == NORMAL){
            move_forward();
        }
        else if(direction == RIGHT_EDGE){
            move_skeew_right();
        }
        else if(direction == LEFT_EDGE){
            move_skeew_left();
        }

        if(barcode_reading == true){
            printf("Entered barcode_reading == true \n");
            move_backward();
            sleep_ms(700);
            move_forward_barcode();
            gpio_set_irq_enabled_with_callback(barcode_IR, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &barcodeISR);
            while(barcode_reading == true){
                if (barcode_state == DECODING_STATE){
                    gpio_set_irq_enabled_with_callback(barcode_IR, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, false, &barcodeISR);
                    printf("DECODING STATE \n");
                    move_stop();
                    decodeBarcode();

                    delimiter = hidden_char[0];
                    for(int x = 0; x < char_count; x++){
                        if(delimiter != hidden_char[x]){
                            decoded_char = hidden_char[x];
                            printf("Decoded Char is %c", decoded_char);
                        }
                    }
                    barcode_state = FIRST_STATE;
                    barcode_reading = false;
                }
            }
        }

        if (timer_enabled) {
            uint64_t encoder_current_time = time_us_64();
            uint64_t time_elapsed = encoder_current_time - timer_start_time;

            // If 10 seconds have passed without an interrupt, disable the counter
            if (time_elapsed >= TIMEOUT_INTERVAL_MS * 1000) {
                float total_distance = pulse_count * DISTANCE_PER_PULSE;
                printf("Total Distance: %.2f cm\n", total_distance);

                // Reset pulse count and disable the timer
                pulse_count = 0;
                timer_enabled = false;
            }
        }
        sleep_us(10);
    }
}
