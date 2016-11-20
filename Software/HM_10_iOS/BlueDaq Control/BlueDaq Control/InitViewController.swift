//
//  BlueDaq Control
//  Inital View Controller
//  HM10 BlueTooth Serial
//  Based on "Bluetooth Serial" by Alex @ Balancing Rock.
//


import UIKit
import CoreBluetooth
import QuartzCore






final class InitViewController: UIViewController, UIPickerViewDelegate, UIPickerViewDataSource, BluetoothSerialDelegate {

//MARK: Variables
    var pickerData: [[String]] = [[String]]()
    var message_buffer : String = ""
    let serial_core = bludaq_core_serial()
    var user_authd = false;
    
//MARK: IBOutlets
    
    
    @IBOutlet weak var connect_button: UIButton!        // Connect Button
    @IBOutlet weak var device_label: UILabel!           // Big Label (info)
    @IBOutlet weak var uuid_label: UILabel!             // Small Label (UUID)
    @IBOutlet weak var num_picker: UIPickerView!        // Code Wheel
    @IBOutlet weak var passcode_label: UILabel!         // Code Label
    @IBOutlet weak var passcode_button: UIButton!       // Code Button
    
//MARK: Functions
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // init serial
        serial = BluetoothSerial(delegate: self)
        serial.writeType = .withoutResponse

        reloadView()
    }
    

    deinit {
        NotificationCenter.default.removeObserver(self)
    }
    

    
    // Change Status Label and Buttons:
    func reloadView() {
        // in case we're the visible view again
        serial.delegate = self

        if serial.isReady {
            showPasscode(state: true)
            device_label.text = serial.connectedPeripheral!.name
            uuid_label.text = serial.connectedPeripheral!.identifier.uuidString
            uuid_label.isHidden = false
            connect_button.setTitle("Disconnect", for: .normal)
            connect_button.tintColor = UIColor.red
            connect_button.isEnabled = true
            
        } else if serial.centralManager.state == .poweredOn {
            showPasscode(state: false)
            device_label.text = "Bluetooth Ready..."
            uuid_label.isHidden = true
            connect_button.setTitle("Choose Device", for: .normal)
            connect_button.tintColor = view.tintColor
            connect_button.isEnabled = true
            
        } else {
            showPasscode(state: false)
            uuid_label.isHidden = true
            device_label.text = "BlueTooth Off?"
            connect_button.setTitle("Choose Device", for: .normal)
            connect_button.tintColor = UIColor.gray
            connect_button.isEnabled = true
        }
    }

    
    

//MARK: BluetoothSerialDelegate
    
    // Device Disconnected
    func serialDidDisconnect(_ peripheral: CBPeripheral, error: NSError?) {
        reloadView()
        let hud = MBProgressHUD.showAdded(to: view, animated: true)
        hud?.mode = MBProgressHUDMode.text
        hud?.labelText = "Disconnected"
        hud?.hide(true, afterDelay: 1.0)
    }
    
    // Device Changed State
    func serialDidChangeState() {
        reloadView()
        if serial.centralManager.state != .poweredOn {

            let hud = MBProgressHUD.showAdded(to: view, animated: true)
            hud?.mode = MBProgressHUDMode.text
            hud?.labelText = "Bluetooth turned off"
            hud?.hide(true, afterDelay: 1.0)
        }
    }
    
    // Message Recieved
    func serialDidReceiveString(_ msg: String) {
        messageResponder(message: msg)
        
    }
    
