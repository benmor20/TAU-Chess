 #define col_data_bit 5 //data
  #define row_data_bit 4 //data
 #define clk 6 //clock  
int LED_MATRIX[ 8 ][ 8 ]={{1,0,0,0,0,0,0,0}, 
                          {0,0,0,0,0,0,0,0},
                          {1,0,0,0,0,0,0,0},
                          {1,0,0,1,0,0,0,0},
                          {0,0,0,0,0,0,0,0},
                          {0,0,0,0,0,0,0,0},
                          {0,0,0,0,0,0,0,0},
                          {0,0,0,0,0,0,0,0}};
                 
int col_data[8]={0,0,0,0,0,0,0,0}; //reversed order (positive, voltage)
int row_data[8]={1,1,1,1,1,1,1,1}; //reversed order (negative, ground)

void setup()   {
  pinMode(clk, OUTPUT);  
  pinMode(col_data_bit, OUTPUT);
  pinMode(row_data_bit, OUTPUT);
 
 } // end setup

void    pulsout(byte x)   {
  byte z = digitalRead(x);
  z = !z;
  digitalWrite(x, z);
  z = !z; // return to original state
  digitalWrite(x, z);
  return;
} // end pulsout()

void writedata(){
     for(int k=0; k < 8; k++){
       digitalWrite(col_data_bit, col_data[k]); 
       digitalWrite(row_data_bit, row_data[k]); 
   pulsout(clk);
   }
   return;
}

void loop(){
  for (int i=0; i<8; i++){
   for (int j=0; j<8; j++){
   if (LED_MATRIX[j][i]==1){
    //writes
    col_data[j]=1;
    row_data[i]=0;
    writedata();
    delay(3.5);// Time between switching Between LEDs
    //erases
    col_data[j]=0;
    row_data[i]=1;
   }
   }
  }
}
