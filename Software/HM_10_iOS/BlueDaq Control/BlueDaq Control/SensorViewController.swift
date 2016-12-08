/*
*
*   BluDAQ Sensor ViewController
*
*   VC for Sensor Scene
*   Displays All Current Sensor Data
*   Handels all Transmit and Recive functions for automation and sensor data
*   Also has conversion equations altitude and sea level pressure
*
*
*/

import UIKit
import CoreBluetooth
//import QuartzCore
import Foundation



final class SensorViewController: UIViewController, BluetoothSerialDelegate {

//MARK: Variables

    var message_buffer : String = ""                                            // Global Message Buffer
    let serial_core = bludaq_core_serial()                                      // Serial Message Functions
    let accesories = bluedaq_settings()                                         // App Settings Class
    var current_settings = bluedaq_settings.prefs()                             // Current App Settings
    var automation_dat = [bluedaq_settings.auto(), bluedaq_settings.auto()]     // Current Automation Settings
    var loaded_auto = true                                                      // Automation Load State
    var sensor_dat = bluedaq_settings.sensor_dat()                              // Sensor Data Struct
	var connection_err : Bool = false
    var celsius = true;                                                         // celsius or fahrenheit

    
    
//MARK: IBOutlets

    @IBOutlet weak var light_txtbox: UITextField!
    @IBOutlet weak var humi_txtbox: UITextField!
    @IBOutlet weak var pres_txtbox: UITextField!
    @IBOutlet weak var temp_txtbox: UITextField!
    @IBOutlet weak var motion_label: UILabel!       // Swap with image? Notifications?
    @IBOutlet weak var timestamp_label: UILabel!
    @IBOutlet weak var store_auto_button: UIButton!

    @IBOutlet weak var temp_button_label: UIButton!


//MARK: Functions
    
    
    // View Loaded
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // Connect Serial
		//serial = BluetoothSerial(delegate: self)
        //serial.writeType = .withoutResponse
        serial.delegate = self

        if serial.centralManager.state != .poweredOn {
            title = "Bluetooth Disabled"
            return
        }
        
        // Setup View:
        load_automation()
        update_view()
        
        // Get Data:
        serial.sendMessageToDevice(serial_core.data_read(state: true))
        
    }
    
    
    deinit {
        NotificationCenter.default.removeObserver(self)
    }
    
    
    
    // Load Automation from User Prefs
    func load_automation(){
        
        // Get Data:
        if(loaded_auto){
            automation_dat[0] = accesories.load_automation(index: 0)
            automation_dat[1] = accesories.load_automation(index: 1)
            
            loaded_auto = false
        }
    }
    
    
    
    // Update View : Loads all views from memory
    func update_view(){
        
        
        // Sensor Data
        light_txtbox.text = String(sensor_dat.light)
        humi_txtbox.text = ("\(sensor_dat.humi) %")
        pres_txtbox.text = ("\(sensor_dat.pres) hPa")
        temp_txtbox.text = celsius ? "\(sensor_dat.temp) \u{2103}" : "\(fahrenheit(celsius: sensor_dat.temp)) \u{2109}"
        motion_label.text = sensor_dat.motion ? "Motion Detected" : "No Motion"
        timestamp_label.text = sensor_dat.timestamp
        
        // Other Elements:
        temp_button_label.setTitle(celsius ? "Temperature (\u{2109})" : "Temperature (\u{2103})", for: .normal)
    
    }
	
	// Serial Message Recieved
    func serialDidReceiveString(_ msg: String) {
        
        let charset = CharacterSet.init(charactersIn: "\r\n")
        
        let msg_array = msg.components(separatedBy: charset)
            
        for lines in msg_array{
            if(lines.characters.count > 7){
                messageResponder(message: lines)
            }
        }
    }
    
    
    // Parse Incoming Message
    func messageResponder(message : String){
		
		let message_resp = serial_core.parse_message(message: message)

		// Check "good" value:
		if(message_resp.good_msg){
            
            switch (message_resp.msg_type) {
            
                case -1:    // Unknown
                    timestamp_label.text = message_resp.body       // DEBUG
                    break
                
                case 0:     // Device Booted
                
                    break
                
                case 1:     // Authentication State
					
						// Check Passcode: (2 = TRUE)
						if (serial_core.body_types[2] == message_resp.body){
								current_settings.auth = true // User has authed
								connection_err = false
						}
			
						else if(serial_core.body_types[1] == message_resp.body){
							
							if(connection_err){
								// Problem Connecting
								self.performSegue(withIdentifier: "unwindToInit", sender: self)
							}
							
							else{
								current_settings.auth = false
								serial.sendMessageToDevice(current_settings.passcode) // Send Passcode
								connection_err = true // Mark in case of return
							}
						}
							
                    break
                
                case 2:     // Device Sleeping
                
                    break
                
                case 3:     // Device Error Message
                    timestamp_label.text = message_resp.body      // DEBUG
                    break
                
                case 4:     // Data: Temperature
                    sensor_dat.temp = serial_core.parse_float(msg_body: message_resp.body)
                    break
                
                case 5:     // Data: Humidity
                    sensor_dat.humi = serial_core.parse_float(msg_body: message_resp.body)
                    break
                
                case 6:     // Data: Pressure
                    sensor_dat.pres = serial_core.parse_float(msg_body: message_resp.body)
                    break
                
                case 7:     // Data: Photoresistor (Light Sensor)
                    sensor_dat.light = serial_core.parse_int(msg_body: message_resp.body)
                    break
                
                case 8:     // Data: PIR State (Motion)
                    sensor_dat.motion = serial_core.parse_bool(msg_body: message_resp.body)
                    break
                
                case 9:     // Data: Relay 0 State
                    current_settings.automation_0 = serial_core.parse_bool(msg_body: message_resp.body)
                    break
                
                case 10:    // Data: Relay 1 State
                    current_settings.automation_1 = serial_core.parse_bool(msg_body: message_resp.body)
                    break
                
                default:
                
                    break
                
            } // END SWITCH //
            
            // Update Timestamp:
            sensor_dat.timestamp = accesories.timestamp()
            
        }
		else{   // Message Not Recognized
			
            // Ignore for now...
			//timestamp_label.text = "Device Error"
        }
        
        // Display Results
         update_view()
    }

    
    // Send Automation Settings to Device
    
    
    func store_automation(){
        
        var index = 0
        
		for i in automation_dat{
            
            // Decending Flag
            if(i.descending){
            
                serial.sendMessageToDevice("\(serial_core.tx_msg_types[10]!)\(String(index))\(serial_core.body_types[2]!)\n")
            }
            else{
                serial.sendMessageToDevice("\(serial_core.tx_msg_types[10]!)\(String(index))\(serial_core.body_types[1]!)\n")
            }
            
            // Enable !
            if(i.enabled){
                serial.sendMessageToDevice("\(serial_core.tx_msg_types[1]!)\(String(index))\(serial_core.body_types[2]!)\n")
            }
            else{
                serial.sendMessageToDevice("\(serial_core.tx_msg_types[1]!)\(String(index))\(serial_core.body_types[1]!)\n")
            }
            
            // Toggle
            if(i.toggle){
                serial.sendMessageToDevice("\(serial_core.tx_msg_types[9]!)\(String(index))\(serial_core.body_types[2]!)\n")
            }
            else{
                serial.sendMessageToDevice("\(serial_core.tx_msg_types[9]!)\(String(index))\(serial_core.body_types[1]!)\n")
            }
            
            // Duration !
            serial.sendMessageToDevice("\(serial_core.tx_msg_types[3]!)\(String(index))\(String(i.duraton))\n")
            
            // Setpoint !
            serial.sendMessageToDevice("\(serial_core.tx_msg_types[2]!)\(String(index))\(String(i.setpoint))\n")
            
            
            
            // Sensor !
            serial.sendMessageToDevice("\(serial_core.tx_msg_types[0]!)\(String(index))\(String(i.sensor.rawValue))\n")
            

            
            index += 1
		}

            // ADD A COMMAND //
            // Need to tell device to store values to EEPROM when the write has completed...
            serial.sendMessageToDevice("\(serial_core.tx_msg_types[11]!)\(serial_core.body_types[2]!)\n")
    }
 
    

    

    

