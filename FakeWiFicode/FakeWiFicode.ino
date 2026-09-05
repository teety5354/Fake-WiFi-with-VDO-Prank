#include <DNSServer.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
#include <SD.h>  
#include <SPI.h> 

DNSServer dnsServer;
AsyncWebServer server(80);

// ESP32 Pin 
const int SD_CS   = 33; 
const int SD_SCK  = 25; 
const int SD_MOSI = 26; 
const int SD_MISO = 27; 

File logFile; //After this is your HTML Webpage for login prank.

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="th">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ลงชื่อเข้าใช้ @RWB_WiFi</title>
    <style>
        * {
            box-sizing: border-box;
            margin: 0;
            padding: 0;
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
            user-select: none; /* ป้องกันการคลุมดำข้อความ */
        }
        body {
            display: flex;
            flex-direction: column;
            align-items: center;
            height: 100vh;
            width: 100vw;
            /* พื้นหลังไล่สีโทนฟ้า-เทา-น้ำเงินเข้ม */
            background: linear-gradient(180deg, #8faebd 0%, #63859c 40%, #2f435c 100%);
            overflow: hidden;
            cursor: pointer; /* เปลี่ยนเมาส์เป็นรูปนิ้วชี้ทั้งหน้าจอ */
        }
        .container {
            display: flex;
            flex-direction: column;
            align-items: center;
            width: 100%;
            max-width: 400px;
            padding: 20px;
            margin-top: 40px;
        }
        
        /* สไตล์โลโก้ RWB WIFI 5G เดิม */
        .logo-box {
            display: flex;
            align-items: center;
            position: relative;
            margin-bottom: 30px;
            transform: scale(0.8); /* ย่อขนาดโลโก้ลงนิดหน่อยให้สมดุล */
        }
        .logo-rwb { font-size: 68px; font-weight: 800; color: #ffffff; line-height: 1; }
        .logo-divider { width: 6px; height: 60px; background-color: #ff8c00; margin: 0 6px 0 8px; border-radius: 1px; }
        .logo-right { display: flex; flex-direction: column; justify-content: space-between; height: 62px; }
        .logo-wifi { font-size: 20px; font-weight: 700; color: #ffffff; writing-mode: vertical-rl; text-orientation: mixed; transform: rotate(180deg); line-height: 1; }
        .logo-5g { font-size: 22px; font-weight: 800; color: #ffffff; position: absolute; bottom: -28px; right: 0; }

        .subtitle {
            color: white;
            font-size: 15px;
            text-align: center;
            margin-bottom: 25px;
        }

        /* กรอบข้อความหลอกๆ (ไม่ใช่ Input จริง) */
        .fake-input {
            width: 100%;
            background: rgba(255, 255, 255, 0.9);
            padding: 12px 15px;
            border-radius: 8px;
            margin-bottom: 12px;
            color: #666;
            font-size: 15px;
            display: flex;
            align-items: center;
            box-shadow: 0 2px 5px rgba(0,0,0,0.1);
        }
        .fake-input span { margin-right: 10px; font-size: 16px; }

        /* ปุ่ม Connect หลอกๆ */
        .fake-btn {
            width: 100%;
            background-color: #3b4e63;
            color: white;
            padding: 14px;
            border-radius: 8px;
            text-align: center;
            font-size: 16px;
            margin-bottom: 25px;
            box-shadow: 0 4px 6px rgba(0,0,0,0.2);
        }

        /* ข้อความด้านล่าง */
        .footer-text {
            color: #2a2a2a;
            font-size: 13px;
            text-align: center;
            line-height: 1.4;
        }
        .footer-text .small {
            font-size: 10px;
            color: #1a1a1a;
            margin-top: 15px;
            display: block;
        }
        .footer-text .school {
            color: #d1d9e0;
            font-size: 12px;
            margin-top: 5px;
            display: block;
            letter-spacing: 0.5px;
        }

        video {
            width: 100vw;
            height: 100vh;
            object-fit: cover;
            position: fixed;
            top: 0;
            left: 0;
            background: black;
            display: none;
            z-index: 9999;
        }
    </style>
</head>
<body>
    <div id="mainUI" class="container">
        
        <!-- โลโก้เดิมของคุณ -->
        <div class="logo-box">
            <div class="logo-rwb">RWB</div>
            <div class="logo-divider"></div>
            <div class="logo-right">
                <div class="logo-wifi">WIFI</div>
            </div>
            <div class="logo-5g">5G</div>
        </div>

        <div class="subtitle">Please log in to use the internet hotspot service</div>

        <!-- ดีไซน์หลอกตา ไม่มีช่องกรอกข้อมูลจริง -->
        <div class="fake-input"><span>👤</span> Username</div>
        <div class="fake-input"><span>🔑</span> Password</div>
        
        <div class="fake-btn">Connect</div>

        <div class="footer-text">
            นักเรียนใช้เลขประจำตัวนักเรียน และบัตร<br>ประชาชน13หลัก<br>
            ครูใช้รหัสbookmark(เช่นtc999) password<br>ปีพ.ศ.เกิด<br>
            
            <span class="small">>>iosไม่ขึ้นหน้าlogin ลองทำการเลิกใช้เครือข่ายทุกครั้ง และทำการ<br>เชื่อมต่อใหม่ </span>
            <span class="school">BADLIQUID FUNNY PRANK</span>
        </div>
    </div>

    <video id="videoPlayer" playsinline>
        <source src="/video" type="video/mp4">
    </video>

    <script>
        document.addEventListener('DOMContentLoaded', function() {
            var mainUI = document.getElementById('mainUI');
            var video = document.getElementById('videoPlayer');
            
            // สั่งให้กดตรงไหนก็ได้บนหน้าจอ (document.body) วิดีโอจะเล่นทันที
            document.body.addEventListener('click', function() {
                mainUI.style.display = 'none'; 
                video.style.display = 'block'; 
                video.play();
                
                // ทำให้คลิกวิดีโอเต็มจอได้ (เฉพาะบางเบราว์เซอร์ที่อนุญาต)
                if (video.requestFullscreen) { video.requestFullscreen(); }
                else if (video.webkitRequestFullscreen) { video.webkitRequestFullscreen(); }
            });
        });
    </script>
</body>
</html>
)rawliteral";
//End of HTML

//Webserver ( login page )
class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}
  bool canHandle(AsyncWebServerRequest *request) { return true; }
  void handleRequest(AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html); 
  }
};

void logMessage(String message) {
  Serial.println(message); 
  if (logFile) {
    logFile.println(message); 
    logFile.flush(); 
  }
}

void setupServer() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html); 
    logMessage("Client Connected");
  });

  server.on("/video", HTTP_GET, [](AsyncWebServerRequest *request) {
    logMessage("Another victim rickrolled :)");
    if (SD.exists("/your-video-file.mp4")) {
      File videoFile = SD.open("/your-video-file.mp4");
      if (videoFile) {
        request->send(SD, "/your-video-file.mp4", "video/mp4");
        videoFile.close();
      } else {
        request->send(500, "text/plain", "Failed to open video file");
      }
    } else {
      request->send(404, "text/plain", "Video file not found");
    }
  });
}

void setup() {
  Serial.begin(115200);

  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);

  if (!SD.begin(SD_CS)) {
    logMessage("SD card initialization failed");
    return;
  }
  logMessage("SD card initialized");

  logFile = SD.open("/log.txt", FILE_APPEND);
  if (!logFile) {
    logMessage("Failed to open log file");
    return;
  }

  logMessage("Setting up AP Mode");
  WiFi.mode(WIFI_AP); 
  WiFi.softAP("@RWB_WiFi 5G"); // Here is your Wifi name😎
  
  logMessage("AP IP address: " + WiFi.softAPIP().toString());
  logMessage("Setting up Async WebServer");
  setupServer();
  logMessage("Starting DNS Server");
  dnsServer.start(53, "*", WiFi.softAPIP());
  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER); 
  server.begin();
  logMessage("All Done!");
}

void loop() {
  dnsServer.processNextRequest();
}