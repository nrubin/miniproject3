#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "usb.h"
#include "pin.h"
#include "uart.h"
#include "oc.h"
#include "ui.h"
#include "timer.h"
#include <stdio.h>

#define HELLO       0   // Vendor request that prints "Hello World!"
#define SET_VALS    1   // Vendor request that receives 2 unsigned integer values
#define GET_VALS    2   // Vendor request that returns 2 unsigned integer values
#define PRINT_VALS  3   // Vendor request that prints 2 unsigned integer values 

const float PIN_CONVERSION_FACTOR = 65472;
const float MAX_VOLTAGE = 3;
const float MIN_WIDTH = 5.5E-4;
const float MAX_WIDTH = 2.3E-3;
const float FREQ = 40e3;
const float PEAK_DETECT_DIFF = -0.5;


float duty_percentage;
float interval;
float prev_signal_value;
float current_signal_value;

uint16_t pos;
uint16_t val1, val2;
uint16_t duty;

void VendorRequests(void) {
    WORD temp;

    switch (USB_setup.bRequest) {
        case HELLO:
            printf("Hello World!\n");
            BD[EP0IN].bytecount = 0;    // set EP0 IN byte count to 0 
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        case SET_VALS:
            val1 = USB_setup.wValue.w;
            val2 = USB_setup.wIndex.w;
            BD[EP0IN].bytecount = 0;    // set EP0 IN byte count to 0 
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        case GET_VALS:
            temp.w = val1;
            BD[EP0IN].address[0] = temp.b[0];
            BD[EP0IN].address[1] = temp.b[1];
            temp.w = val2;
            BD[EP0IN].address[2] = temp.b[0];
            BD[EP0IN].address[3] = temp.b[1];
            BD[EP0IN].bytecount = 4;    // set EP0 IN byte count to 4
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;            
        case PRINT_VALS:
            printf("val1 = %u, val2 = %u\n", val1, val2);
            BD[EP0IN].bytecount = 0;    // set EP0 IN byte count to 0
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        default:
            USB_error_flags |= 0x01;    // set Request Error Flag
    }
}

void VendorRequestsIn(void) {
    switch (USB_request.setup.bRequest) {
        default:
            USB_error_flags |= 0x01;                    // set Request Error Flag
    }
}

void VendorRequestsOut(void) {
    switch (USB_request.setup.bRequest) {
        default:
            USB_error_flags |= 0x01;                    // set Request Error Flag
    }
}

uint16_t get_duty(float duty_percentage){
    return (uint16_t)(65536 * duty_percentage);
}

float pin_read_to_actual_voltage(uint16_t pin_value){
    // 0b1111111111000000 = 65,472 ~ 3 V
    // 0b0000000000000000 = 0 ~ 0 V
    float pin_value_float = (float)pin_value;
    float ratio = pin_value_float / PIN_CONVERSION_FACTOR;
    float actual_voltage = MAX_VOLTAGE * ratio;
    return actual_voltage;
}

int16_t main(void) {
    init_pin();
    init_clock();
    init_uart();
    init_ui();
    init_timer();
    init_oc();

    //setup the signal input pin
    pin_analogIn(&A[3]);

    prev_signal_value = 3;
    current_signal_value = 3;
    
    led_on(&led2);
    timer_setPeriod(&timer2, 0.2);
    timer_start(&timer2);
    timer_setPeriod(&timer5,0.001);
    timer_start(&timer5);
    val1 = 0;
    val2 = 0;
    uint16_t send_pulse = 0; // sending boolean

    interval = 0.02;

    pos = 0; //16 bit int with binary point in front of the MSB
    duty_percentage = 0;
    duty = get_duty(duty_percentage);

    // oc_servo(&oc1,&D[0],&timer1, interval,MIN_WIDTH, MAX_WIDTH, pos);
    // oc_servo(&oc2,&D[2],&timer3, interval,MIN_WIDTH, MAX_WIDTH, pos);
    oc_pwm(&oc3,&D[3],NULL,FREQ,duty);

    printf("Good morning\n");

    InitUSB();                              // initialize the USB registers and serial interface engine
    while (USB_USWSTAT!=CONFIG_STATE) {     // while the peripheral is not configured...
        ServiceUSB();                       // ...service USB requests
    }
    while (1) {
        ServiceUSB();
        current_signal_value = pin_read_to_actual_voltage(pin_read(&A[3]));
        if (current_signal_value - prev_signal_value < PEAK_DETECT_DIFF)
        {
            printf("Peak detected!\n");
        }
        // printf("%d\n", current_signal_value);
        prev_signal_value = current_signal_value;
        //write the values to the servos (move the servos to the requested position)
        // pin_write(&D[0],val1);
        // pin_write(&D[2],val2);

        if (timer_flag(&timer5)) {
            timer_lower(&timer5);
            if (send_pulse)
            {
                pin_write(&D[3],get_duty(0.5));
            } else {
                pin_write(&D[3],get_duty(0));
            }
            send_pulse = 0;
            // printf("val1 = %u, val2 = %u\n", val1, val2);
        }
        if (timer_flag(&timer2)) {
            timer_lower(&timer2);
            led_toggle(&led1);
            send_pulse = !send_pulse;
            // printf("val1 = %u, val2 = %u\n", val1, val2);
        }
    }
}

