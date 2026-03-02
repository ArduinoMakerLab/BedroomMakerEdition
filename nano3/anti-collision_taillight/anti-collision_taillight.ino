const int trigPin = 9; // HC-SR04 trig
const int echoPin = 10; // HC-SR04 echo
const int ledPin = 2;  // LED

void setup() {
  Serial.begin(115200);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(ledPin, OUTPUT);
}

float speed, distance;

void ledControl(bool state)
{
  if(state)
  {
    digitalWrite(ledPin, LOW);
  }
  else
  {
    digitalWrite(ledPin, HIGH);
  }
}

void ledLight(uint32_t time)
{
  digitalWrite(ledPin, LOW);
  //Serial.print("1");
  delay(time);
  digitalWrite(ledPin, HIGH);
  //Serial.print("0");
}

void ledBlink(uint32_t cycle, uint32_t count)
{
  for(uint32_t ii = 0 ; ii < count ; ii++)
  {
    ledLight(cycle/2);
    delay(cycle/2);
  }
}

typedef enum
{
  LED_ON = 0,
  LED_OFF,
  LED_BLINK
}LEDMode_E;

LEDMode_E led;
void loop() {
  speed = takeSpeedMeasure(200, &distance);
  if(distance < 0.5 && distance > 0)
  {
    if(speed < -0.2)
    {
      led = LED_BLINK;
    }
    else
    {
      led = LED_ON;
    }
  }
  else if(distance < 2 && distance > 0 && speed < -0.5)
  {
      led = LED_ON;
  }
  else
  {
    led = LED_OFF;
  }

  uint64_t us;
  switch(led)
  {
    case LED_ON:
      ledControl(true);
    break;
    case LED_OFF:
      ledControl(false);
    break;
    case LED_BLINK:
      ledBlink(200, 5);
      ledControl(true);
    break;
  }
  // float dis = takeOneMeasure(&us);
  // Serial.println((int)(dis*10));
  Serial.print("speed: ");
  Serial.print(speed);
  Serial.print("dis: ");
  Serial.print(distance);
  Serial.println("");
}

float takeOneMeasure(uint64_t *us)
{
  long duration;  // High-level duration of Echo pin (µs)
  float distance; // the calculated distance (cm)
  
  // send trig signal
  digitalWrite(trigPin, LOW);   // 
  delayMicroseconds(2);         // hold low-level 2us
  digitalWrite(trigPin, HIGH);   
  delayMicroseconds(10);        // send high-level 10us
  digitalWrite(trigPin, LOW);   // 

  // read echo pulse
  duration = pulseIn(echoPin, HIGH);

  // calculate distance:  distance(m) = time(s) x speed of sound(340m/s)
  // us to s:   duration / 1000000
  // distance(cm) = duration * 0.034 / 2 = duration / 58
  distance = (double)duration / 58.0;

  *us = duration;

  if(distance <= 400)
  {
    return distance / 100.0;
  }
  else
  {
    return 0;
  }  
}

float takeSpeedMeasure(uint32_t interval, float *distance)
{
  float d1, d2;
  uint64_t t1, t2;
  float speed = 0;
  uint16_t count = 0;

  *distance = 0;

  // average ths results
  for(uint16_t ii = 0 ; ii < 10 ; ii++)
  {
    d1 = takeOneMeasure(&t1);
    delay(interval / 10.0);
    d2 = takeOneMeasure(&t2);
    if(d2!=0 && d1!=0)
    {
      speed += (d2 - d1) / (interval / 10000.0 + (double)t1 / 1e6 + (double)t2 / 1e6);
      count++;
      *distance = d2;
    }
  }

  if(count)
  {
    speed /= count;
    
  }
  else
  {
    speed = 0;
  }

  // Serial.println("d2:");
  // Serial.println(d2);
  // Serial.println("d1:");
  // Serial.println(d1);
  // Serial.println("speed:");
  // Serial.println(speed);

  return speed;
}
