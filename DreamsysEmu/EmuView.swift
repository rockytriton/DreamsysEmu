//
//  EmuView.swift
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/8/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

import Cocoa

class EmuView: NSView {

    @IBOutlet weak var imgView: NSImageView!
    
    public struct PixelData {
        var a:UInt8 = 255
        var r:UInt8
        var g:UInt8
        var b:UInt8
    }
    
    public static var drawing = false
    
    private let rgbColorSpace = CGColorSpaceCreateDeviceRGB()
    private let bitmapInfo:CGBitmapInfo = CGBitmapInfo(rawValue: CGImageAlphaInfo.premultipliedFirst.rawValue)
    
    var pixelData = [PixelData](repeating: PixelData(a:0,r:0,g:0,b:0), count: 240 * 256);
    
    var imageData = [[Int?]](
        repeating: [Int?](repeating: nil, count: 256),
        count: 240
    )
    
    override var acceptsFirstResponder: Bool {
        return true
    }
    
    public func imageFromARGB32Bitmap(pixels:[PixelData], width:Int, height:Int)->NSImage {
        let bitsPerComponent:Int = 8
        let bitsPerPixel:Int = 32
        
        assert(pixels.count == Int(width * height))
        
        var data = pixels // Copy to mutable []
        let providerRef = CGDataProvider(
            data: NSData(bytes: &data, length: data.count * MemoryLayout.size(ofValue: pixels[0]))
        )
        
        let cgim = CGImage(
            width: width,
            height: height,
            bitsPerComponent: bitsPerComponent,
            bitsPerPixel: bitsPerPixel,
            bytesPerRow: width * Int(MemoryLayout.size(ofValue: pixels[0])),
            space: rgbColorSpace,
            bitmapInfo: bitmapInfo,
            provider: providerRef!,
            decode: nil,
            shouldInterpolate: true,
            intent: .defaultIntent
        )
        
        return NSImage(cgImage: cgim!, size: NSZeroSize)
    }
    
    var lastFrame : UInt32 = 0;
    
    override func draw(_ dirtyRect: NSRect) {
        super.draw(dirtyRect)

        if (emu_running() == 0) {
            return;
        }
        
        if (emu_current_frame() == lastFrame) {
            return;
        }
        
        if (lastFrame != -1) {
        //return;
        }
        
        //updateAudio();
        
        lastFrame = emu_current_frame();
        
        var pb = emu_image_buffer();
        
        for yyy in 0..<240 {
            var ypos = (yyy * 256);
            
            for xxx in 0..<256 {
                //print("y,x = ", yyy, ",", xxx)
                var bt = pb![xxx]
                var n = Int(bt![yyy]);
                //imageData[yyy][xxx] = n
                
                var p = PixelData(a:255, r: UInt8((n & 0xFF000000) >> 24), g: UInt8((n & 0x00FF0000) >> 16), b: UInt8((n & 0x0000FF00) >> 8))
                pixelData[ypos + xxx] = p;
            }
        }
        
        let image = imageFromARGB32Bitmap(pixels: pixelData, width: 256, height: 240)
        imgView.image = image
    }
    
    var keyUp = false;
    var keyDown = false;
    var keyLeft = false;
    var keyRight = false;
    var keyA = false;
    var keyB = false;
    var keySel = false;
    var keyStart = false;
    
    override func keyUp(with event: NSEvent) {
        var kc = event.keyCode
        
        if (kc == 36) {
            controller_set(0, UInt8(BUTTON_ST), 0);
            keyStart = false;
        } else if (kc == 13) {
            controller_set(0, UInt8(BUTTON_U), 0);
            keyUp = false;
        } else if (kc == 1) {
            controller_set(0, UInt8(BUTTON_D), 0);
            keyDown = false;
        } else if (kc == 0) {
            controller_set(0, UInt8(BUTTON_L), 0);
            keyLeft = false;
        } else if (kc == 2) {
            controller_set(0, UInt8(BUTTON_R), 0);
            keyRight = false;
        } else if (kc == 47) {
            controller_set(0, UInt8(BUTTON_B), 0);
            keyB = false;
        } else if (kc == 44) {
            controller_set(0, UInt8(BUTTON_A), 0);
            keyA = false;
        } else if (kc == 39) {
            controller_set(0, UInt8(BUTTON_SE), 0);
            keySel = false;
        }
    }
    
    override func keyDown(with event: NSEvent) {
        var kc = event.keyCode
        
        //print("KEY CODE: ", kc);
        
        if (!keyStart && kc == 36) {
            controller_set(0, UInt8(BUTTON_ST), 1);
            keyStart = true
        } else if (!keyUp && kc == 13) {
            controller_set(0, UInt8(BUTTON_U), 1);
            keyUp = true
        } else if (!keyDown && kc == 1) {
            controller_set(0, UInt8(BUTTON_D), 1);
            keyDown = true
        } else if (!keyLeft && kc == 0) {
            controller_set(0, UInt8(BUTTON_L), 1);
            keyLeft = true
        } else if (!keyRight && kc == 2) {
            controller_set(0, UInt8(BUTTON_R), 1);
            keyRight = true
        } else if (!keyB && kc == 47) {
            controller_set(0, UInt8(BUTTON_B), 1);
            keyB = true
        } else if (!keyA && kc == 44) {
            controller_set(0, UInt8(BUTTON_A), 1);
            keyA = true
        } else if (!keySel && kc == 39) {
            controller_set(0, UInt8(BUTTON_SE), 1);
            keySel = true
            //ppu_dump_ram();
        }
    }
}
