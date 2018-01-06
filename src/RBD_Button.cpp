// Arduino RBD Button Library v2.1.1 - Read and debounce buttons and switches.
// https://github.com/alextaujenis/RBD_Button
// Copyright 2016 Alex Taujenis
// MIT License

#include <Arduino.h>
#include <RBD_Timer.h>  // https://github.com/alextaujenis/RBD_Timer
#include <RBD_Button.h> // https://github.com/alextaujenis/RBD_Button

namespace RBD {
  // input pullup enabled by default
  Button::Button(uint8_t pin)
  : _pressed_debounce(), _released_debounce() {
    _pin = pin;
    _inputPullup();
    setDebounceTimeout(debounce_timeout);
  }

  // overloaded constructor to disable input pullup
  Button::Button(uint8_t pin, bool input_pullup)
  : _pressed_debounce(), _released_debounce() {
    _pin = pin;
    if(input_pullup) {_inputPullup();}
    else {_disableInputPullup();}
    setDebounceTimeout(debounce_timeout);
  }

  void Button::setDebounceTimeout(unsigned long value) {
    _pressed_debounce.setTimeout(value);
    _released_debounce.setTimeout(value);
  }

  bool Button::isPressed() {
    _temp_reading = digitalRead(_pin);
    if(_invert) {return !_temp_reading;}
    else {return _temp_reading;}
  }

  bool Button::isReleased() {
    return !isPressed();
  }

  bool Button::onPressed() {
    if(isPressed()) {
      _pressed_debounce.restart();
      if(!_has_been_pressed) {
        return _has_been_pressed = true;
      }
      return false;
    }
    // else
    if(_pressed_debounce.isExpired()) {
      return _has_been_pressed = false;
    }
    else {
      return false;
    }
  }

  bool Button::onReleased() {
    if(isReleased()) {
      _released_debounce.restart();
      if(!_has_been_released) {
        return _has_been_released = true;
      }
      return false;
    }
    // else isPressed()
    if(_released_debounce.isExpired()) {
      return _has_been_released = false;
    }
    else {
      return false;
    }
  }
  
  bool Button::onClicked() {
	_processClick();  
	if(_has_been_clicked) {
		_has_been_clicked = false;
		return true;
	}
	return false;	  
  }

  bool Button::onDoubleClicked() {
	_processClick();  
	if(_has_been_double_clicked) {
		_has_been_double_clicked = false;
		return true;
	}
	return false;	  
  }

  bool Button::onLongClicked() {
	_processClick();  
	if(_has_been_long_clicked) {
		_has_been_long_clicked = false;
		return true;
	}
	return false;	  
  }

  void Button::invertReading() {
    _invert = !_invert;
  }
  
  // private

  void Button::_inputPullup() {
    pinMode(_pin, INPUT_PULLUP);
  }

  void Button::_disableInputPullup() {
    pinMode(_pin, INPUT);
  }
  
  void Button::_processClick() {
	bool isUnknown = false;  
	if(onPressed()) {
		switch(_click_state) {
			case 0:
				_click_timer.setTimeout(longclick_time);
				_click_timer.restart();
				_click_state = 1;
				break;
			case 2:
				if(_click_timer.isExpired()) {
					_has_been_clicked = true;
					_click_timer.setTimeout(longclick_time);
					_click_timer.restart();
					_click_state = 1;
				} else {
					_click_timer.setTimeout(longclick_time);
					_click_timer.restart();
					_click_state = 3;
				}
				break;
			default:
				isUnknown = true;
		}
	} else if(onReleased()) {
		switch(_click_state) {
			case 1:
				if(_click_timer.isActive()) {
					_click_timer.setTimeout(doubleclick_gap);
					_click_timer.restart();
					_click_state = 2;
				}
				break;
			case 3:
				if(_click_timer.isActive()) {
					_has_been_double_clicked = true;
					_click_timer.stop();
					_click_state = 0;
				} else {
					_has_been_clicked = true;
					_has_been_long_clicked = true;
					_click_state = 0;
				}
				break;
			case 4:
				_click_state = 0;
				break;
			default:
				isUnknown = true;
		}
	} else {
		switch(_click_state) {
			case 1:
				if(_click_timer.isExpired()) {
					_has_been_long_clicked = true;
					_click_state = 4;
				}
				break;
			case 2:
				if(_click_timer.isExpired()) {
					_has_been_clicked = true;
					_click_state = 0;
				}
				break;
			case 3:
				if(_click_timer.isExpired()) {
					_has_been_clicked = true;
					_has_been_long_clicked = true;
					_click_state = 4;
				}				
				break;
		}
	}
	if(isUnknown) {
		_click_timer.stop();
		_click_state = 0;
	}
  }
}