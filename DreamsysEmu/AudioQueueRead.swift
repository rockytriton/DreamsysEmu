//
//  AudioQueueRead.swift
//  AudioPlay
//
//  Created by Casey on 16/04/2019.
//  Copyright © 2019 nb. All rights reserved.
//

import AVFoundation


class AudioQueueRead: NSObject {
    
    
    
    
    var _bufferQueue = Array<AudioQueueBufferRef>()
    var _reuserBufferQueue = Array<AudioQueueBufferRef>()
    
    var _audioQueue:AudioQueueRef?
    var _basicDescription:AudioStreamBasicDescription?
    var _bufferSize:UInt32 = 0
    
    
    
    override init() {
        super.init()
        initLock()
        
    }
    
    func startAudio()  {
        
        if _audioQueue != nil {
            let status = AudioQueueStart(_audioQueue!, nil)
            if status != noErr {
                print("start fail: \(status)")
            }
        }
    }
    
    func pauseAudio() {
        
        if _audioQueue != nil {
            AudioQueueStop(_audioQueue!, true)
            AudioQueuePause(_audioQueue!)
        }
    }
    
    func createQueue(_ basicDescription:AudioStreamBasicDescription?, _ bufferSize:UInt32) -> Bool {
        
        
        _basicDescription = basicDescription!
        
        let selfPoint =  Unmanaged.passUnretained(self as AnyObject).toOpaque()
        
        var status =  AudioQueueNewOutput(&_basicDescription!, { (client, inAQ, inBuffer) in
            
            if let selfPoint = UnsafeRawPointer.init(client) {
                
                let target = Unmanaged<AudioQueueRead>.fromOpaque(selfPoint).takeUnretainedValue()
                target.audioQueueOutputCallback(inBuffer)
            }
            
            
            //            if let selfPoint = client {
            //
            //                let target =  selfPoint.assumingMemoryBound(to: AudioQueueRead.self).pointee
            //                target.audioQueueOutputCallback(inBuffer)
            //            }
            
        }, selfPoint, nil, nil, 0, &_audioQueue)
        
        
        if status != noErr {
            print("AudioQueueNewOutput error:\(status)")
            return false
        }
        
        status = AudioQueueAddPropertyListener(_audioQueue!, kAudioQueueProperty_IsRunning, { (client, inAQ, InID) in
            
            if let selfPoint = UnsafeRawPointer.init(client) {
                
                let target = Unmanaged<AudioQueueRead>.fromOpaque(selfPoint).takeUnretainedValue()
                target.handleAudioPropertyStatus(inID: InID)
            }
            
            //            if let selfPoint = client {
            //
            //                let target =  selfPoint.assumingMemoryBound(to: AudioQueueRead.self).pointee
            //                target.handleAudioPropertyStatus(inID: InID)
            //            }
            
            
        }, selfPoint)
        
        
        if status != noErr {
            print("kAudioQueueProperty_IsRunning error:\(status)")
            _audioQueue = nil
            return false
        }
        
        if _audioQueue == nil {
            print("_audioQueue nil")
            return false
        }
        status = AudioQueueStart(_audioQueue!, nil)
        
        if status != noErr {
            print("AudioQueueStart error:\(status)")
            AudioQueueDispose(_audioQueue!, true)
            _audioQueue = nil
            return false
        }
        
        
        
        //var property = kAudioQueueHardwareCodecPolicy_PreferSoftware
        
        //AudioQueueSetProperty(_audioQueue!, property, &property, UInt32(MemoryLayout.size(ofValue: property)))
        
        _bufferSize = bufferSize
        
        if _bufferQueue.count == 0 {
            
            for _ in 0...1 {
                
                var buffer:AudioQueueBufferRef?
                status = AudioQueueAllocateBuffer(_audioQueue!, bufferSize, &buffer)
                if status == noErr {
                    
                    _bufferQueue.append(buffer!)
                    _reuserBufferQueue.append(buffer!)
                    
                }
            }
        }
        return true
    }
    
    
    
    var _start = false
    
    func playerAudioQueue(_ audioData:NSData, _ numPackets:UInt32, packetDescs:NSArray) -> Bool { // Array<AudioStreamPacketDescription>
        
        print("playerAudioQueue::\(packetDescs.count)")
        
        if _reuserBufferQueue.count == 0 {
            self.mutexWait()
        }
        if packetDescs.count == 0 {
            return true
        }
        let queueBufferRef =  _reuserBufferQueue.remove(at: 0)
        
        
        memcpy(queueBufferRef.pointee.mAudioData, audioData.bytes, audioData.length)
        
        queueBufferRef.pointee.mAudioDataByteSize = UInt32(audioData.length)
        
        let packketDescPoint = UnsafeMutablePointer<AudioStreamPacketDescription>.allocate(capacity: Int(numPackets+1))
        var packetsCount = Int(numPackets);
        if numPackets > packetDescs.count {
            packetsCount = packetDescs.count - 1
        }
        for index in 0...packetsCount {
            
            let point =  packketDescPoint.advanced(by: index)
            if let desc = packetDescs[index] as? AudioStreamPacketDescription {
                point.pointee = desc
            }
        }
        
        print("EnqueueBuffer")
        let status = AudioQueueEnqueueBuffer(_audioQueue!, queueBufferRef, numPackets, packketDescPoint)
        ////
        packketDescPoint.deallocate()
        if status == noErr {
            
            if _reuserBufferQueue.count == 0 {
                
                if !_start && !startAudioQueue(){
                    print("_start error:\(status)")
                    return false
                }
                
            }
            
        }else {
            
            print("AudioQueueEnqueueBuffer error:\(status)")
            return false
        }
        return true
    }
    
    
    func audioQueueOutputCallback(_ inBuffer:AudioQueueBufferRef) {
        
        for queueBufferR in _bufferQueue {
            
            if queueBufferR == inBuffer {
                
                _reuserBufferQueue.append(queueBufferR)
                break
            }
        }
        
        self.mutexSignal()
    }
    
    
    var _running:UInt32 = 0;
    func handleAudioPropertyStatus(inID:AudioQueuePropertyID) {
        
        if inID == kAudioQueueProperty_IsRunning {
            
            var size = UInt32(MemoryLayout.size(ofValue: _running))
            AudioQueueGetProperty(_audioQueue!, inID, &_running, &size)
            
            print("_running: \(_running)")
            
        }
        
    }
    
    func startAudioQueue() -> Bool{
        
        let status = AudioQueueStart(_audioQueue!, nil)
        if status != noErr {
            print("AudioQueueStart error:\(status)")
            return false
            
        }else {
            _start = true
        }
        return true
    }
    
    
    //MARK: Lock
    
    var _mutex_t:pthread_mutex_t =  pthread_mutex_t.init()
    var _cond:pthread_cond_t = pthread_cond_t.init()
    
    func initLock()  {
        pthread_mutex_init(&_mutex_t, nil)
        pthread_cond_init(&_cond, nil)
    }
    
    func mutexWait()  {
        
        
        pthread_mutex_lock(&_mutex_t)
        
        print("wait")
        pthread_cond_wait(&_cond, &_mutex_t)
        
        pthread_mutex_unlock(&_mutex_t)
        
    }
    
    func mutexSignal() {
        
        pthread_mutex_lock(&_mutex_t)
        
        print("signal")
        pthread_cond_signal(&_cond)
        
        pthread_mutex_unlock(&_mutex_t)
        
    }
    
    func mutexDestory() {
        
        pthread_mutex_destroy(&_mutex_t)
        pthread_cond_destroy(&_cond)
    }
}
