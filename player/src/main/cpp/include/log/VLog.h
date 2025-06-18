//
// Created by xx on 2025/6/8.
//

#ifndef JKKPLAYER_ALOG_H

#include <android/log.h>

#define JKKPLAYER_ALOG_H


class VLog {
public:
    static const void v(const char *tag, const char *fmt, ...);


    static const void d(const char *tag, const char *fmt, ...);


    static const void i(const char *tag, const char *fmt, ...);


    static const void w(const char *tag, const char *fmt, ...);


    static const void e(const char *tag, const char *fmt, ...);


private:
    const char *tag;

    void print(const char *tag, int prio, const char *fmt, ...);
};


#endif //JKKPLAYER_ALOG_H
