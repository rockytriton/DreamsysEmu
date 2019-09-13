//
//  CodeViewVC.swift
//  RockEmu
//
//  Created by Rocky Pulley on 8/29/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

import Cocoa

class CodeViewVC: NSViewController {
    
    @IBOutlet var codeView: CodeView!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do view setup here.
        
        Timer.scheduledTimer(withTimeInterval: TimeInterval(0.03), repeats: true, block: {_ in self.codeView.onUpdate()})
    }
    
    func onUpdate() {
        
    }
}
