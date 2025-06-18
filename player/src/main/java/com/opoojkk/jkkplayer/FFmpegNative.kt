package com.opoojkk.jkkplayer

object FFmpegNative {
    init {
        System.loadLibrary("avcodec");
        System.loadLibrary("avfilter");
        System.loadLibrary("avformat");
        System.loadLibrary("avutil");
        System.loadLibrary("swresample");
        System.loadLibrary("swscale");
        System.loadLibrary("player-native");
    }

    external fun initFFmpeg()

    external fun readFile(fileName: String)
}