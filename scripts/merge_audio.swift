#!/usr/bin/env swift

import Foundation
import AVFoundation

guard CommandLine.arguments.count >= 4 else {
    print("Usage: swift merge_audio.swift <video_path> <audio_path> <output_mp4_path>")
    exit(1)
}

let videoPath = CommandLine.arguments[1]
let audioPath = CommandLine.arguments[2]
let outputPath = CommandLine.arguments[3]

let videoURL = URL(fileURLWithPath: videoPath)
let audioURL = URL(fileURLWithPath: audioPath)
let outputURL = URL(fileURLWithPath: outputPath)

try? FileManager.default.removeItem(at: outputURL)

let videoAsset = AVURLAsset(url: videoURL)
let audioAsset = AVURLAsset(url: audioURL)

let composition = AVMutableComposition()

guard let videoTrack = videoAsset.tracks(withMediaType: .video).first else {
    print("[-] Error: No video track found in \(videoPath)")
    exit(1)
}

guard let compVideoTrack = composition.addMutableTrack(withMediaType: .video, preferredTrackID: kCMPersistentTrackID_Invalid) else {
    print("[-] Error: Could not create composition video track")
    exit(1)
}

let videoDuration = videoAsset.duration
let timeRange = CMTimeRange(start: .zero, duration: videoDuration)

do {
    try compVideoTrack.insertTimeRange(timeRange, of: videoTrack, at: .zero)
} catch {
    print("[-] Error inserting video track: \(error)")
    exit(1)
}

compVideoTrack.preferredTransform = videoTrack.preferredTransform

if let audioTrack = audioAsset.tracks(withMediaType: .audio).first {
    if let compAudioTrack = composition.addMutableTrack(withMediaType: .audio, preferredTrackID: kCMPersistentTrackID_Invalid) {
        let audioDuration = min(audioAsset.duration.seconds, videoDuration.seconds)
        let audioRange = CMTimeRange(start: .zero, duration: CMTime(seconds: audioDuration, preferredTimescale: 600))
        do {
            try compAudioTrack.insertTimeRange(audioRange, of: audioTrack, at: .zero)
            print("[+] Added audio track (\(String(format: "%.2f", audioDuration))s)")
        } catch {
            print("[-] Warning: Failed to insert audio track: \(error)")
        }
    }
} else {
    print("[-] Error: No audio track found in \(audioPath)")
    exit(1)
}

guard let exportSession = AVAssetExportSession(asset: composition, presetName: AVAssetExportPresetHighestQuality) else {
    print("[-] Error: Failed to create AVAssetExportSession")
    exit(1)
}

exportSession.outputURL = outputURL
exportSession.outputFileType = .mp4
exportSession.shouldOptimizeForNetworkUse = true

let semaphore = DispatchSemaphore(value: 0)
print("[*] Merging video and audio into \(outputPath)...")

exportSession.exportAsynchronously {
    switch exportSession.status {
    case .completed:
        print("[+] SUCCESS: Successfully exported merged MP4 to \(outputPath)")
    case .failed:
        print("[-] Export failed: \(String(describing: exportSession.error))")
    case .cancelled:
        print("[-] Export cancelled")
    default:
        print("[-] Unknown export status: \(exportSession.status.rawValue)")
    }
    semaphore.signal()
}

semaphore.wait()

