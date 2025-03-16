#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H
#include <stdint.h>  // 包含 uint8_t 的定义
#include <stddef.h>  // 包含 size_t 的定义

// 百度智能云语音识别API配置
#define BAIDU_SPEECH_TO_TEXT_URL "https://aip.baidubce.com/oauth/2.0/token"
#define BAIDU_API_KEY "HKXvSjLc5Co28bjtvVIXkVuE" // 替换为你的百度API密钥
#define BAIDU_SECRET_KEY "pzVcTIXM8K2mTHWuPWZVMZceoQqjptMf" // 替换为你的百度Secret Key
#define ACCESS_TOKEN "24.0c10b25b8cb7f7167f1fe3aab0bd3bd1.2592000.1744720185.282335-118065803"
// MiniMax API配置
#define MINIMAX_API_URL "https://api.minimax.chat/v1/chat/completion"
#define MINIMAX_API_KEY "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJHcm91cE5hbWUiOiJva2wiLCJVc2VyTmFtZSI6Im9rbCIsIkFjY291bnQiOiIiLCJTdWJqZWN0SUQiOiIxOTAwNzM0MDE1NTA2MjM1NzIxIiwiUGhvbmUiOiIxNTIyMTY1NTYzNiIsIkdyb3VwSUQiOiIxOTAwNzM0MDE1NDkzNjUyODA5IiwiUGFnZU5hbWUiOiIiLCJNYWlsIjoiIiwiQ3JlYXRlVGltZSI6IjIwMjUtMDMtMTYgMjA6NDM6NTIiLCJUb2tlblR5cGUiOjEsImlzcyI6Im1pbmltYXgifQ.qhPLzu1MBBh1b7GLt0Zc0H9URmIOSsrNhveYWcAJgKcIMqr2fw5dGPTw4LSCBhNKZYZ2qdb4tIE1i-gbT8SvR1c924JVy3w_wIwHcwoSYBJNR3Ej7-HpshFSYUbIwuoOycxEbDiayr19_lN1MZOO9XpdK6kn_duy1UJhNRdBlOKX4paL9oe2XWgp6wR1W0tvrlypa8lDkhV1Xkmh0UII8tVMzWvopLuTABToc7ebbXZtn9dPsduy-iP1qDeyXMllMPsHf3VFblDy2ohaSIyRtevO_sG_nFr3H5MZuWHf2crAToPe-ocCT-i-R1SYazUmAryQSrlZZlMwsnImcCDXrA" // 替换为你的MiniMax API密钥

char* http_speech_to_text(const uint8_t* audio_data, size_t audio_length);
char* http_ai_dialogue(const char* text);


#endif