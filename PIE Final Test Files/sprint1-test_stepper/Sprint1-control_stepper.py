from serial import Serial, SerialException

cxn = Serial('/dev/tty.usbmodem1411', baudrate=9600)

while(True):    
    try:        
        cmd_id = int(input("Please enter a command ID (1 - read potentiometer, 2 - read the button: "))        
        if int(cmd_id) > 2 or int(cmd_id) < 1:            
            print("Values other than 1 or 2 are ignored.")       
        else:            
            cxn.write([int(cmd_id)])            

            while cxn.inWaiting() < 1:                
                pass            
            result = cxn.readline();            
            print(result)    
    except ValueError:        
        print("You must enter an integer value between 1 and 2.")
