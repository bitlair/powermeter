#include <WProgram.h>



#include <inttypes.h>

#define READINGS       250
#define MS_PER_HOUR    3.6e6

int lower_threshold = 101;
int upper_threshold = 255;
int cycles_per_kwh = 100;
int max_watt = 6000;

unsigned long debounce_time;

void calc_debounce() {
  debounce_time = (1000 * ((double) MS_PER_HOUR / ((long) cycles_per_kwh * max_watt)));
  Serial.print("Debounce time (ms): ");
  Serial.println(debounce_time);
}



void setup () {
  Serial.begin(57600);
  pinMode(A1, INPUT);
  pinMode(13, OUTPUT);
  pinMode(2, INPUT);
  digitalWrite(2, HIGH);
}

boolean ledstate = LOW;
unsigned long cycle = 0;
unsigned long previous = 0; // timestamp

unsigned short readings[READINGS];
unsigned short cursor = 0;
boolean gotenough = false;

unsigned short hits = 0;

unsigned long restore_time = 0;
boolean settingschanged = false;
unsigned long key_debounce = 0;
  
void loop () {
  delay(10);
  

  unsigned short sum = 0;
  for (byte i = 0; i < 40; i++) {
    sum += analogRead(1);
  }

  unsigned long bigsum = 0;
  for (unsigned short i = 0; i < READINGS; i++) bigsum += readings[i];
  unsigned short average = bigsum / READINGS;
  
  unsigned short ratio = (double) sum / (average+1) * 100;
  
  if (restore_time && millis() >= restore_time) {
    restore_time = 0;
    if (settingschanged) {
      Serial.println("Saving settings");
      settingschanged = false;
    }
  }
  
  //Serial.println(ratio);
  boolean newledstate = ledstate 
    ? (ratio >  lower_threshold)
    : (ratio >= upper_threshold);

  int numleds = ratio - lower_threshold;
  if (numleds < 0) numleds = 0;
  if (numleds > 8) numleds = 8;
  unsigned long ledmask = 0xff >> 8 - numleds;
  if (newledstate) ledmask <<= 8;
   
  if ((!gotenough) || (!newledstate)) {
    readings[cursor++] = sum;
    if (cursor >= READINGS) {
      cursor = 0;
      if (!gotenough) {
        gotenough = true;
        Serial.println("Done averaging");
      }
    }
  }

  
  if (newledstate) hits++;
 
  if (newledstate == ledstate) return;
  
  digitalWrite(13, ledstate = newledstate);

  if (!ledstate) {
    Serial.print("Marker: ");
    Serial.print(millis() - previous);
    Serial.print(" ms (");
    Serial.print(hits, DEC);
    Serial.println(" readings)");
    hits = 0;
    return;
  }
  
  unsigned long now = millis();
  unsigned long time = now - previous;

  if (time < debounce_time) return;

  previous = now;  
 
  if (!cycle++) {
    Serial.println("Discarding incomplete cycle.");
    return;
  }
  
  double W = 1000 * ((double) MS_PER_HOUR / time) / cycles_per_kwh;
  Serial.print("Cycle ");
  Serial.print(cycle, DEC);
  Serial.print(": ");
  Serial.print(time, DEC);
  Serial.print(" ms, ");
  Serial.print(W, 2);
  Serial.println(" W");
  
}

