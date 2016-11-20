//
//  SerialViewController.swift
//  HM10 Serial
//
//  Created by Alex on 10-08-15.
//  Copyright (c) 2015 Balancing Rock. All rights reserved.
//

import UIKit
import CoreBluetooth
import QuartzCore



final class SensorViewController: UIViewController, BluetoothSerialDelegate {

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
    
    
    
    
//MARK: IBActions

    @IBAction func connect_dev(_ sender: AnyObject) {
        if serial.connectedPeripheral == nil {
            performSegue(withIdentifier: "ShowScanner", sender: self)
        } else {
            serial.disconnect()
        }
    }
}
