//
//  EmuVC.swift
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/8/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

import Cocoa

class EmuVC: ViewController {

    @IBOutlet weak var emuImage: NSImageView!
    
    @IBOutlet var emuView: EmuView!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        emuImage.imageScaling = NSImageScaling.scaleProportionallyUpOrDown
        
        Timer.scheduledTimer(withTimeInterval: TimeInterval(0.03), repeats: true, block: {_ in self.onDraw()})
        
    }
    
    func onDraw() {
        emuView.setNeedsDisplay(NSRect(x: 0, y: 0, width: 240, height: 256))
    }
    
}
