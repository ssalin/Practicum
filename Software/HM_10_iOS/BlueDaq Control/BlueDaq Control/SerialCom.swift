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

    
    // Returned from parser:
    struct transaction_resp{
        var good_msg:   Bool            // Wass message parsed correctly?
        var message: String             // Complete Message
        var msg_type: Int               // Messag type (from Device)
        var body: String                // Rest of Message
    }
    
 
    // Recieved Message Types (RX):
    var rx_msg_types: [String:Int] = [
        "UNKO=" : -1,    // Unknown Message
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
        "DRL1=" : 10,    // Data: Relay 1 State
    ]
    
    // Transmit Message Types (TX):
    var tx_msg_types: [Int: String] = [
    
        0: "AUTN=",    // Automation Channel Select = <Channel>
        1: "AUTF=",    // Automation Flag Set = <Byte>
        2: "AUTS=",    // Automation Setupoint = <Float> (4-byte)
        3: "AUTD=",    // Automation Duration = <Int> (2-byte)
        4: "AUTC=",    // Automation Complete = <Channel> (for confirmation)
        5: "CPOL=",    // Configure Polling Frequency = <byte>
        6: "DATA=",    // Perform Data Operations (Read Sensors)
        7: "STAT=",    // Ask for status
        8: "AUTK=",    // Send Auth Key = <KEY>
        9: "AUTG=",    // Enable Toggle <T/F>
        10: "AUTE=",    // Decending Value Enabled
        11: "EEPW="    // Write to EEPROM
    ]
    
    
    // Message Bodies (TX/RX)
    var body_types: [Int:String] = [

        0 : "BADC",     // Bad Command or Value
        1 : "FALS",     // Logical False
        2 : "TRUE",     // Logical True
        3 : "ERRR",     // Unknown Error (not caught)
        4 : "NOAU",     // No Access (bad auth)
        5 : "STRT",     // Begin Operation
        6 : "ENDD",     // End Operation
        7 : "AUTS",     // Automation Status
        8 : "DATS",     // Data Status
        9 : "ATHS"      // Auth Status
    ]
	
	
	// Non-Callback Parser:
    
    // Need to fix ugly strings like "34.08/nDATA=20" Somehow this makes it in the body...
	func parse_message(message: String) -> bludaq_core_serial.transaction_resp{
        
        // Prase out Message Type:
        let msg_keys = [String](rx_msg_types.keys)
        let msg = message.trimmingCharacters(in: CharacterSet.whitespacesAndNewlines)
    
        for (key) in msg_keys {
            
            if msg.uppercased().range(of: key) != nil {
                
                // Find "=" index for substring:
                let chars = msg.characters;
                let index = chars.index(of: "=")!
                var body = msg.substring(from: index)
                body.remove(at: msg.startIndex)
                body = body.trimmingCharacters(in: CharacterSet.whitespacesAndNewlines)
                
                // Remove Overflow
                if let new_line = body.characters.index(of: " "){
                    body = String(body.characters.prefix(upTo: new_line))
                }
                
                // Populate Struct:
                let t_data = transaction_resp(
                    good_msg: true,
                    message: msg,
                    msg_type: rx_msg_types[key]!,
                    body: body)
                
                // Execte Callback:
                return t_data
                
            }
        }
        
        // Populate Struct:
        let t_data = transaction_resp(
        good_msg: false,
        message: msg,
        msg_type: rx_msg_types["UNKO="]!,
        body: "ERRR")
		
        return t_data
		
    } // END PARSER //



    
    // Special Parsers: Parse and cast to proper data type for display
    // Do any nesssiary corrections or math here...
    
    // Parse Body for a Float
    func parse_float(msg_body: String) -> Float{
    
        return Float(msg_body)!
    }
    
    
    // Parse Body for an Int
    func parse_int(msg_body : String) -> Int {
    
        return Int(msg_body)!
    }
    
    
    // Parse Body for Bool
    func parse_bool(msg_body : String) -> Bool {
        
        if((msg_body.uppercased().range(of: body_types[2]!)) != nil){ // Check True
            return true
        }
        
        return false
    
    }
    
    
    // TX Functions:
    
    func send_key(msg_body: String) -> String{
        
        return tx_msg_types[8]! + msg_body + "\n"
    }
    
    // Start or Stop Data Upload Process
    func data_read(state : Bool) -> String{
        
        if(state){      // Start
            return (String(tx_msg_types[6]! + body_types[5]! + "\n")) // Start
        
        }
        // Stop
        return (String(tx_msg_types[6]! + body_types[6]! + "\n")) // Start

    }
    
    
    
}


    /*
    
    // Split message string into key : value
    func parse_message(message: String, t_type: transaction_req, completion: (_ t_resp: transaction_resp) -> Void) -> Bool{
        
        // Prase out Message Type:
        let msg_keys = [String](rx_msg_types.keys)
        
        for (key) in msg_keys {
            if message.uppercased().range(of: key) != nil {
                
                // Find "=" index for substring:
                let chars = message.characters;
                let index = chars.index(of: "=")!
                var body = message.substring(from: index)
                body.remove(at: message.startIndex)
                body = body.trimmingCharacters(in: CharacterSet.whitespacesAndNewlines)
                
                // Populate Struct:
                let t_data = transaction_resp(
                    good_msg: true,
                    message: message,
                    msg_type: rx_msg_types[key]!,
                    t_type: t_type,
                    body: body)
                
                // Execte Callback:
                completion(t_data)
                return true
                
            }
        }
        
        // Populate Struct:
        let t_data = transaction_resp(
        good_msg: false,
        message: message,
        msg_type: rx_msg_types["UNKO"]!,
        t_type: t_type,
        body: message)
                
        // Execte Callback:
        completion(t_data)
        return false
        
    } // END PARSER //
    
	*/
