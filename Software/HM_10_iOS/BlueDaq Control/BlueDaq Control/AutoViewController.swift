//
//  BluDaq Automation View Controller
//
//

import UIKit
import CoreBluetooth
import QuartzCore
import Foundation



final class AutoViewController: UIViewController,  UIPickerViewDelegate, UIPickerViewDataSource, BluetoothSerialDelegate {

//MARK: Variables

    var pickerData = [["Temperature", "Pressure", "Humidity", "Light Level"]]
    var message_buffer : String = ""                // Global Message Buffer
    let serial_core = bludaq_core_serial()          // Serial Message Functions
    let accesories = bluedaq_settings()             // App Settings Class
    var current_settings = bluedaq_settings.prefs(init_ : true, auth : false, timestamp : "", last_UUID : "", passcode : "", last_device_name : "", automation_0 : false, automation_1 : false)
    


//MARK: IBOutlets
    
    @IBOutlet weak var title_label: UILabel!
    @IBOutlet weak var thresh_label: UILabel!
    @IBOutlet weak var automation_label: UILabel!
    @IBOutlet weak var motion_label: UILabel!
    @IBOutlet weak var sensor_selector: UIPickerView!
    @IBOutlet weak var threshold_txtbox: UITextField!
    @IBOutlet weak var asc_switch: UISwitch!
    @IBOutlet weak var motion_switch: UISwitch!
    @IBOutlet weak var asc_label: UILabel!
    @IBOutlet weak var chan_select: UISegmentedControl!
    

    
//MARK: Functions
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // Connect Serial
        serial.delegate = self
        
        if serial.centralManager.state != .poweredOn {
            title = "Bluetooth Disabled"
            return
        }
        
        // Picker
        load_picker()
    }
    
    
    deinit {
        NotificationCenter.default.removeObserver(self)
    }
  
//MARK: Picker

    // Setup Passcode Picker
    func load_picker(){
        
        // Connect data:
        self.sensor_selector.delegate = self
        self.sensor_selector.dataSource = self
        
        // Input data into the Array:
        
    }
    
    //# of Components (col)
    func numberOfComponents(in pickerView: UIPickerView) -> Int {
        return pickerData.count
    }
    
    // Row Count
    func pickerView(_ pickerView: UIPickerView, numberOfRowsInComponent component: Int) -> Int {
        return pickerData[component].count
    }

    // Data Deligate
    func pickerView(_ pickerView: UIPickerView, titleForRow row: Int, forComponent component: Int) -> String? {
        return pickerData[component][row]
    }
    
    // Get Selected Value
    func getDevice() -> String{
       
        return pickerData[0][sensor_selector.selectedRow(inComponent: 0)]

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

//MARK: IBActions:


    @IBAction func asc_button(_ sender: Any) {
    
        if asc_switch.isOn == true{ // Above
            asc_label.text = "above"
        }
        else{
            asc_label.text = "below"
        }
    
    }


    @IBAction func motion_button(_ sender: Any) {
    }


    @IBAction func chan_button(_ sender: Any) {
    }

    @IBAction func txtbox_button(_ sender: Any) {
    }



//MARK: Segue Prep

override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
	
	// Unwind to Sensor View
    if segue.identifier == "unwindToSensor" {
        let controller = segue.destination as! InitViewController
        controller.current_settings = current_settings // Copy Settings
    }
    
}
    
    
//MARK: IBActions


}
