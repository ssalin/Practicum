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

    var pickerData = [["Temperature", "Pressure", "Humidity", "Light Level", "Motion Sensor"]]
    var message_buffer : String = ""                // Global Message Buffer
    let serial_core = bludaq_core_serial()          // Serial Message Functions
    let accesories = bluedaq_settings()             // App Settings Class
    var current_settings = bluedaq_settings.prefs()
    var loaded_auto = true
    var selected_chan = 0
    var automation_dat = [bluedaq_settings.auto(), bluedaq_settings.auto()]

    

//MARK: IBOutlets
    
    @IBOutlet weak var title_label: UILabel!
    @IBOutlet weak var toggle_label: UILabel!
    @IBOutlet weak var thresh_label: UILabel!
    @IBOutlet weak var automation_label: UILabel!
    @IBOutlet weak var toggle_enabled_label: UILabel!
    @IBOutlet weak var sensor_selector: UIPickerView!
    @IBOutlet weak var threshold_txtbox: UITextField!
    @IBOutlet weak var duration_txtbox: UITextField!
    @IBOutlet weak var asc_switch: UISwitch!
    @IBOutlet weak var asc_label: UILabel!
    @IBOutlet weak var chan_select: UISegmentedControl!
    @IBOutlet weak var toggle_switch: UISwitch!
    @IBOutlet weak var enable_button: UIButton!
    

    