//MARK: BluetoothSerialDelegate
    
    func serialDidDisconnect(_ peripheral: CBPeripheral, error: NSError?) {

        let hud = MBProgressHUD.showAdded(to: view, animated: true)
        hud?.mode = MBProgressHUDMode.text
        hud?.labelText = "Disconnected"
        hud?.hide(true, afterDelay: 1.0)
    }
    
    func serialDidChangeState() {
    
        if serial.centralManager.state != .poweredOn {

            let hud = MBProgressHUD.showAdded(to: view, animated: true)
            hud?.mode = MBProgressHUDMode.text
            hud?.labelText = "Bluetooth turned off"
            hud?.hide(true, afterDelay: 1.0)
        }
    }
    
    
    
    
//MARK: Segue Prep

override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
    
    // Disable Data:
    serial.sendMessageToDevice(serial_core.data_read(state: false))
    
    // Automation View
    if segue.identifier == "ShowAuto" {
        let controller = segue.destination as! AutoViewController
        controller.current_settings = current_settings // Copy Settings
    }
}
    
    
    
//MARK: IBActions

    // Trigger Unit Change on Temp
    @IBAction func temp_button(_ sender: Any) {
        celsius = !celsius
        update_view()
    }
    
    // Store Automation
    @IBAction func store_automation(_ sender: Any) {
    
        store_automation()

    }

    // Return To This View (unwind)
    @IBAction func unwindToSensor(sender: UIStoryboardSegue) {
	
        // Get data from AutomationVC
        if let sourceViewController = sender.source as? AutoViewController {
            current_settings = sourceViewController.current_settings
        }
    }

// Unit conversions and things:

/**************************************************************************/
/*
    Altitude:
    
    Equation taken from BMP180 datasheet (page 16):
    http://www.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf
    
    Note that using the equation from wikipedia can give bad results
    at high altitude.  See this thread for more information:
    http://forums.adafruit.com/viewtopic.php?f=22&t=58064
    
    
    Pressure at Sea Level for Current Altitude:
    
    Equation taken from BMP180 datasheet (page 17):
    http://www.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf

    Note that using the equation from wikipedia can give bad results
    at high altitude.  See this thread for more information:
    http://forums.adafruit.com/viewtopic.php?f=22&t=58064

    Calculates the pressure at sea level (in hPa) from the specified altitude 
    (in meters), and atmospheric pressure (in hPa).  
    @param  altitude      Altitude in meters
    @param  atmospheric   Atmospheric pressure in hPa
*/
/**************************************************************************/

    
    // Get Altitude with Pressure at Sea Level
    // Requires current pressure at Sea Level in hPa
    func get_altitude(seaLevel : Float, pressure : Float) -> Float{

        let atmospheric = pressure / 100.0
        return 44330.0 * (1.0 - pow(atmospheric / seaLevel, 0.1903))
    }


    // Get Current Pressure at Sea Level
    // Requires Current Altitude and Pressure
    func seaLevelForAltitude(altitude : Float, atmospheric : Float) -> Float{

        return atmospheric / pow(1.0 - (altitude/44330.0), 5.255)
    }

    func fahrenheit(celsius : Float) -> Float{
        return celsius * (9/5) + 32
    }

}
