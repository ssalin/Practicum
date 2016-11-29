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
	var init_ : Bool = true
    var auth : Bool  = false            // Is Authenticated (Not Stored)
    var timestamp : String  = ""		// Last time prefs were updated
    var last_UUID : String  = ""		// Previous Device UUID
    var passcode : String   = ""        // Current Passcode (Not Stored)
    var last_device_name : String = ""  // Previous Device Name
    var automation_0 : Bool = false     // Is automation enabled for this device
    var automation_1 : Bool = false     // Is automation enabled for this device
}

// Sensor Enumeration:
enum sensor_sel : Int {
    case TEMP, PRES, HUMI, LIGHT, PIR
}


// Automation:
struct auto {

    var enabled : Bool = false      // Enable
    var descending : Bool = false   // Descending Value
    var toggle : Bool = false       // Toggle Relay (shut off after duration)
    var sensor : sensor_sel = sensor_sel.TEMP
    var setpoint : Float = 0      // Setpoint or threshold..
    var duraton : Int = 0         // Duration (est..)
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
        init_ : true,	// Not Loaded
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

    defaults.set(aut_data.descending, forKey: "auto_\(index)_desc")
    defaults.set(aut_data.duraton, forKey: "auto_\(index)_dur")
    defaults.set(aut_data.enabled, forKey: "auto_\(index)_en")
    defaults.set(aut_data.sensor.rawValue, forKey: "auto_\(index)_sen")
    defaults.set(aut_data.setpoint, forKey: "auto_\(index)_set")
    defaults.set(aut_data.toggle, forKey: "auto_\(index)_tog")
    
}

// Save Automation
// Select Value with Index
func load_automation(index : Int) -> bluedaq_settings.auto{
    
        
        let defaults = UserDefaults.standard
    
        let auto_data = bluedaq_settings.auto(
            enabled: defaults.bool(forKey: "auto_\(index)_en"),
            descending: defaults.bool(forKey: "auto_\(index)_desc"),
            toggle: defaults.bool(forKey: "auto_\(index)_tog"),
            sensor: bluedaq_settings.sensor_sel(rawValue: defaults.integer(forKey: "auto_\(index)_sen"))!,
            setpoint: defaults.float(forKey: "auto_\(index)_set"),
            duraton: defaults.integer(forKey: "auto_\(index)_dur")
        )

        return auto_data
}

}
