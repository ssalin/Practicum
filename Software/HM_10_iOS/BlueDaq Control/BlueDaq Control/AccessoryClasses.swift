//
//  AccessoryClasses.swift
//  BlueDaq Control
//
//  Created by shmandy on 11/21/16.
//  Copyright © 2016 Team 5. All rights reserved.
//

import Foundation

class bluedaq_settings {

// Prefs:
struct prefs {
    var auth : Bool  = false               // Is Authenticated (Not Stored)
    var timestamp : String  = ""        // Last time prefs were updated
    var last_UUID : String  = ""    // Previous Device UUID
    var passcode : String   = ""        // Current Passcode (Not Stored)
    var last_device_name : String = ""  // Previous Device Name
    var automation_0 : Bool = false          // Is automation enabled for this device
    var automation_1 : Bool = false          // Is automation enabled for this device
    
    /*
    init(){
        auth = false
        timestamp = ""
        last_UUID = ""
        passcode = ""
        last_device_name = ""
        automation_0 = false
        automation_1 = false
    }
    */
    
}

// Automation:
struct auto {

    var enabled : Bool      // Enable
    var descending : Bool   // Descending Value
    var toggle : Bool       // Toggle Relay (shut off after duration)
    var tmp_sel : Bool      // Select Temp Sensor
    var pres_sel : Bool     // Select Presure Sensor
    var hum_sel : Bool      // Select Humidity Sensor
    var photo_sel : Bool    // Select Photocell
    var pir_sel : Bool      // Select Motion Sensor
    var setpoint : Float32  // Setpoint or threshold..
    var duraton : Int16     // Durration (est..)
    
    /*
    init(){
        enabled = false
        descending = false
        toggle = false
        tmp_sel = false
        pres_sel = false
        hum_sel = false
        photo_sel = false
        pir_sel = false
        setpoint = 0
        duraton = 0
    }
    */
}


// Timestamp:
func timestamp() -> String {
    
    // Get Date
    let date = NSDate()
    let calendar = NSCalendar.current
    let components = calendar.dateComponents([.year, .month, .day, .hour, .minute, .second], from: date as Date)
    let current_time = NSCalendar.current.date(from: components)!
    
    // Format
    let formatter = DateFormatter()
    formatter.dateStyle = .short
    formatter.timeStyle = .medium
    
    // Return:
    return formatter.string(from: current_time)
}

// Save user settings
func save_defaults(prefs : bluedaq_settings.prefs){
    
    let defaults = UserDefaults.standard

    defaults.set(timestamp(), forKey: "timestamp")
    defaults.set(prefs.auth, forKey: "last_UUID")
    defaults.set(prefs.last_device_name, forKey: "last_dname")
    defaults.set(prefs.automation_0, forKey: "automation_0")
    defaults.set(prefs.automation_1, forKey: "automation_1")
}


// Load user settings
func load_defaults() -> bluedaq_settings.prefs {
    
    let defaults = UserDefaults.standard
    
    let l_prefs = bluedaq_settings.prefs(
    auth: false,    // Not Loaded
    timestamp : defaults.string(forKey: "timestamp") ?? "",
    last_UUID : defaults.string(forKey: "last_UUID") ?? "",
    passcode : "",   // Not Loaded
    last_device_name : defaults.string(forKey: "last_dname") ?? "",
    automation_0: defaults.bool(forKey: "automation_0"),
    automation_1: defaults.bool(forKey: "automation_1")
    )
    
    return l_prefs
}

// Save Automation
// Select Value with Index
func save_automation(index : Int, aut_data : bluedaq_settings.auto){
    
    let defaults = UserDefaults.standard
    
    let key = "automation_ \(index)"
    defaults.set(aut_data, forKey: key)

}

// Save Automation
// Select Value with Index
func load_automation(index : Int) -> bluedaq_settings.auto{

        let defaults = UserDefaults.standard
    
        let key = "automation_ \(index)"
        if let auto_data = defaults.object(forKey: key){

            return auto_data  as! bluedaq_settings.auto
        }
    
        let auto_data = bluedaq_settings.auto(enabled : false, descending : false, toggle : false, tmp_sel : false, pres_sel : false, hum_sel : false, photo_sel : false, pir_sel : false, setpoint : 0, duraton : 0)
        return auto_data
}

}
