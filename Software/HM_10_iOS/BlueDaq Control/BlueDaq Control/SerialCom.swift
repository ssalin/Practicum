//
//  SerialCom.swift
//  BlueDaq Control
//
//  Created by shmandy on 11/20/16.
//  Copyright © 2016 Team 5. All rights reserved.
//

import Foundation

class bludaq_core_serial{
    
    /*
    *   Message Formation:
    *
    *   TYPE:INFO <key:value> Pair
    *   Will be followed by newline or null '\0' sym
    *   Use this class as Method in each ViewController active (new)
    */
    
    // Class Variables:
    
    var auth_set : Bool = false
    
    // Types of callback funtion reqeusts (for parser)
    enum transaction_req{
        case Hello, Authenticate, Automation, Data
    }
    
    // Returned from parser:
    struct transaction_resp{
        var good_msg:   Bool            // Wass message parsed correctly?
        var message: String             // Complete Message
        var msg_type: Int               // Messag type (from Device)
        var t_type: transaction_req     // Request type (from caller)
        var body: String                // Rest of Message
    }
    
 
    // Recieved Message Types:
    var msg_types: [String:Int] = [
        "UNKO" : -1,    // Unknown Message
        "HELO=" : 0,     // Device Booted
        "AUTH=" : 1,     // Authentication State
        "SLEP=" : 2,     // Device Sleeping
        "EROR=" : 3,     // Device Error Msg
        "DTMP=" : 4,     // Data: Temperature
        "DHUM=" : 5,     // Data: Humidity
        "DPRS=" : 6,     // Data: Pressure
        "DRLS=" : 7,     // Data: Photoresistor
        "DPIR=" : 8,     // Data: PIR State (Motion T/F)
        "DRL0=" : 9,     // Data: Relay 0 State
        "DRL1=" : 10,     // Data: Relay 1 State
    ]
    
    
    // Recieved Message (text) bodies:
    var body_types: [Int:String] = [

        0 : "BADC",     // Bad Command or Value
        1 : "FALS",     // Logical False
        2 : "TRUE",     // Logical True
        3 : "ERRR"      // Unknown Error with Vaue (default)
    ]
    
    
    // Split message string into key : value
    func parse_message(message: String, t_type: transaction_req, completion: (_ t_resp: transaction_resp) -> Void) -> Bool{
        
        // Prase out Message Type:
        let msg_keys = [String](msg_types.keys)
        
        for (key) in msg_keys {
            if message.uppercased().range(of: key) != nil {
                
                // Find "=" index for substring:
                let chars = message.characters;
                let index = chars.index(of: "=")!
                
                // Populate Struct:
                let t_data = transaction_resp(
                    good_msg: true,
                    message: message,
                    msg_type: msg_types[key]!,
                    t_type: t_type,
                    body: message.substring(from: index))
                
                // Execte Callback:
                completion(t_data)
                return true
                
            }
        }
        
        // Populate Struct:
        let t_data = transaction_resp(
        good_msg: false,
        message: message,
        msg_type: msg_types["UNKO"]!,
        t_type: t_type,
        body: message)
                
        // Execte Callback:
        completion(t_data)
        return false
        
    } // END PARSER //
    
    
    
    
    // Special Parsers: Parse and cast to proper data type for display
    // Do any nesssiary corrections or math here...
    
    // Temperature
    func parse_temp(msg_body: String) -> Float{
    
        return 0
    }
    
    // Humidity
    func parse_humidity(msg_body: String) -> Float{
    
        return 0
    }
    
    // Pressure
    func parse_pressure(msg_body: String) -> Float{
    
        return 0
    }
    
    // Photodiode
    func parse_photo(msg_bod: String) -> Int {
    
        return 0
    
    }
    
    
    
}
