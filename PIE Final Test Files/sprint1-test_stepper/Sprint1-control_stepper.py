from serial import Serial, SerialException

cxn = Serial('/dev/tty.usbmodem1411', baudrate=9600)

while(True):    
    try:        
        cmd_id = int(input("Please enter the number of steps to move the motor (1-200 inclusive "))        
        if int(cmd_id) > 200 or int(cmd_id) < 1:            
            print("Please insert a valid integer.")       
        else:            
            cxn.write([int(cmd_id)])            

            while cxn.inWaiting() < 1:                
                pass            
            result = cxn.readline();            
            print(result)    
    except ValueError:        
        print("You must enter an integer value between 1 and 2.")
