void setup() {
  Serial.begin(9600);
  while(!Serial){;}
  Serial.println("Start");
  String scan1 = "00000001000000020000000300000004";
  String scan2 = "00000005000000020000000300000004";
}

void loop() {
  String scan1 = "00000001000000020000000300000004";
  String scan2 = "00000005000000020000000300000004";
  uint8_t numberOfTags1 = scan1.length() / 8;

  uint8_t numberOfTags2 = scan2.length() / 8;
  String finalTags = keepIndividuals(scan1, numberOfTags1, scan2, numberOfTags2);
  Serial.println((String)"finalTags" + finalTags);
  
}


String keepIndividuals(String scan1, uint8_t nTags1, String scan2, uint8_t nTags2)
{
  uint8_t i = 0, j = 0;
  for (j = 0; j < nTags2; j++)
  {
    bool alreadyHere = false;
    if (scan2.substring(8 * j, 8 + 8 * j) != "bb01ff00")
    {
      for (i = 0; i < nTags1; i++)
      {
        if (scan1.substring(8 * i, 8 + 8 * i) == scan2.substring(8 * j, 8 + 8 * j))
        {
          alreadyHere = true;
          break;
        }
      }
      if (!alreadyHere)
      {
        scan1 += scan2.substring(8 * j, 8 + 8 * j);
      }
    }
  }
  return scan1;
}
