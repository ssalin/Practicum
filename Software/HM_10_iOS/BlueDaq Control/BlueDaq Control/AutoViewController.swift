//
//  BluDaq Automation View Controller
//
//

import UIKit
import CoreBluetooth
import QuartzCore
import Foundation



final class AutoViewController: UIViewController, BluetoothSerialDelegate {

//MARK: Variables

    var message_buffer : String = ""                // Global Message Buffer
    let serial_core = bludaq_core_serial()          // Serial Message Functions
    let accesories = bluedaq_settings()             // App Settings Class
    var current_settings = bluedaq_settings.prefs(init_ : true, auth : false, timestamp : "", last_UUID : "", passcode : "", last_device_name : "", automation_0 : false, automation_1 : false)



//MARK: IBOutlets
    
    



//MARK: Functions
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // Connect Serial
        serial.delegate = self
        
        if serial.centralManager.state != .poweredOn {
            title = "Bluetooth Disabled"
            return
        }
    }
    
    
    deinit {
        NotificationCenter.default.removeObserver(self)
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
	
	// Unwind to Sensor View
    if segue.identifier == "unwindToSensor" {
        let controller = segue.destination as! InitViewController
        controller.current_settings = current_settings // Copy Settings
    }
    
}
    
    
//MARK: IBActions


}
