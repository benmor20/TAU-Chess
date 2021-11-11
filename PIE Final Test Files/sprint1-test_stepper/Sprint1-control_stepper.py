from serial import Serial, SerialException

cxn = Serial('COM11', baudrate=9600)

while(True):            
        cmd_id = int(input("Please enter the number of steps to move the motor (1-200 inclusive): "))        
        print(cmd_id)
        if int(cmd_id) > 2000 or int(cmd_id) < 1:            
            print("Please insert a valid integer.")       
        else:            
            cxn.write([int(cmd_id)])
            while cxn.inWaiting() < 1:                
                pass                       
            result = cxn.readline();            
            print(result)    
    