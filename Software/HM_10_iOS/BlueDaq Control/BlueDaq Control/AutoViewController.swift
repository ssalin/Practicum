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
    var current_settings = bluedaq_settings.prefs()
    var auto_0 = bluedaq_settings.auto()
    var auto_1 = bluedaq_settings.auto()
    var loaded_auto = true


//MARK: IBOutlets
    
    @IBOutlet weak var title_label: UILabel!
    @IBOutlet weak var thresh_label: UILabel!
    @IBOutlet weak var automation_label: UILabel!
    @IBOutlet weak var motion_label: UILabel!
    @IBOutlet weak var sensor_selector: UIPickerView!
    @IBOutlet weak var threshold_txtbox: UITextField!
    @IBOutlet weak var enable_channel: UISwitch!
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

//MARK: Functions
    
    
    // Update the view - Something has changed...
    func update_view(){
        
        thresh_label.text = "When the \(getDevice()) is"
    
    }
    
    // Load Automation from User Prefs
    func load_automation(index: Int){


    
        // Get Data:
        if(loaded_auto){
            auto_0 = accesories.load_automation(index: 0)
            auto_1 = accesories.load_automation(index: 1)
        }
        
        let autos = [auto_0, auto_1]
        var selected = [0, 0]
        
        // Extrapolate Data:
        
        for i in selected{
            
            // Check Enabled
            if(!autos[1].enabled){
                selected[i] = 0;
                break;
            }
            
            // Selected Sensor
            if(autos[i].tmp_sel){
                selected[i] = 0
            }
            else if (autos[i].pres_sel){
                selected[i] = 1
                
            }
            else if (autos[i].hum_sel){
                selected[i] = 2
            }
            else if (autos[i].photo_sel){
                selected[i] = 3
            }
        
        }
        
        // Update Fields:
        if (chan_select.selectedSegmentIndex == 0){ // Channel A
            
            sensor_selector.selectRow(selected[0], inComponent: 0, animated: true)  // Set Picker
            title_label.text = ""
            
            
            
        }
        else { // Channel B
        
            sensor_selector.selectRow(selected[1], inComponent: 0, animated: true)
        }
        
        update_view()
        
    }
    
    
    
    // Save Automation to User Prefs
    func save_automation(){
        
        
        
        
        accesories.save_automation(index: 0, aut_data: auto_0)
        accesories.save_automation(index: 1, aut_data: auto_1)
        
    }
    
    // Enable / Disable Channel:
    func enable_ch(){
        
        if(enable_channel.isOn){ // Enable View
        
        sensor_selector.isOpaque = false
        sensor_selector.isUserInteractionEnabled = true
        
        
        
        }
        
        else{
        
        
        }
        
    
    }





//MARK: Keyboard Controller





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
    
    // Picker Changed
    func pickerView(_ pickerView: UIPickerView, didSelectRow row: Int, inComponent component: Int) {
        
        update_view()

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
