// ui_types.h
#ifndef UI_TYPES_H
#define UI_TYPES_H

typedef enum {
    BUTTON_EVENT_NONE,
    BUTTON_EVENT_UP,
    BUTTON_EVENT_DOWN,
    BUTTON_EVENT_SELECT,
    BUTTON_EVENT_BACK
} ButtonEventType;

typedef enum {
    MAIN_OPTION_PLAY,
    MAIN_OPTION_PAUSE,
    MAIN_OPTION_SETTINGS
} MainOption;

typedef enum {
    COLOR_OPTION_BLACK,
    COLOR_OPTION_WHITE,
    COLOR_OPTION_BLUE,
    // Add other color options
} ColorOption;

#endif // UI_TYPES_H