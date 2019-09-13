//
//  CodeView.swift
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/13/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

import Cocoa

class CodeView: NSView {

    @IBOutlet weak var lblMem: NSTextField!
    
    override func draw(_ dirtyRect: NSRect) {
        super.draw(dirtyRect)

    }
    
    public func onUpdate() {
        
        if (emu_running() == 0) {
            return;
        }
        
        var p = emu_read_ram();
        
        var str = ""
        
        for var i in 0..<0x400 {
            var b = p![i]
            
            str += String(format:"%02X ", b)
            
            if ((i % 32) == 0 && i != 0) {
                str += "\r\n"
            }
        }
        
        lblMem.stringValue = str;
    }
    
}
