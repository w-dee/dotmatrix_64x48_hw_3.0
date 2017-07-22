#ifndef BUTTONS_H
#define BUTTONS_H

#include <Arduino.h>

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


/**
 * Get button state in bitmap format.
 * button buffer is to be cleared.
 */
uint32_t button_get();

#define ORD_BUTTON_LEFT   0
#define ORD_BUTTON_UP     1
#define ORD_BUTTON_DOWN   2
#define ORD_BUTTON_RIGHT  3
#define ORD_BUTTON_OK     4
#define ORD_BUTTON_CANCEL 5

#define BUTTON_LEFT      (1<<ORD_BUTTON_LEFT)
#define BUTTON_UP        (1<<ORD_BUTTON_UP)
#define BUTTON_DOWN      (1<<ORD_BUTTON_DOWN)
#define BUTTON_RIGHT     (1<<ORD_BUTTON_RIGHT)
#define BUTTON_OK        (1<<ORD_BUTTON_OK)
#define BUTTON_CANCEL    (1<<ORD_BUTTON_CANCEL)

void button_push(uint32_t button);

#endif