//MARK: Functions
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // Connect Serial
        serial.delegate = self
        
        if serial.centralManager.state != .poweredOn {
            title = "Bluetooth Disabled"
            return
        }
        
        // Keyboard:
        let tap: UITapGestureRecognizer = UITapGestureRecognizer(target: self, action: #selector(AutoViewController.dismissKeyboard))
        view.addGestureRecognizer(tap)
        
        NotificationCenter.default.addObserver(self, selector: #selector(self.keyboardWillShow), name: NSNotification.Name.UIKeyboardWillShow, object: nil)
        NotificationCenter.default.addObserver(self, selector: #selector(self.keyboardWillHide), name: NSNotification.Name.UIKeyboardWillHide, object: nil)
        
        // Setup Picker
        load_picker()
        
        // Load Automation
        load_automation()

    }
    
    
    deinit {
        NotificationCenter.default.removeObserver(self)
    }
    
    // Hide Keyboard
    func dismissKeyboard() {
        view.endEditing(true)
    }
    
    // Move Screen for Keyboard
     func keyboardWillShow(notification: NSNotification) {

        if let keyboardSize = (notification.userInfo?[UIKeyboardFrameBeginUserInfoKey] as? NSValue)?.cgRectValue {
            if self.view.frame.origin.y == 0{
            self.view.frame.origin.y -= keyboardSize.height
        }
        }

    }

    func keyboardWillHide(notification: NSNotification) {
        if let keyboardSize = (notification.userInfo?[UIKeyboardFrameBeginUserInfoKey] as? NSValue)?.cgRectValue {
            if self.view.frame.origin.y != 0{
            self.view.frame.origin.y += keyboardSize.height
        }
        }
    }
    
    // Process changes and update view
    func update_view(){

        // Channel State
        if(automation_dat[selected_chan].enabled){
            enable_button.setTitle("Enabled", for: .normal)
            enable_button.setTitleColor(UIColor.blue, for: .normal)
            sensor_selector.isUserInteractionEnabled = true
             sensor_selector.alpha = 1
            title_label.isEnabled = true
            toggle_switch.isEnabled = true

        }
        else{
            enable_button.setTitle("Disabled", for: .normal)
            enable_button.setTitleColor(UIColor.red, for: .normal)
            sensor_selector.isUserInteractionEnabled = false
            sensor_selector.alpha = 0.25
            title_label.isEnabled = false
            toggle_switch.isEnabled = false
        }
        
        
        // Select Label
        thresh_label.text = "When the \(getDevice()) is at or"
        
        // Toggle Switch
        if(toggle_switch.isOn && automation_dat[selected_chan].enabled){
            toggle_enabled_label.text = "Toggle Enabled"
            duration_txtbox.isEnabled = true
            toggle_label.isEnabled = true
        }
        else{
            toggle_enabled_label.text = "Toggle Disabled"
            duration_txtbox.isEnabled = false
            toggle_label.isEnabled = false
        
        }
        
        // Set Switches:

        asc_switch.setOn(automation_dat[selected_chan].descending, animated: true)
        toggle_switch.setOn(automation_dat[selected_chan].toggle, animated: true)
        
        
        // Ascending Switch
        if asc_switch.isOn == true{ // Above
            asc_label.text = "above"
        }
        else{
            asc_label.text = "below"
        }
        
        // Motion Sensor Disables Options
        if(getDevice() == "Motion Sensor" || !automation_dat[selected_chan].enabled){
            thresh_label.isEnabled = false
            asc_label.isEnabled = false
            asc_switch.isOpaque = true
            asc_switch.isEnabled = false
            threshold_txtbox.isEnabled = false
            threshold_txtbox.isOpaque = true
        }
        else{
            thresh_label.isEnabled = true
            asc_label.isEnabled = true
            asc_switch.isOpaque = false
            asc_switch.isEnabled = true
            threshold_txtbox.isEnabled = true
            threshold_txtbox.isOpaque = false
        }
        
        
        // Update Picker
        sensor_selector.selectRow(automation_dat[selected_chan].sensor.rawValue , inComponent: 0, animated: true)  // Set Picker
        
        // Update TextBox
        threshold_txtbox.text = String(automation_dat[selected_chan].setpoint)
        duration_txtbox.text = String(automation_dat[selected_chan].duraton)
        
        
        // Save Changes:
        accesories.save_automation(index: selected_chan, aut_data: automation_dat[selected_chan])

    }
    
    
    
    // Load Automation from User Prefs
    func load_automation(){
        
        // Get Data:
        if(loaded_auto){
            automation_dat[0] = accesories.load_automation(index: 0)
            automation_dat[1] = accesories.load_automation(index: 1)
            
            loaded_auto = false
            
            update_view()
        }
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
    
    // Picker Changed
    func pickerView(_ pickerView: UIPickerView, didSelectRow row: Int, inComponent component: Int) {
        
        // Update Data:
        automation_dat[selected_chan].sensor = bluedaq_settings.sensor_sel(rawValue: sensor_selector.selectedRow(inComponent: 0))!
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
    
    // Channel Enabled / Disabled
    @IBAction func enable_button(_ sender: Any) {
    
        automation_dat[selected_chan].enabled = !automation_dat[selected_chan].enabled
        update_view()
    }
    
    // Channel Selected
    @IBAction func chan_button(_ sender: Any) {
        // Select Channel:
        selected_chan = chan_select.selectedSegmentIndex
        update_view()
    }
    
    // Ascending Direction Changed
    @IBAction func asc_button(_ sender: Any) {
        
        automation_dat[selected_chan].descending = asc_switch.isOn ? true : false
        update_view()
    }
    
    // Threshold Value Changed
    @IBAction func txtbox_button(_ sender: Any) {
        automation_dat[selected_chan].setpoint = Float(threshold_txtbox.text!)!
        update_view()
    }
    
    // Toggle Setting Changed
    @IBAction func toggle_switch(_ sender: Any) {
        automation_dat[selected_chan].toggle = toggle_switch.isOn ? true : false
        update_view()
    }
    
    // Duration Value Changed
    @IBAction func duration_txt_button(_ sender: Any) {
        
        automation_dat[selected_chan].duraton = Int(duration_txtbox.text!)!
        update_view()
    }



//MARK: Segue Prep
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
	
        // Unwind to Sensor View
        if segue.identifier == "unwindToSensor" {
            let controller = segue.destination as! InitViewController
            controller.current_settings = current_settings // Copy Settings
        }
    
    }


}
