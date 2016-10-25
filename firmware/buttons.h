#ifndef BUTTONS_H
#define BUTTONS_H

#define MAX_BUTTONS 24
/**
 * Button pushed count.
 * Put zero to reset count.
 */
extern uint8_t buttons[MAX_BUTTONS];

/**
 * Call this in main loop.
 */
void button_update();


#define BUTTON_LEFT   16
#define BUTTON_UP     17
#define BUTTON_DOWN   18
#define BUTTON_RIGHT  19
#define BUTTON_OK     20
#define BUTTON_CANCEL 21

#endif

