void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(9,OUTPUT);
  pinMode(13,OUTPUT);
  digitalWrite(9,LOW);
}

void LEDblick()
{
  byte statue = digitalRead(9);
  digitalWrite(13,statue);
}

void loop() {
  // put your main code here, to run repeatedly:



  attachInterrupt(digitalPinToInterrupt(9),LEDblick,CHANGE);
}
