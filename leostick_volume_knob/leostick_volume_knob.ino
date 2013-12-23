// a little sketch for adjusting a computer's volume level using a clicky knob
// by @Bluebie on GitHub - MIT License
// Prerequisites: install the USB HID modifications created by Stefan Jones:
// http://stefanjones.ca/blog/arduino-leonardo-remote-multimedia-keys/

#define BUTTON_PIN 5
#define KNOB_QUAD_LEFT 2
#define KNOB_QUAD_RIGHT 3

void setup() {
  // set encoder and button pins to be inputs
  pinMode(BUTTON_PIN, INPUT);
  pinMode(KNOB_QUAD_LEFT, INPUT);
  pinMode(KNOB_QUAD_RIGHT, INPUT);
  
  // turn on pullup resistors
  digitalWrite(BUTTON_PIN, HIGH);
  digitalWrite(KNOB_QUAD_LEFT, HIGH);
  digitalWrite(KNOB_QUAD_RIGHT, HIGH);
  
 
}

// debounce a pin, used for mute button
// increase DEBOUNCE_DURATION_US if sometimes pressing mute button
// does the action too many times.
#define DEBOUNCE_DURATION_US 1500 /* 0.5 milliseconds */
void debounce(byte pin, boolean state) {
  unsigned long time_now = micros();
  unsigned long time_since_wrong = time_now;
  
  while (time_since_wrong + DEBOUNCE_DURATION_US > time_now) {
    time_now = micros();
    
    if (digitalRead(pin) != state) {
      time_since_wrong = time_now;
    }
  }
}

void loop() {
  // check if button is pressed
  if (digitalRead(BUTTON_PIN) == LOW) {
    button_pressed();
    debounce(BUTTON_PIN, LOW); // wait till button has settled as being low
    debounce(BUTTON_PIN, HIGH); // wait till button has settled on being high
  }
  
  alter_volume(read_knob());
  Remote.clear();
}

void alter_volume(int amount) {
  while (amount) {
    if (amount > 0) {
      Remote.increase();
      amount -= 1;
    } else if (amount < 0) {
      Remote.decrease();
      amount += 1;
    }
  }
}

void button_pressed() {
  
  Remote.mute();
}

// returns the bits from the two knob wires as a 2-bit number
byte knob_bits() {
  return digitalRead(KNOB_QUAD_LEFT) << 1 | digitalRead(KNOB_QUAD_RIGHT);
}

// reads a simple incremental quad encoder knob, returns -1, 0, or +1 indicating if the knob has rotated
// to the left or right by one click. Call this function as often as possible so it handles knobs being turned
// very quickly okay
char read_knob() {
  static byte previous_state; // these two variables keep their values between function calls
  static byte armed;
  byte state = knob_bits();
  char result = 0;
  
  // to ensure bouncy mechanical switch stuff only makes one positive or negative impulse for each mechanical click
  // we "arm" it when it gets half way through a click
  if (state == 0b00) {
    armed = true; // allow one click result
  }
  
  // then when it finishes a click, we consume the one "armed" token and deliver the result just once to the caller
  if (armed == true && state == 0b11) {
    armed = false; // consumed.
    if (previous_state == 0b01) {
      result = -1;
    } else if (previous_state == 0b10) {
      result = +1;
    }
  }
   
  previous_state = state;
  return result;
}



