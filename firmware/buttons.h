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


#define BUTTON_LEFT   0
#define BUTTON_UP     1
#define BUTTON_DOWN   2
#define BUTTON_RIGHT  3
#define BUTTON_OK     4
#define BUTTON_CANCEL 5

#endif

