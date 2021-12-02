//
// pin assignments
//
const int MAGNET_PIN = 4;

void setup() 
{
  pinMode(MAGNET_PIN, OUTPUT);   
  
  Serial.begin(9600);
  Serial.println("Setup done.");
}

void loop() 
{
  Serial.println("Magnet high");
  digitalWrite(MAGNET_PIN, HIGH);
  delay(1000);
  
  Serial.println("Magnet low");
  digitalWrite(MAGNET_PIN, LOW);
  delay(1000);
}
