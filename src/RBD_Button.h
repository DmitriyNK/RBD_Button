// Arduino RBD Button Library v2.1.1 - Read and debounce buttons and switches.
// https://github.com/alextaujenis/RBD_Button
// Copyright 2016 Alex Taujenis
// MIT License

#ifndef RBD_BUTTON_H
#define RBD_BUTTON_H

#include <Arduino.h>
#include <RBD_Timer.h>  // https://github.com/alextaujenis/RBD_Timer

namespace RBD {
  #define doubleclick_gap 250 // max ms between clicks for a double click event
  #define longclick_time 1200 // ms hold period: how long to wait for press+hold event
  #define debounce_timeout 10 // ms
	
  class Button {
    public:
      Button(uint8_t pin);                    // constructor: input pullup enabled by default
      Button(uint8_t pin, bool input_pullup); // overloaded constructor: flag available to disable input pullup
      void setDebounceTimeout(unsigned long value);
      bool isPressed();
      bool isReleased();
      bool onPressed();
      bool onReleased();
      void invertReading();
	  bool onClicked();
	  bool onDoubleClicked();
	  bool onLongClicked();
    private:
      uint8_t _pin;
      bool _has_been_pressed  = false;
      bool _has_been_released = false;
      bool _invert = true;
      bool _temp_reading;
      void _inputPullup();
      void _disableInputPullup();
	  void _processClick();
      Timer _pressed_debounce;
      Timer _released_debounce;
	  
	  Timer _click_timer;
	  uint8_t _click_state = 0;
	  bool _has_been_clicked = false;
	  bool _has_been_long_clicked = false;
	  bool _has_been_double_clicked = false;
  };
}
#endif