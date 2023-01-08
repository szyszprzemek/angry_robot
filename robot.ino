/* Includes */
#include <Servo.h>

/* Defines */
#define PIN_ULTRASONIC_TRIG 12
#define PIN_ULTRASONIC_ECHO 11
#define PIN_BUZZER          13
#define PIN_RED_1           2
#define PIN_RED_2           3
#define DISTANCE_TRIG       50
#define STATE_HOLD_TIME     150

/* Globals */
Servo servo;
int mode = 0; /* 0 = IDLE_MODE, 1 = SWEAR_MODE */
int state_timer = 0;
long timestamp_buzzer = 0;
long timestamp_hand = 0;
long buzzer_timer = 0;
long hand_timer = 0;
boolean buzzer_value = false;
boolean hand_value = false;
  
/* Callback function to measure distance from Ultrasonic sensor */
int measureDistance()
{
  long time1;

  /* Please note that the delays are essential and not possible to avoid */
  digitalWrite(PIN_ULTRASONIC_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_ULTRASONIC_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_ULTRASONIC_TRIG, LOW);

  time1 = pulseIn(PIN_ULTRASONIC_ECHO, HIGH);
  return time1 / 58;
}

void setup() 
{
  /* Serial */
  Serial.begin(9600);

  /* PINS init */
  pinMode(PIN_ULTRASONIC_TRIG, OUTPUT);
  pinMode(PIN_ULTRASONIC_ECHO, INPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_RED_1, OUTPUT);
  pinMode(PIN_RED_2, OUTPUT);
  servo.attach(9);
}

void loop() 
{
  int distance;

  /* Distance measurement */
  distance = measureDistance();
  Serial.print(distance);
  Serial.print(" cm, state_t =");
  Serial.print(state_timer);
  Serial.print(" , buzzer_t =");
  Serial.print(buzzer_timer);
  Serial.print(" , hand_t =");
  Serial.print(hand_timer);
  Serial.print(" ,");
  Serial.println(" ");
  
  /* Logic states */
  if (((0 == mode) && (distance > DISTANCE_TRIG)) ||
      ((1 == mode) && (state_timer > STATE_HOLD_TIME)))
  {
    mode = 0;
    state_timer = 0;
    buzzer_timer = 0;
    hand_timer = 0;
  }
  else 
  {
    mode = 1;
    state_timer += 1;
  }

  /* Handle IDLE */
  if (0 == mode)
  {
    digitalWrite(PIN_BUZZER, LOW);
    digitalWrite(PIN_RED_1, LOW);
    digitalWrite(PIN_RED_2, LOW);  
    servo.write(0);
  }

  /* Handle SWEAR_MODE */
  if (1 == mode)
  {
    /* Control swearing buzzer */
    if (buzzer_timer >= timestamp_buzzer)
    {
      timestamp_buzzer = random(10, 40);
      buzzer_timer = 0;

      /* Once swear and once silent */
      if (false == buzzer_value)
      {
        buzzer_value = true;
        digitalWrite(PIN_BUZZER, LOW);
      }
      else
      {
        buzzer_value = false;
        digitalWrite(PIN_BUZZER, HIGH);
      }
    }

    /* Control hand movement */
    if (hand_timer >= timestamp_hand)
    {
      timestamp_hand = random(5, 10);
      hand_timer = 0;

      /* Wave hand */
      if (false == hand_value)
      {
        hand_value = true;
        servo.write(90); /* different value to produce jerky movement */
      }
      else
      {
        hand_value = false;
        servo.write(180); /* different value to produce jerky movement */
      }
    }

    /* Always light up the eyes of the robot */
    digitalWrite(PIN_RED_1, HIGH);
    digitalWrite(PIN_RED_2, HIGH);

    /* update timers */
    buzzer_timer += 1;
    hand_timer += 1;
  }
}
