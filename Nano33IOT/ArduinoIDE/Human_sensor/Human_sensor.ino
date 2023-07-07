int HumanRead;

void setup() {
  // put your setup code here, to run once:
  pinMode(12,INPUT);
  Serial.begin(115200);
}

void loop() {
  HumanRead=digitalRead(12);
  //Serial.println(HumanRead);
  if(HumanRead==1)
  {Serial.println("Have people!");}
  else{Serial.println("No people!");}
  delay(200);

}
