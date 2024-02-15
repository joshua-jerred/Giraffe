/**
 * @file SA868.ino
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Testing code for the SA868 VHF radio module using the Pi Pico. This
 * will not be used in the final project, just testing the hardware!
 *
 * @date 2023-02-03
 * @copyright Copyright (c) 2023
 */

#include <Arduino.h>

#define PWR_PIN 21
#define PTT_PIN 20
#define SQ_PIN 19

#define RX_LED 11
#define TX_LED 12

enum class Ack {ACK, NACK};

const String MAX_TX_FREQ = "145.8000";
const String MIN_TX_FREQ = "145.5000";

const float MAX_FREQ = 174.0000; // per datasheet
const float MIN_FREQ = 134.0000; // per datasheet

struct SA868 {
  bool powered = false; // true = on, false = off
  String tx_power = "1"; // 0 = high, 1 = low

  /*
    Range for the VHF Module: （134.0000M～174.0000M).
    Default is inside the experimental portion of the 2m band.
    This will currently block and transmission outside of the
    experimental part of the band.

    The current_freq will be set to both tx and rx freqs
  */
  String current_freq = "145.5100";
  String tx_cxcss = "0000"; // Disabled
  String rx_cxcss = "0000"; // Disabled

  String squelch = "4"; // 0~8, 0 is monitor mode
  String volume = "8"; // 1~8, 8 is max

  // Filters (0 = on, 1 = off)
  String pre_emphasis = "1";
  String de_emphasis = "1";
  String low_pass = "1";
};

SA868 radio;

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);
  while (!Serial) { ; }
  Serial.println("Starting");

  pinMode(PWR_PIN, OUTPUT);
  pinMode(PTT_PIN, OUTPUT);
  pinMode(SQ_PIN, INPUT_PULLUP);

  pinMode(RX_LED, OUTPUT);
  pinMode(TX_LED, OUTPUT);
}

void loop() {
  if (digitalRead(SQ_PIN)) {
    digitalWrite(RX_LED, HIGH);
  } else {
    digitalWrite(RX_LED, LOW);
  }

  if (Serial.available()) {
    String in_str = Serial.readString();

    if (in_str == "tx-on") {
      digitalWrite(TX_LED, HIGH);
      digitalWrite(PTT_PIN, HIGH);
      return;
    } else if (in_str == "tx-off") {
      digitalWrite(TX_LED, LOW);
      digitalWrite(PTT_PIN, LOW);
      return;
    }

    if (in_str == "test") {       // Handshake with the pico
      Serial.println("test received");
    } else if (in_str == "on") {  // Power up the radio
      if (powerOn() == Ack::ACK) {
        delay(100);
        Serial.println("ACK");
      }
    } else if (in_str == "off") { // Power down the radio
      powerOff();
      Serial.println("ACK");
    } else {

      if (!radio.powered) {
        Serial.println("Radio not powered on");
      } else if (in_str == "handshake") {
        if (handshake() == Ack::ACK) {
          Serial.println("ACK");
        }

      } else if (in_str.startsWith("set-freq")) {

        if (in_str.length() < 17 || in_str.charAt(12) != '.') {
          Serial.println("Invalid format : [set-freq xxx.xxxx]");
          return;
        }
        String new_freq = in_str.substring(9, 17);
        float new_freq_float = new_freq.toFloat();

        if (new_freq_float < MIN_FREQ || new_freq_float > MAX_FREQ) { // Verify that it's in the supported range
          Serial.print(new_freq_float);
          Serial.println(" freq out of range");
          return;
        }

        radio.current_freq = new_freq; // does not check for 12.5k/25k steps
        if (setGroup() == Ack::ACK) {
          Serial.println("ACK");
        }
      } else if (in_str.startsWith("set-volume")) {

        if (in_str.length() < 12) {
          Serial.println("Invalid format : [set-volume x] 1-8");
          return;
        }
        String new_volume = in_str.substring(11, 12);
        int new_volume_int = new_volume.toInt();
        if (new_volume_int < 1 || new_volume_int > 8) {
          Serial.print(new_volume);
          Serial.println(" : Invalid volume (1-8)");
          return;
        }
        radio.volume = new_volume;
        if (setVolume() == Ack::ACK) {
          Serial.println("ACK");
          return;
        }

      } else if (in_str.startsWith("set-filter")) { // format: set-filters 0 0 0 (0 = on, 1 = off)
        if (in_str.length() < 16) {
          Serial.println("Invalid format : [set-filter x x x] 0 on, 1 off");
          return;
        }
        char pre_emph = in_str.charAt(11);
        if (pre_emph != '0' && pre_emph != '1') {
          Serial.println("Bad pre-empisis filter");
          return;
        }
        char de_emph = in_str.charAt(13);
        if (de_emph != '0' && de_emph != '1') {
          Serial.println("Bad de-empisis filter");
          return;
        }
        char low_pass = in_str.charAt(15);
        if (low_pass != '0' && low_pass != '1') {
          Serial.println("Bad low-pass filter");
          return;
        }

        radio.pre_emphasis = pre_emph;
        radio.de_emphasis = de_emph;
        radio.low_pass = low_pass;

        if (setFilter() == Ack::ACK) {
          Serial.println("ACK");
          return;
        }
      } else if (in_str.startsWith("set-sq")) {

        if (in_str.length() < 8) {
          Serial.println("Invalid format : [set-sq x] 0-8");
          return;
        }
        String new_sq = in_str.substring(7, 8);
        int new_sq_int = new_sq.toInt();
        if (new_sq_int < 0 || new_sq_int > 8) {
          Serial.print(new_sq_int);
          Serial.println(" : Invalid squelch (0-8)");
          return;
        }
        radio.squelch = new_sq;
        if (setGroup() == Ack::ACK) {
          Serial.println("ACK");
          return;
        }
      } else if (in_str == "rssi") {
        Serial.println(rssi());
      } else {
        Serial.println("Unknown command " + in_str);
      }
    }
  }

    //Serial2.write(Serial.read());
  //if (Serial2.available()) {
  //  Serial.write(Serial2.read());
  //}
}

