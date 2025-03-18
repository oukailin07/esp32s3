#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H
#include <stdint.h>  // 包含 uint8_t 的定义
#include <stddef.h>  // 包含 size_t 的定义

// 百度智能云语音识别API配置
#define BAIDU_SPEECH_TO_TEXT_URL "https://aip.baidubce.com/oauth/2.0/token"
#define BAIDU_API_KEY "HKXvSjLc5Co28bjtvVIXkVuE" // 替换为你的百度API密钥
#define BAIDU_SECRET_KEY "pzVcTIXM8K2mTHWuPWZVMZceoQqjptMf" // 替换为你的百度Secret Key
#define ACCESS_TOKEN "24.0c10b25b8cb7f7167f1fe3aab0bd3bd1.2592000.1744720185.282335-118065803"



void http_speech_to_text(const char* audio_data, size_t audio_length);
//char* http_ai_dialogue(const char* text);
void http_init();

#endif