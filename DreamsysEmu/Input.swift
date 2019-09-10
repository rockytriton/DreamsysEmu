//
//  Input.swift
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/9/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

import Foundation

import IOKit
import IOKit.usb
import IOKit.hid


var devices : CFSet? = nil

var valueCallback : IOHIDValueCallback = {
    (context, result, sender, value) in
    
    let element = IOHIDValueGetElement(value)
    let cookie = IOHIDElementGetCookie(element)
    let code = IOHIDValueGetIntegerValue(value)
    
    var event : CGEvent?
    
    //controller_set(0, UInt8(BUTTON_ST), 0);
    
    switch cookie {
    case 7:     /* Button 1 */
        break;
    //event = generateMediaKeyEvent(key: NX_KEYTYPE_SOUND_DOWN, down: code == 1)
    case 3:     /* Button 2 */
        controller_set(0, UInt8(BUTTON_B), UInt8(code));
    //event = generateMediaKeyEvent(key: NX_KEYTYPE_PREVIOUS, down: code == 1)
    case 4:     /* Button 3 */
        controller_set(0, UInt8(BUTTON_A), UInt8(code));
    //event = generateMediaKeyEvent(key: NX_KEYTYPE_PLAY, down: code == 1)
    case 10:     /* Button 4 */
        controller_set(0, UInt8(BUTTON_SE), UInt8(code));
    //event = generateMediaKeyEvent(key: NX_KEYTYPE_NEXT, down: code == 1)
    case 11:     /* Button 5 */
        controller_set(0, UInt8(BUTTON_ST), UInt8(code));
    //event = generateMediaKeyEvent(key: NX_KEYTYPE_SOUND_UP, down: code == 1)
    case 16:    /* Jog Dial */
        if (code == 0) {
            controller_set(0, UInt8(BUTTON_U), UInt8(1));
            controller_set(0, UInt8(BUTTON_R), UInt8(0));
            controller_set(0, UInt8(BUTTON_D), UInt8(0));
            controller_set(0, UInt8(BUTTON_L), UInt8(0));
        } else if (code == 1) {
            controller_set(0, UInt8(BUTTON_U), UInt8(1));
            controller_set(0, UInt8(BUTTON_R), UInt8(1));
            controller_set(0, UInt8(BUTTON_D), UInt8(0));
            controller_set(0, UInt8(BUTTON_L), UInt8(0));
        } else if (code == 2) {
            controller_set(0, UInt8(BUTTON_U), UInt8(0));
            controller_set(0, UInt8(BUTTON_R), UInt8(1));
            controller_set(0, UInt8(BUTTON_D), UInt8(0));
            controller_set(0, UInt8(BUTTON_L), UInt8(0));
        } else if (code == 3) {
            controller_set(0, UInt8(BUTTON_U), UInt8(0));
            controller_set(0, UInt8(BUTTON_R), UInt8(1));
            controller_set(0, UInt8(BUTTON_D), UInt8(1));
            controller_set(0, UInt8(BUTTON_L), UInt8(0));
        } else if (code == 4) {
            controller_set(0, UInt8(BUTTON_U), UInt8(0));
            controller_set(0, UInt8(BUTTON_R), UInt8(0));
            controller_set(0, UInt8(BUTTON_D), UInt8(1));
            controller_set(0, UInt8(BUTTON_L), UInt8(0));
        } else if (code == 5) {
            controller_set(0, UInt8(BUTTON_U), UInt8(0));
            controller_set(0, UInt8(BUTTON_R), UInt8(0));
            controller_set(0, UInt8(BUTTON_D), UInt8(1));
            controller_set(0, UInt8(BUTTON_L), UInt8(1));
        } else if (code == 6) {
            controller_set(0, UInt8(BUTTON_U), UInt8(0));
            controller_set(0, UInt8(BUTTON_R), UInt8(0));
            controller_set(0, UInt8(BUTTON_D), UInt8(0));
            controller_set(0, UInt8(BUTTON_L), UInt8(1));
        } else if (code == 15) {
            controller_set(0, UInt8(BUTTON_U), UInt8(0));
            controller_set(0, UInt8(BUTTON_R), UInt8(0));
            controller_set(0, UInt8(BUTTON_D), UInt8(0));
            controller_set(0, UInt8(BUTTON_L), UInt8(0));
        }
        event = nil
    case 17:    /* Wheel */
        event = nil
    default:
        //print("Unknown element: ", cookie)
        event = nil
    }
    
    if (event != nil) {
        //event!.post(tap:.cgSessionEventTap)
    }
    //CFRunLoopRun();
}


