//
// Created by xx on 2025/6/8.
//

#include <jni.h>

#ifndef JKKPLAYER_LOG_H
#define logv(tag, fmt, ...) VLog::v(tag, fmt, ##__VA_ARGS__)
#define logd(tag, fmt, ...) VLog::d(tag, fmt, ##__VA_ARGS__)
#define logi(tag, fmt, ...) VLog::i(tag, fmt, ##__VA_ARGS__)
#define logw(tag, fmt, ...) VLog::w(tag, fmt, ##__VA_ARGS__)
#define loge(tag, fmt, ...) VLog::e(tag, fmt, ##__VA_ARGS__)
#define JKKPLAYER_LOG_H
#endif

void save_gray_frame(uint8_t *string, int i, int width, int height, char filename[1024]);

extern "C" {
#include "libavutil/avutil.h"
#include "avformat.h"
#include "avcodec.h"
#include "log/VLog.h"
}


extern "C"
JNIEXPORT void JNICALL
Java_com_opoojkk_jkkplayer_FFmpegNative_initFFmpeg(JNIEnv
*env,
jobject thiz
) {
// Initialize FFmpeg libraries
avformat_network_init();

// Set the log level to AV_LOG_DEBUG for detailed logging
av_log_set_level(AV_LOG_DEBUG);

// Optionally, set a custom log callback if needed
// av_log_set_callback(custom_log_callback);

// Log initialization message
logi("FFmpegNative", "FFmpeg libraries initialized successfully.");
}


extern "C"
JNIEXPORT void JNICALL
Java_com_opoojkk_jkkplayer_FFmpegNative_readFile(JNIEnv
*env,
jobject thiz, jstring
file_name) {
char tag[] = "FFmpegPlayer";

// 获得相关格式（容器）的信息
AVFormatContext *pFormatContext = avformat_alloc_context();
if (!pFormatContext) {
av_log(NULL,
AV_LOG_ERROR, "Could not allocate AVFormatContext.\n");
return;
}

// 打开文件并读取文件的头信
auto result = avformat_open_input(&pFormatContext, env->GetStringUTFChars(file_name, nullptr),
                                  nullptr,
                                  nullptr);
if (result < 0) {
loge(tag, "Could not open file %s: %s",
     env->GetStringUTFChars(file_name, nullptr), av_err2str(result));
avformat_free_context(pFormatContext);
return;
}
logi("File opened successfully: %s\n", env->GetStringUTFChars(file_name, nullptr));

// 从媒体文件中读取数据
result = avformat_find_stream_info(pFormatContext, nullptr);
if (result < 0) {
loge(tag, "Could not find stream information: %s", av_err2str(result));
return;
}

// 循环获取所有流数据
for (
unsigned int i = 0;
i<pFormatContext->
nb_streams;
i++) {
AVStream *stream = pFormatContext->streams[i];
AVCodecParameters *codecpar = stream->codecpar;
av_log(NULL,
AV_LOG_INFO, "Stream %u: Codec ID: %d, Type: %d, Bitrate: %lld\n",
i, codecpar->codec_id, codecpar->codec_type, codecpar->bit_rate);
AVCodec *codec = const_cast<AVCodec *>(avcodec_find_decoder(codecpar->codec_id));
// 用于视频和音频
if (codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
logi(tag, "Video Codec: resolution %d x %d",
     codecpar->width, codecpar->height);
} else if (codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
logi(tag, "Audio Codec: %d channels, sample rate %d", codecpar->ch_layout,
     codecpar->sample_rate);
}

// 通用
logi(tag, "Stream %u: Codec %s ID %d, Bitrate: %lld",
     i, codec->long_name, codecpar->codec_id, codecpar->bit_rate);
VLog::v(tag,
"Stream %u: Codec ID: %d, Type: %d, Bitrate: %lld",
i, codecpar->codec_id, codecpar->codec_type, codecpar->bit_rate);


// 打开解码器
AVCodecContext *pCodecContext = avcodec_alloc_context3(codec);
avcodec_parameters_to_context(pCodecContext, codecpar
);
result = avcodec_open2(pCodecContext, codec, NULL);
if (result < 0) {
loge(tag, "Could not open codec for stream %u: %s", i, av_err2str(result));
avcodec_free_context(&pCodecContext);
continue;
}

// 读取数据包之前，分配 AVPacket 和 AVFrame
AVPacket *pPacket = av_packet_alloc();
AVFrame *pFrame = av_frame_alloc();

// 读取数据包
while (
av_read_frame(pFormatContext, pPacket
) >= 0) {
if (pPacket->stream_index == i) {
// 解码数据包
// 把原始数据包（未解压的帧）发送给解码器。
int ret = avcodec_send_packet(pCodecContext, pPacket);
if (ret < 0) {
loge(tag, "Error sending packet for decoding: %s\n",
     av_err2str(ret));
continue;
}
// 接收解码后的帧
// 解码器接受原始数据帧（解压后的帧）
ret = avcodec_receive_frame(pCodecContext, pFrame);
if (ret < 0) {
loge(tag, "Error receiving frame from decoder: %s\n",
     av_err2str(ret));
continue;
}
// 处理解码后的帧
logi(tag, "Decoded frame %d from stream %u",
     pFrame->pts, i);

}
av_packet_unref(pPacket);
}
char *frame_filename = "/data/data/com.opoojkk.jkkplayer/files/saved_frame";

save_gray_frame(pFrame
->data[0], pFrame->linesize[0], pFrame->width, pFrame->height,
frame_filename);
}


}


void save_gray_frame(uint8_t *buf, int wrap, int xsize, int ysize, char *filename) {
    FILE *f;
    int i;
    f = fopen(filename, "w");
    // 编写 pgm 格式所需的最小文件头
    // portable graymap format -> https://en.wikipedia.org/wiki/Netpbm_format#PGM_example
    fprintf(f, "P5\n%d %d\n%d\n", xsize, ysize, 255);

    // 逐行写入
    for (i = 0; i < ysize; i++)
        fwrite(buf + i * wrap, 1, xsize, f);
    fclose(f);
}