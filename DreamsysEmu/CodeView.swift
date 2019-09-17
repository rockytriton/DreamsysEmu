//
//  CodeView.swift
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/13/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

import Cocoa

class CodeView: NSView, NSTextFieldDelegate {

    @IBOutlet weak var lblMem: NSTextField!
    
    @IBOutlet weak var txtAddr: NSTextField!
    @IBOutlet weak var txtValue: NSTextField!
    var bytesPerLine = 16;
    var lines = 20;
    var page = 0;
    
    override func viewDidMoveToWindow() {
    }
    
    required init(coder: NSCoder) {
        super.init(coder: coder)!
    }
    
    @IBAction func onSetValue(_ sender: Any) {
        var addr = Int(txtAddr.stringValue, radix: 16)
        var val = Int(txtValue.stringValue, radix: 16)
        
        emu_read_ram()![addr!] = UInt8(val!);
    }
    
    func control(_ control: NSControl, textShouldBeginEditing fieldEditor: NSText) -> Bool {
        print("SHOULD I?")
        
        return false
    }
    
    @objc func textDidChange(_ notification: Notification) {
        
    }
    
    override func draw(_ dirtyRect: NSRect) {
        super.draw(dirtyRect)
    }
    
    @IBAction func onPreviousPage(_ sender: Any) {
        page -= bytesPerLine * (lines - 1);
        
        if (page < 0) {
            page = 0;
        }
    }
    
    @IBAction func onLblAction(_ sender: Any) {
        print("LBL ACTION: ")
    }
    
    @IBAction func onNextPage(_ sender: Any) {
        page += bytesPerLine * (lines - 1);
        print("NEXT PAGE");
        
        if (page > (0x800) - (bytesPerLine * (lines - 1))) {
            page = 0x800 - (bytesPerLine * lines);
        }
    }
    
    public func onUpdate() {
        
        if (emu_running() == 0) {
            return;
        }
        
        var p = emu_read_ram();
        
        var str = ""
        var line = 0;
        var strAscii = ""
        
        for var i in (page)..<0x800 {
            
            if ((i % bytesPerLine) == 0 && i != page) {
                str += " " + strAscii
                strAscii = ""
                str += "\r\n"
                line += 1;
                
                if (line >= lines) {
                    break;
                }
            }
            
            if ((i % bytesPerLine) == 0) {
                str += String(format: "%0.4X  ", (line * bytesPerLine) + (page));
                
            }
            
            var b = p![i]
            
            str += String(format:"%02X ", b)
            
            if (b >= 0x20 && b <= 0x7E) {
                strAscii += String(format: "%c", b);
            } else {
                strAscii += "?"
            }
        }
        
        lblMem.stringValue = str + " " + strAscii;
    }
    
}
