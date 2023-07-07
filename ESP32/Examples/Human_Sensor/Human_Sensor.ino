int HumanRead;
int i = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(4,INPUT);
  Serial.begin(115200);
  
}

void loop() {
  // HumanRead=analogRead(4);
  // Serial.println(HumanRead);
  HumanRead=digitalRead(4);
  //Serial.println(HumanRead);
  if(HumanRead==1)
  {Serial.print("Have people!");}
  else{Serial.print("No people!");}
  Serial.println(i);
  i++;
  delay(1000);

}