//MARK: Picker
    
    
    // Setup Passcode Picker
    func load_picker(){
        
        // Connect data:
        self.num_picker.delegate = self
        self.num_picker.dataSource = self
        
        // Input data into the Array:
        pickerData = [["0", "1", "2", "3", "4", "5", "6", "7", "8", "9"], ["0", "1", "2", "3", "4", "5", "6", "7", "8", "9"], ["0", "1", "2", "3", "4", "5", "6", "7", "8", "9"], ["0", "1", "2", "3", "4", "5", "6", "7", "8", "9"]]
        
    }


    // The number of columns of data
    func numberOfComponents(in pickerView: UIPickerView) -> Int {
        return 4
    }
    
    // The number of rows of data
    func pickerView(_ pickerView: UIPickerView, numberOfRowsInComponent component: Int) -> Int {
        return 10
    }
    
    // The data to return for the row and component (column) that's being passed in
    func pickerView(_ pickerView: UIPickerView, titleForRow row: Int, forComponent component: Int) -> String? {
        return pickerData[component][row]
        
    }
    
    // Function updates on change of picker
    func pickerView(_ pickerView: UIPickerView, didSelectRow row: Int, inComponent component: Int) {
    

    }

    // Return (int) of passcode (uses position not value)
    func getCode() -> String{
    
        var PassCode : String
        PassCode = String(num_picker.selectedRow(inComponent: 0)) + String(num_picker.selectedRow(inComponent: 1)) + String(num_picker.selectedRow(inComponent: 2)) + String(num_picker.selectedRow(inComponent: 3))

        return PassCode
        
    }
    
    // Show Passcode Picker
    func showPasscode(state : Bool)  {
        
        if(state){
            load_picker()
            num_picker.isHidden = false
            passcode_label.isHidden = false
            passcode_button.isHidden = false
            passcode_label.text = "Enter Passcode"
        
        }
        else{
        
            num_picker.isHidden = true
            passcode_label.isHidden = true
            passcode_button.isHidden = true
        }
    
    }
    
    
    
//MARK: Message Responder
    
    
    // Assume validated
    func messageResponder(message : String){
        let this_message = bludaq_core_serial.transaction_req.Authenticate
        
        // Note: Value passed in callback function: t_resp is created by serial_core method instance
        if(serial_core.parse_message(message: message, t_type: this_message, completion: {(t_resp: bludaq_core_serial.transaction_resp) -> Void in
            
             passcode_label.text = t_resp.body
            
            
            /*
            // Check "good" value:
            if(t_resp.good_msg){
                
                // Check Passcode: (2 = TRUE)
                if (serial_core.body_types[2] == t_resp.body){
                
                    user_authd = true; // User has authed
                }
                else{
                
                    passcode_label.text = t_resp.body
                }
                
            }
            
            */
            
        })){ // Begin if(serial.core.parse_message()
        
        // Do nothing? (if callback worked we're authenticated...
        passcode_label.text = "Device Connected"
        enable_views()
        return
        
        
        }
        else{
        
        // Message Not Recognized
        
        //passcode_label.text = "Device Error"
        return
        
        }
        
    }


//MARK: Enable Applicaton ViewControllers 

    func enable_views(){
    
    
    }

    
    
//MARK: IBActions


    // Passcode Button Pressed
    @IBAction func passcode_buttonp(_ sender: Any) {
        
        let passCode : String = getCode() + "\n"
        serial.sendMessageToDevice(passCode)
    
    }
    
    // Return To This View (unwind)
    @IBAction func unwindToInit(sender: UIStoryboardSegue) {
        
        // Refresh:
        reloadView()
        
        // Return from Scanner
        if sender.source is ScannerViewController {
        
            // Check Connectivity:
            if !serial.isReady {
                device_label.text = "Please Select Device"
                
                // Display Popup:
                /*
                let alert = UIAlertController(title: "Please Select Device", message: "Please connect to a BlueDaq Device.", preferredStyle: .alert)
                let OKAction = UIAlertAction(title: "OK", style: UIAlertActionStyle.default, handler: {
                    (_)in
                    self.performSegue(withIdentifier: "ShowScanner", sender: self)
                })
        
                alert.addAction(OKAction)
                self.present(alert, animated: true, completion: nil)
                */
            }
         }
         
        // Return from other view...
        else {
            
        }
        
    }

    @IBAction func connect_dev(_ sender: AnyObject) {
        if serial.connectedPeripheral == nil {
            performSegue(withIdentifier: "ShowScanner", sender: self)
        } else {
            serial.disconnect()
            reloadView()
        }
    }
}


