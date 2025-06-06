| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- | -------- |

# Native OTA example

This example is based on `app_update` component's APIs.

## Configuration

Refer the README.md in the parent directory for the setup details.


在bulid目录启动服务器：python -m http.server 8080 --bind 0.0.0.0
然后可以通过http://192.168.0.101:8080/访问到服务器
进入升级后如果set(PROJECT_VER "1.0.2")版本一致不会进行升级