//#define ground_row 4 //data
#define green_column 5 //data
#define red_column 6 //data
#define clk 8 //clock  
#define A 11 //select mux 
#define B 10 //select mux 
#define C 9 //select mux
#define A1 13 //select mux 
#define B1 12 //select mux 
#define C1 2 //select mux

#define sensor_read A0 //select
int threshold=550;
int green_led[8][8]={{0,0,0,0,0,0,0,0},
                     {0,0,0,0,0,0,0,0},
                     {0,0,0,0,0,0,0,0},
                     {0,0,0,0,0,0,0,0},
                     {0,0,0,0,0,0,0,0},
                     {0,0,0,0,0,0,0,0},
                     {0,0,0,0,0,0,0,0},
                     {0,0,0,0,0,0,0,0}};
int red_led[8][8]={{0,0,0,0,0,0,0,0},
                   {0,0,0,0,0,0,0,0},
                   {0,0,0,0,0,0,0,0},
                   {0,0,0,0,0,0,0,0},
                   {0,0,0,0,0,0,0,0},
                   {0,0,0,0,0,0,0,0},
                   {0,0,0,0,0,0,0,0},
                   {0,0,0,0,0,0,0,0}};
 

//int sensor_data[8][8]={{0,0,0,0,0,0,0,0},
//                       {0,0,0,0,0,0,0,0},
//                       {0,0,0,0,0,0,0,0},
//                       {0,0,0,0,0,0,0,0},
//                       {0,0,0,0,0,0,0,0},
//                       {0,0,0,0,0,0,0,0},
//                       {0,0,0,0,0,0,0,0},
//                       {0,0,0,0,0,0,0,0}};
//int value;
//int sending_ground_row_data[8]={0,0,0,0,0,0,0,0};
int sending_green_column_data[8]={0,0,0,0,0,0,0,0};
int sending_red_column_data[8]={0,0,0,0,0,0,0,0};
int C_list[8]={0,0,0,0,1,1,1,1};
int B_list[8]={0,0,1,1,0,0,1,1};
int A_list[8]={0,1,0,1,0,1,0,1};
void setup()   {
pinMode(clk, OUTPUT);  
//pinMode(ground_row, OUTPUT);
pinMode(green_column, OUTPUT);
pinMode(red_column, OUTPUT);
pinMode(A, OUTPUT);
pinMode(B, OUTPUT);
pinMode(C, OUTPUT);
pinMode(A1, OUTPUT);
pinMode(B1, OUTPUT);
pinMode(C1, OUTPUT);
pinMode(sensor_read, INPUT);
//   digitalWrite(A1,0);
//digitalWrite(B1,0);
//digitalWrite(C1,0);

   Serial.begin(9600);
 } // end setup

void    pulsout(byte x)   {
  byte z = digitalRead(x);
  z = !z;
  digitalWrite(x, z);
  delayMicroseconds(1);
  z = !z; // return to original state
  digitalWrite(x, z);
  return;
} // end pulsout()

void writedata(){
     for(int k=7; k >= 0; k--){
      //digitalWrite(ground_row,sending_ground_row_data[k]); 
//      Serial.println(k);
//      Serial.println(sending_green_column_data[k]);
      digitalWrite(green_column, sending_green_column_data[k]);
      digitalWrite(red_column, sending_red_column_data[k]);  
      pulsout(clk);
   }
   return;
}

void loop(){
  for (int i=0; i<8; i++){
    digitalWrite(A1,A_list[i]);
    digitalWrite(B1,B_list[i]);
    digitalWrite(C1,C_list[i]);

    delay(10);
    scansensor();
    

   for (int j=0; j<8; j++){
   if (red_led[i][j]==1){
    //writes
    sending_red_column_data[j]=1;
    
    writedata();
    delay(1000);// Time between switching Between LEDs
    //erases
    sending_red_column_data[j]=0;
   }
   if (green_led[i][j]==1){
       //writes
    sending_green_column_data[j]=1;
    writedata();
    delay(1000);// Time between switching Between LEDs
    //erases
    sending_green_column_data[j]=0;
   }
   writedata();
    
  }
 
}
 Serial.println("----------------------");
 delay(1000);
}
void scansensor(){
    digitalWrite(A,0);
    digitalWrite(B,0);
    digitalWrite(C,0);
    Serial.print(threshold>analogRead(sensor_read));
    Serial.print(", ");
    digitalWrite(A,1);
    digitalWrite(B,0);
    digitalWrite(C,0);
    Serial.print(threshold>analogRead(sensor_read));
        Serial.print(", ");
    digitalWrite(A,0);
    digitalWrite(B,1);
    digitalWrite(C,0);
    Serial.print(threshold>analogRead(sensor_read));
        Serial.print(", ");
    digitalWrite(A,1);
    digitalWrite(B,1);
    digitalWrite(C,0);
    Serial.print(threshold>analogRead(sensor_read));
        Serial.print(", ");
    digitalWrite(A,0);
    digitalWrite(B,0);
    digitalWrite(C,1);
    Serial.print(threshold>analogRead(sensor_read));
        Serial.print(", ");
    digitalWrite(A,1);
    digitalWrite(B,0);
    digitalWrite(C,1);
    Serial.print(threshold>analogRead(sensor_read));
        Serial.print(", ");
    digitalWrite(A,0);
    digitalWrite(B,1);
    digitalWrite(C,1);
    Serial.print(threshold>analogRead(sensor_read));
        Serial.print(", ");
    digitalWrite(A,1);
    digitalWrite(B,1);
    digitalWrite(C,1);
    Serial.println(threshold>analogRead(sensor_read)); 
}