Ack powerOn() {
  digitalWrite(PWR_PIN, HIGH);
  radio.powered = true;
  return handshake();
}

void powerOff() {
  digitalWrite(PWR_PIN, LOW);
  radio.powered = false;
}

Ack handshake() {
  Serial2.write("AT+DMOCONNECT\r\n");
  String response = readRadio();
  if (response == "+DMOCONNECT:0\r\n") {
    return Ack::ACK;
  } else {
    Serial.println("Handshake failed: " + response);
    return Ack::NACK;
  }
}

Ack setGroup() { // Not yet implemented
  String command = "AT+DMOSETGROUP="
    + radio.tx_power + ","
    + radio.current_freq + ","
    + radio.current_freq + ","
    + radio.tx_cxcss + ","
    + radio.squelch + ","
    + radio.rx_cxcss + "\r\n";

  Serial2.write(command.c_str());
  String response = readRadio();

  if (response == "+DMOSETGROUP:0\r\n") {
    return Ack::ACK;
  } else {
    Serial.println(response);
    return Ack::NACK;
  }
}

Ack setFilter() {
  String command = "AT+SETFILTER="
    + radio.pre_emphasis + ","
    + radio.de_emphasis + ","
    + radio.low_pass + "\r\n";

  Serial2.write(command.c_str());
  String response = readRadio();
  if (response == "+DMOSETFILTER:0\r\n") {
    return Ack::ACK;
  } else {
    Serial.println(response);
    return Ack::NACK;
  }
}

Ack setVolume() {
  String command = "AT+DMOSETVOLUME="
    + radio.volume + "\r\n";

  Serial2.write(command.c_str());
  String response = readRadio();

  if (response == "+DMOSETVOLUME:0\r\n") {
    return Ack::ACK;
  } else {
    Serial.println(response + " " + command);
    return Ack::NACK;
  }
}

String rssi() {
  String command = "AT+RSSI?\r\n";
  Serial2.write(command.c_str());
  String response = readRadio();
  return response.substring(5, 8);
}

String readRadio() {
  return Serial2.readString();
}

