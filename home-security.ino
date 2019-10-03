#include <MFRC522.h>

/*
 * Pin layout should be as follows:
 * Signal     Pin           Pin
 * -----------------------------
 * Reset      ANY (D2)      RST
 * SPI SS     ANY (A2)      SDA
 * SPI MOSI   A5            MOSI
 * SPI MISO   A4            MISO
 * SPI SCK    A3            SCK
 *
 */

#define SS_PIN SS
#define RST_PIN D2

/**
 * GLOBALS
 */
int BUZZER_PIN = D3;
int TRIG_PIN = D4;
int ECHO_PIN = D5;

int GREEN_LED_PIN = D0;
int RED_LED_PIN = D1;

// min and max distance values for our ultrasonic sensor
int MAX_DISTANCE = 200;
int MIN_DISTANCE = 0;

// distance in CM from the sensor that will trigger the alarm
int TRIGGER_DISTANCE_CM = 25;

// update with your own whitelist. remember to update the count below as well!
// use rfid_dumper.ino to get the uid of one of your own cards
String APPROVED_KEYCARDS[] = {
     "d984bd55" // white card
    ,"ed9cfaed"  // school ID
    //,"06c7baf7" // blue rfid keytag
};
int NUM_APPROVED_KEYCARDS = 2;

// mutable globals
String card_id = "";
double distance_cm = 0.0;
bool alarm_triggered = false;

// MFRC522 instance
MFRC522 mfrc522(SS_PIN, RST_PIN);

/**
 * SETUP & LOOP
 */
void setup() 
{
    // init our pins
    pinMode(RED_LED_PIN, OUTPUT);
    pinMode(GREEN_LED_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    
    // init MFRC522 card
    mfrc522.setSPIConfig();
    mfrc522.PCD_Init(); 
}

void loop() 
{
    if(alarm_triggered)
    {
        ring_the_alarm();
        rfid_scan_handler();
    }
    else
    {
        monitor_movement();
    }
}

/**
 * FUNCTIONS
 */
void monitor_movement() {
    if(!alarm_triggered)
    {
        distance_cm = get_distance_cm();
        
        if(distance_cm < TRIGGER_DISTANCE_CM && distance_cm != -1)
        {
            alarm_triggered = true;
            Particle.publish("alarm_state", "triggered");
        }
        
        delay(100);
    }
}

void rfid_scan_handler()
{
    // look for new cards
    if (!mfrc522.PICC_IsNewCardPresent()) 
        return;

    // select one of the cards
    if (!mfrc522.PICC_ReadCardSerial()) 
        return;
    
    // now we have a card selected, fetch the UID and assign it to our id string
    card_id = "";
    for(byte i = 0; i < mfrc522.uid.size; i++) 
    {
        card_id += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
        card_id += String(mfrc522.uid.uidByte[i], HEX);
    }
    
    mfrc522.PICC_HaltA();
    
    for(int i = 0; i < NUM_APPROVED_KEYCARDS; i++) 
    {
        if(card_id == APPROVED_KEYCARDS[i]) 
        {
            stop_the_alarm();
            break; 
        }
    }
    
    // if the alarm is still triggered, we found no match on the scanned rfid
    if(alarm_triggered)
    {
        buzzer_rejected_id();
    }
}

void ring_the_alarm()
{
    digitalWrite(GREEN_LED_PIN, LOW);
    
    digitalWrite(RED_LED_PIN, HIGH);
    ring_buzzer(500);
    digitalWrite(RED_LED_PIN, LOW);
    ring_buzzer(500);
}

void stop_the_alarm()
{
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, HIGH);
    
    buzzer_approved_id();
    
    alarm_triggered = false;
    Particle.publish("alarm_state", "disarmed");
}

double get_distance_cm()
{
    send_trigger_pulse(TRIG_PIN);
    
    wait_for_echo(ECHO_PIN, HIGH, 100);
    long start_time = micros();
    
    wait_for_echo(ECHO_PIN, LOW, 100);
    long end_time = micros();
    
    long duration = end_time - start_time;
    double distance = duration / 29.0 / 2.0;
    
    if (distance < MIN_DISTANCE || distance > MAX_DISTANCE)
    {
        return -1;
    }
    
    return distance;
}

void send_trigger_pulse(int pin)
{
    digitalWrite(pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(pin, LOW);
}

void wait_for_echo(int pin, int value, long timeout)
{
    long giveup_time = millis() + timeout;
    while (digitalRead(pin) != value && millis() < giveup_time)
    {
        // wait
    }
}

void ring_buzzer(int t_delay)
{
    digitalWrite(BUZZER_PIN, HIGH);
    delay(t_delay);
    digitalWrite(BUZZER_PIN, LOW);
    delay(t_delay);
}

// two medium-length buzzes for an OK rfid scan
void buzzer_approved_id()
{
    for(int i = 0; i < 2; i++)
        ring_buzzer(100);
}

// three short buzzes for a rejected rfid scan
void buzzer_rejected_id()
{
    for(int i = 0; i < 3; i++)
        ring_buzzer(50);
        
    delay(500);
}