func deviceAdded(_ inContext: UnsafeMutableRawPointer?, inResult: IOReturn, inSender: UnsafeMutableRawPointer?, deviceRef: IOHIDDevice!) {
    
    if IOHIDDeviceConformsTo(deviceRef, UInt32(kHIDPage_GenericDesktop), UInt32(kHIDUsage_GD_GamePad)) {
        
        print("gamepad added!")
        
        
        print("adding cb")
        IOHIDDeviceRegisterInputValueCallback(deviceRef, valueCallback, nil)
        
        var ret = IOHIDDeviceOpen(deviceRef, IOOptionBits(kIOHIDOptionsTypeSeizeDevice))
        IOHIDDeviceScheduleWithRunLoop(deviceRef, CFRunLoopGetCurrent(), CFRunLoopMode.defaultMode.rawValue)
        
        print("scheduled")
        CFRunLoopRun();
        
    }
    else if IOHIDDeviceConformsTo(deviceRef, UInt32(kHIDPage_GenericDesktop), UInt32(kHIDUsage_GD_Keyboard)) || IOHIDDeviceConformsTo(deviceRef, UInt32(kHIDPage_GenericDesktop), UInt32(kHIDUsage_GD_Keypad)) {
        
        print("keyboard added!")
        
        //IOHIDDeviceRegisterInputValueCallback(deviceRef, keyboardEvent, nil)
        
    }
    else {
        print("unknown device added")
    }
    
}

func deviceRemoved(_ inContext: UnsafeMutableRawPointer?, inResult: IOReturn, inSender: UnsafeMutableRawPointer?, inIOHIDDeviceRef: IOHIDDevice!) {
    
    var gpIdx = 0

    
    // Unregister value callback
    IOHIDDeviceRegisterInputValueCallback(inIOHIDDeviceRef, nil, nil)
    
}

func createDeviceMatchingDictionary( usagePage: Int, usage: Int) -> CFMutableDictionary {
    let dict = [
        kIOHIDDeviceUsageKey: usage,
        kIOHIDDeviceUsagePageKey: usagePage
        ] as NSDictionary
    
    return dict.mutableCopy() as! NSMutableDictionary;
}

func InitGamepad() {
    var worker2 = DispatchQueue(label: "gp");
    worker2.async {
        
        let manager = IOHIDManagerCreate(kCFAllocatorDefault, IOOptionBits(kIOHIDOptionsTypeNone));
        let keyboard = createDeviceMatchingDictionary(usagePage: kHIDPage_GenericDesktop, usage: kHIDUsage_GD_GamePad)
        
        IOHIDManagerOpen(manager, IOOptionBits(kIOHIDOptionsTypeNone) )
        IOHIDManagerSetDeviceMatching(manager, keyboard)
        
        let Handle_DeviceMatchingCallback: IOHIDDeviceCallback = {context, result, sender, device in
        }
        let Handle_DeviceRemovalCallback: IOHIDDeviceCallback = {context, result, sender, device in
            print("Disconnected")
        }
        
        devices = IOHIDManagerCopyDevices(manager)
        
        if (devices != nil) {
            print("Found devices!", devices!)
            let n = CFSetGetCount(devices!)
            let array = UnsafeMutablePointer<UnsafeRawPointer?>.allocate(capacity: n)
            array.initialize(repeating: nil, count: n)
            CFSetGetValues(devices!, array);
            
            IOHIDManagerRegisterDeviceMatchingCallback(manager, deviceAdded, nil)
            IOHIDManagerRegisterDeviceRemovalCallback(manager, deviceRemoved, nil)
            
            
            IOHIDManagerScheduleWithRunLoop(manager, CFRunLoopGetCurrent(),
                                            CFRunLoopMode.defaultMode.rawValue)
            IOHIDManagerOpen(manager, IOOptionBits(kIOHIDOptionsTypeNone))
            
            CFRunLoopRun()
        }
        else {
            print("Did not find any devices :(")
        }
        
        while(true) {
            print("RUNNING LOOP")
            CFRunLoopRun()
            usleep(500000);
            print("RUN DONE")
        }
    }
}

