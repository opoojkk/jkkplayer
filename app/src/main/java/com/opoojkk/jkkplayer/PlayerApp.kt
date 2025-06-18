package com.opoojkk.jkkplayer

import android.app.Application

class PlayerApp : Application() {

    override fun onCreate() {
        super.onCreate()
        // Initialize FFmpeg native libraries
        FFmpegNative.initFFmpeg()
    }
}