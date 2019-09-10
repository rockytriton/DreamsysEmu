//
//  AppDelegate.swift
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/6/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

import Cocoa

@NSApplicationMain
class AppDelegate: NSObject, NSApplicationDelegate {
    
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
    
    func applicationDidFinishLaunching(_ aNotification: Notification) {
        // Insert code here to initialize your application
    }

    func applicationWillTerminate(_ aNotification: Notification) {
        // Insert code here to tear down your application
    }


}

