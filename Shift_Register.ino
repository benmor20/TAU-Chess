 #define data_bit 4 //data
 #define clk 6 //clock  
 #define mc 7 //Clear

int data[8]={1,0,0,0,0,0,0,1};

void setup()   {
  pinMode(mc, OUTPUT); 
  pinMode(clk, OUTPUT);  
  pinMode(data_bit, OUTPUT);
 
  digitalWrite(mc, 1);  
  digitalWrite(clk, 0); 
  digitalWrite(data_bit, 0);
 } // end setup

void    pulsout(byte x)   {
  byte z = digitalRead(x);
  z = !z;
  digitalWrite(x, z);
  //delayMicroseconds(1);
  z = !z; // return to original state
  digitalWrite(x, z);
  return;
} // end pulsout()

void loop(){
   for(int i=0; i < 8; i++){
   // k = j & 0x01;
   if (data[i]==1){
      digitalWrite(data_bit, HIGH); 
      }
   else if (data[i]==0){
      digitalWrite(data_bit, LOW);
      }
   pulsout(clk);
   }
   delay(5000);
}
