//
//  MemView.swift
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/15/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

import Cocoa

class MemView: NSTextField {

    override func draw(_ dirtyRect: NSRect) {
        super.draw(dirtyRect)

        // Drawing code here.
    }
    
    override func mouseUp(with event: NSEvent) {
        super.mouseUp(with: event)
        
    }
    
    var wantsSelectAll = false
    
    
    
    override func becomeFirstResponder() -> Bool {
        wantsSelectAll = true
        
        return true; //super.becomeFirstResponder()
    }
    
    override func mouseDown(with event: NSEvent) {
        super.mouseDown(with: event)
        
        print("POINT: ", self.selectedCell()?.accessibilityActivationPoint())
        
        if wantsSelectAll {
            //selectText(self)
            wantsSelectAll = false
        }
    }
}
