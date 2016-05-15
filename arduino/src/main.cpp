#include <WProgram.h>
#include <inttypes.h>
#include <OneWire.h>

#define MS_PER_HOUR    3.6e6

int cycles_per_kwh = 100;
int max_watt = 6000;
unsigned long previous = 0;
unsigned long cycle = 0;
boolean last = LOW;

int cycles_per_m3 = 100;
int max_m3 = 6000;
unsigned long gasprevious = 0;
unsigned long gascycle = 0;
boolean gaslast = LOW;
boolean gaspin = LOW;

int readgas = 0;

OneWire  ds(10);

void kwhChange() {
    unsigned long now = millis();
    unsigned long time = now - previous;

    if (cycle == 0) {
        cycle++;
        return;
    }

    digitalWrite(13,last);

    previous = now; 

    double W = 1000 * ((double) MS_PER_HOUR / time) / cycles_per_kwh;
    Serial.print("Cycle ");
    Serial.print(cycle, DEC);
    Serial.print(": ");
    Serial.print(time, DEC);
    Serial.print(" ms, ");
    Serial.print(W, 2);
    Serial.println(" W");
    cycle++;
    if (last) last = LOW;
    else last = HIGH;
}

void gasChange() {
    Serial.print(gascycle);
    unsigned long gasnow = millis();
    unsigned long gastime = gasnow - gasprevious;

    if (gascycle == 0) {
        gascycle++;
        return;
    }

    gasprevious = gasnow;

    double M3 = 1000 * ((double) MS_PER_HOUR / gastime) / cycles_per_m3;
    Serial.print("Gas ");
    Serial.print(gascycle, DEC);
    Serial.print(": ");
    Serial.print(gastime, DEC);
    Serial.print(" ms, ");
    Serial.print(M3, 2);
    Serial.println(" l/h");
    gascycle++;
    if (gaslast) gaslast = LOW;
    else gaslast = HIGH;
}

void setup () {
  Serial.begin(57600);
  pinMode(13, OUTPUT);
  pinMode(3,INPUT);
  digitalWrite(3, HIGH);
  attachInterrupt(0, kwhChange, FALLING);
//  attachInterrupt(1, gasChange, RISING);
//  interrupts();
  Serial.println("Bitlair usage monitor starting");
}

void loop () {
  readgas = digitalRead(3);
  if (readgas == 1 && gaspin == LOW) {
    gaspin = HIGH;
    gasChange();
  }
  if (readgas == 0 && gaspin == HIGH) {
    gaspin = LOW;
  }
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius; 
  
  if ( !ds.search(addr)) {
    ds.reset_search();
    delay(250);
    return;
  }

  for( i = 0; i < 8; i++) {
    Serial.print(addr[i], HEX);
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
      return;
  }
  switch (addr[0]) {
    case 0x10:
      //Serial.println("  Chip = DS18S20");  // or old DS1820
      type_s = 1;
      break;
    case 0x28:
      //Serial.println("  Chip = DS18B20");
      type_s = 0;
      break;
    case 0x22:
      //Serial.println("  Chip = DS1822");
      type_s = 0;
      break;
    default:
      //Serial.println("Device is not a DS18x20 family device.");
      return;
  } 
  ds.reset();
  ds.select(addr);
  ds.write(0x44,1);
  delay(1500);
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad


  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
  }

  unsigned int raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // count remain gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    if (cfg == 0x00) raw = raw << 3;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw << 2; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw << 1; // 11 bit res, 375 ms
    // default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;
  Serial.print(' ');
  Serial.print(celsius);
  Serial.println();
    
}

