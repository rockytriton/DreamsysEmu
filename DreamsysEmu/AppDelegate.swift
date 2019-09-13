//
//  AppDelegate.swift
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/6/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

import Cocoa

import AudioKit;

@NSApplicationMain
class AppDelegate: NSObject, NSApplicationDelegate {
    /*
    let sampler = AKSampler()
    var audioQueueRead = AudioQueueRead()
    var buffers:[AudioQueueBufferRef?] = []
    var bufferLengthInFrames:UInt32 = 4096
    var frameSizeInBytes:UInt32 = UInt32(MemoryLayout<Float32>.size)
    var bufferLengthInBytes:UInt32 {
        return bufferLengthInFrames * frameSizeInBytes
    }
    var queue:AudioQueueRef? = nil
    var audioDispatchQueue = DispatchQueue(label: "nesAudioQueue", qos: DispatchQoS.default)
    */
    @IBAction func onOpenFile(_ sender: Any) {
        let dialog = NSOpenPanel();
        
        dialog.title                   = "Choose a .nes file";
        dialog.showsResizeIndicator    = true;
        dialog.showsHiddenFiles        = false;
        dialog.canChooseDirectories    = true;
        dialog.canCreateDirectories    = true;
        dialog.allowsMultipleSelection = false;
        dialog.allowedFileTypes        = ["nes"];
        
        if (dialog.runModal() == NSApplication.ModalResponse.OK) {
            let result = dialog.url // Pathname of the file
            
            if (result != nil) {
                let path = result!.path
                
                let alert = NSAlert()
                alert.messageText = path
                alert.informativeText = "Game"
                alert.alertStyle = NSAlert.Style.warning
                alert.addButton(withTitle: "OK")
                alert.addButton(withTitle: "Cancel")
               
                
                print("OPENING PATH: ", path);
                
                loadCartridge(path)
                
                NSDocumentController.shared.noteNewRecentDocumentURL(URL(fileURLWithPath:path));
                
                let worker = DispatchQueue(label: "cpu");
                worker.async {
                    emu_run();
                }
            }
        } else {
            // User clicked on "Cancel"
            return
        }
    }
    
    func application(_ sender: NSApplication, openFile filename: String) -> Bool {
        loadCartridge(filename)
        
        let worker = DispatchQueue(label: "cpu");
        worker.async {
            emu_run();
        }
        
        return true
    }
    
    var outputQueue : AudioQueueRef? = nil;
    
    
    func prepareBuffer(queue:AudioQueueRef, buffer:AudioQueueBufferRef) {
        
        /*
        if (nes.getAudioBufferLength() > 0) {
            var len = emu_read_audio(buffer.pointee.mAudioData)
            
            let bufferLength = min(nes.getAudioBufferLength() * frameSizeInBytes, bufferLengthInBytes)
            memcpy(buffer.pointee.mAudioData, nes.getAudioBuffer(), Int(bufferLength))
            buffer.pointee.mAudioDataByteSize = bufferLength
            nes.clearAudioBuffer()
        } else {
            memset(buffer.pointee.mAudioData, 0, Int(bufferLengthInBytes))
            buffer.pointee.mAudioDataByteSize = bufferLengthInBytes
        }
 */
        //var computeArray: [Float] = Array(repeating: 0, count: 4096)
        //var p = withUnsafeMutablePointer(to: &computeArray) { p in return p }
        
        /*
        var len : UInt16 = 0;
        var p : UnsafeMutablePointer<Float>? = emu_read_samples(&len);
        //var len = emu_read_audio(p)
        //print("PREP BUFF: ", len);
        
        
        if (len > 0) {
            let bufferLength = min(UInt32(len) * frameSizeInBytes, bufferLengthInBytes)
            memcpy(buffer.pointee.mAudioData, p, Int(bufferLength))
            buffer.pointee.mAudioDataByteSize = bufferLength
        } else {
            memset(buffer.pointee.mAudioData, 0, Int(bufferLengthInBytes))
            buffer.pointee.mAudioDataByteSize = bufferLengthInBytes
        }
        //nes.clearAudioBuffer()
        emu_done_read();
        
        let error:OSStatus = AudioQueueEnqueueBuffer(queue, buffer, 0, nil)
        if (error != 0) {
            print ("Error enqueing audio buffer:\(error)")
        }
 */
    }
    
    func applicationDidFinishLaunching(_ aNotification: Notification) {
        // Insert code here to initialize your application
        InitGamepad();
        
        emu_init();
        
        /*
        var audioFormat:AudioStreamBasicDescription = AudioStreamBasicDescription()
        audioFormat.mSampleRate = SAMPLE_RATE //44100
        audioFormat.mFormatID = kAudioFormatLinearPCM
        audioFormat.mFormatFlags = kLinearPCMFormatFlagIsFloat | kLinearPCMFormatFlagIsPacked
        audioFormat.mFramesPerPacket = 1
        audioFormat.mChannelsPerFrame = 1
        audioFormat.mBitsPerChannel = frameSizeInBytes * 8
        audioFormat.mBytesPerFrame = frameSizeInBytes
        audioFormat.mBytesPerPacket = frameSizeInBytes
        audioFormat.mReserved = 0
    
        
        
        var error:OSStatus
        error = AudioQueueNewOutputWithDispatchQueue(&queue, &audioFormat, 0, audioDispatchQueue, { [unowned self] (queue:AudioQueueRef, buffer:AudioQueueBufferRef) -> Void in
            self.prepareBuffer(queue: queue, buffer: buffer)
        })
        
        if (error != 0) {
            print ("Error creating audio queue:\(error)")
            return
        }
        
        for _ in 0..<5 {
            var buffer:AudioQueueBufferRef?
            error = AudioQueueAllocateBuffer(queue!, bufferLengthInBytes, &buffer)
            if (error != 0) {
                print ("Error allocating audio buffer:\(error)")
                return
            }
            self.prepareBuffer(queue: queue!, buffer: buffer!)
        }
        AudioQueueSetParameter(queue!, kAudioQueueParam_Volume, 1.0);
        error = AudioQueueStart(queue!, nil)
        if (error != 0) {
            print ("Error starting queue:\(error)")
            return
        }
        
        print("STARTED Q")
        */
        
        
    }

    func applicationWillTerminate(_ aNotification: Notification) {
        // Insert code here to tear down your application
    }


}

