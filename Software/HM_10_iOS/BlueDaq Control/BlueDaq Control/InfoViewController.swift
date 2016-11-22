//
//  BluDaq Sensor View Controller
//
//

import UIKit
import CoreBluetooth
import QuartzCore
import Foundation


final class InfoViewController: UIViewController, BluetoothSerialDelegate {

//MARK: Variables

    let serial_core = bludaq_core_serial()
    var navigationBarAppearace = UINavigationBar.appearance()
    var current_settings = bluedaq_settings.prefs(auth : false, timestamp : "", last_UUID : "", passcode : "", last_device_name : "", automation_0 : false, automation_1 : false)
    
//MARK: IBOutlets
    
    @IBOutlet weak var device_label: UILabel!
    @IBOutlet weak var UUID_Label: UILabel!
    @IBOutlet weak var time_label: UILabel!


//MARK: Functions
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // Connect Serial
        serial.delegate = self
        

        if serial.centralManager.state != .poweredOn {
            title = "Bluetooth Disabled"
            return
        }
        
        // Update Labels:
        device_label.text = current_settings.last_device_name
        UUID_Label.text = current_settings.last_UUID
        time_label.text = current_settings.timestamp
        
        
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
    
    
    
    
//MARK: IBActions


}
