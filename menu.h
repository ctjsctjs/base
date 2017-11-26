#ifndef PORTS_H_
#define PORTS_H_

// *************************************************************************************************
// Defines section

// Port, pins and interrupt resources for buttons
#define BUTTONS_IN              (P2IN)
#define BUTTONS_OUT             (P2OUT)
#define BUTTONS_DIR             (P2DIR)
#define BUTTONS_REN             (P2REN)
#define BUTTONS_IE              (P2IE)
#define BUTTONS_IES             (P2IES)
#define BUTTONS_IFG             (P2IFG)
#define BUTTONS_IRQ_VECT2       (PORT2_VECTOR)

// Button ports
#define BUTTON_DOWN_PIN         (BIT0) // 0000 0000
#define BUTTON_NUM_PIN          (BIT1) // 0000 0010
#define BUTTON_STAR_PIN         (BIT2) // 0000 0100
#define BUTTON_BACKLIGHT_PIN    (BIT3) // 0000 1000
#define BUTTON_UP_PIN           (BIT4) // 0001 0000
#define ALL_BUTTONS             (BUTTON_STAR_PIN + BUTTON_NUM_PIN + BUTTON_UP_PIN + BUTTON_DOWN_PIN + BUTTON_BACKLIGHT_PIN)

// Macros for button press detection
#define BUTTON_STAR_IS_PRESSED          ((BUTTONS_IN & BUTTON_STAR_PIN) == BUTTON_STAR_PIN)
#define BUTTON_NUM_IS_PRESSED           ((BUTTONS_IN & BUTTON_NUM_PIN) == BUTTON_NUM_PIN)
#define BUTTON_UP_IS_PRESSED            ((BUTTONS_IN & BUTTON_UP_PIN) == BUTTON_UP_PIN)
#define BUTTON_DOWN_IS_PRESSED          ((BUTTONS_IN & BUTTON_DOWN_PIN) == BUTTON_DOWN_PIN)
#define BUTTON_BACKLIGHT_IS_PRESSED     ((BUTTONS_IN & BUTTON_BACKLIGHT_PIN) == \
                                         BUTTON_BACKLIGHT_PIN)
#define NO_BUTTON_IS_PRESSED            ((BUTTONS_IN & ALL_BUTTONS) == 0)

// Macros for button release detection
#define BUTTON_STAR_IS_RELEASED                 ((BUTTONS_IN & BUTTON_STAR_PIN) == 0)
#define BUTTON_NUM_IS_RELEASED                  ((BUTTONS_IN & BUTTON_NUM_PIN) == 0)
#define BUTTON_UP_IS_RELEASED                   (BUTTONS_IN & BUTTON_UP_PIN) == 0)
#define BUTTON_DOWN_IS_RELEASED                 ((BUTTONS_IN & BUTTON_DOWN_PIN) == 0)
#define BUTTON_BACKLIGHT_IS_RELEASED    ((BUTTONS_IN & BUTTON_BACKLIGHT_PIN) == 0)

// Button debounce time (msec)
//#define BUTTONS_DEBOUNCE_TIME_IN        (5u)
//#define BUTTONS_DEBOUNCE_TIME_OUT       (250u)
//#define BUTTONS_DEBOUNCE_TIME_LEFT      (50u)

// Detect if STAR / NUM button is held low continuously
//#define LEFT_BUTTON_LONG_TIME           (2u)

// Backlight time  (sec)
//#define BACKLIGHT_TIME_ON                       (3u)

// Leave set_value() function after some seconds of user inactivity
//#define INACTIVITY_TIME                 (30u)
// Set of button flags
/*
typedef union
{
    struct
    {
        // Manual button events
        int star : 1;           // Short STAR button press
        int num : 1;            // Short NUM button press
        int up : 1;             // Short UP button press
        int down : 1;           // Short DOWN button press
        int backlight : 1;      // Short BACKLIGHT button press
        int star_long : 1;      // Long STAR button press
        int num_long : 1;       // Long NUM button press
        int star_not_long : 1;  // Between short and long STAR button press
        int num_not_long : 1;   // Between short and long NUM button press
    } flag;
    int all_flags;              // Shortcut to all display flags (for reset)
} s_button_flags;
extern volatile s_button_flags button;

struct struct_button
{
    int star_timeout;            // this variable is incremented each second if STAR button is still
                                // pressed
    int num_timeout;             // this variable is incremented each second if NUM button is still
                                // pressed
    int backlight_timeout;       // controls the timeout for the backlight
    int backlight_status;        // 1 case backlight is on
    int repeats;
};
extern volatile struct struct_button sButton;

// *************************************************************************************************
// Extern section
extern void button_repeat_on(int msec);
extern void button_repeat_off(void);
extern void button_repeat_function(void);
extern void init_buttons(void);
*/
#endif                          /*PORTS_H_ */

