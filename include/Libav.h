#include <string>
#include <iomanip>
#include <fstream>

extern "C"
{
    #include "libavformat/avformat.h"
    #include <libavutil/timestamp.h>
}

#ifdef av_err2str
#undef av_err2str
av_always_inline std::string av_err2string(int errnum){
    char str[AV_ERROR_MAX_STRING_SIZE];
    return av_make_error_string(str, AV_ERROR_MAX_STRING_SIZE, errnum);
}
#define av_err2str(err) av_err2string(err).c_str()
#endif

std::string unixTimeToChunkName(int unixtime){
    time_t t = unixtime;
    struct tm tm;
    localtime_r(&t, &tm);
    std::string year = std::to_string(1900 + tm.tm_year);
    std::string month = std::to_string(tm.tm_mon+1);
    std::string day = std::to_string(tm.tm_mday);
    std::string hour = std::to_string(tm.tm_hour);
    std::string minute = std::to_string(tm.tm_min);
    std::string second = std::to_string(tm.tm_sec);

    if(month.length() == 1) month = "0"+month;
    if(day.length() == 1) day = "0"+day;
    if(hour.length() == 1) hour = "0"+hour;
    if(minute.length() == 1) minute = "0"+minute;
    if(second.length() == 1) second = "0"+second;

    std::string result = year + "-" + month + "-" + day + "-" + hour + "-" + minute + "-" + second;
    return result;
}

class Libav{
    private:
        const char *filePath;
        std::ofstream chunkP;

        AVFormatContext *inputCtx;
        AVFormatContext *outputCtx;

        double currentDurationChunk;
        size_t chunkDuration;
    public:
        Libav(std::string path);

        int openInput();
        int openOutput();
        void remux();
        void logPacket(AVFormatContext *fmt_ctx, AVPacket *pkt, char *tag);
        ~Libav();
};