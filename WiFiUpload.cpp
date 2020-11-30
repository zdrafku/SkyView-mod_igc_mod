/*
 *  WiFiUpload.cpp
 *
 *  Export to server files in ICG directory via WiFi AP
 *
 *  Author: Mr. Psycho, psychotm@gmail.com
 */

#include "WiFiUpload.h"

void uWiFi_setup() {
  Serial.printf("\n\nConnecting to %s", SSID_NAME);

  WiFi.disconnect();
  WiFi.begin(SSID_NAME, SSID_PASSWORD);
  /*
    IPAddress ip(192, 168, 1, 254);
    IPAddress gateway(192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);
    WiFi.config(ip, gateway, subnet);
  */
  delay(500);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(". ");
  }

  Serial.print("\nWiFi connected with IP: ");
  Serial.println(WiFi.localIP());
}

bool Upload_to_API(String uFile) {

  WiFiClient client;
  //const char* host =  "log.my-domain.com";
  const char* host =  "192.168.1.15";
  //String URI =        "/importigc";
  String URI =        "/files/post.php";
  String fe_user =    "WEB USER";
  String fe_pass =    "WEB PASS";
  String import_uid = "WEB uID";

  //const int httpPort = 80;
  const int httpPort = 12312;
  if (!client.connect(host, httpPort)) {
    Serial.println("Remote connection failed");
    return false;
  }
  auto upload_file = SD.open(IGC_DIRECTORY + uFile, FILE_READ);
  const auto upload_size = upload_file.size();

  if ( upload_size < 1) return false;
  String file_line;
  while(upload_file.available()) {
    file_line += upload_file.readStringUntil('\r');
  }
  upload_file.close();

  String request_body;// = "\r\n";
  request_body = "\r\n------WebKitFormBoundaryjg2qVIUS8teOAbN3\r\n";
  request_body += "Content-Disposition: form-data; name=\"import_uid\"\r\n";
  request_body += "\r\n";
  request_body += import_uid;
  request_body += "\r\n------WebKitFormBoundaryjg2qVIUS8teOAbN3\r\n";
  request_body += "Content-Disposition: form-data; name=\"fe_user\"\r\n";
  request_body += "\r\n";
  request_body += fe_user;
  request_body += "\r\n------WebKitFormBoundaryjg2qVIUS8teOAbN3\r\n";
  request_body += "Content-Disposition: form-data; name=\"fe_pass\"\r\n";
  request_body += "\r\n";
  request_body += fe_pass;
  request_body += "\r\n------WebKitFormBoundaryjg2qVIUS8teOAbN3\r\n";
  request_body += "Content-Disposition: form-data; name=\"importigc_igc\"; filename=\"";
  uFile.replace(IGC_DIRECTORY, "");
  request_body += uFile;
  request_body += "\"\r\n"; // file name
  request_body += "Content-Type: application/octet-stream\r\n";
  request_body += "\r\n";
  request_body += file_line;
  request_body += "\r\n------WebKitFormBoundaryjg2qVIUS8teOAbN3--";
  request_body += "\r\n";

  client.print("POST ");
  client.print(URI);
  client.println(" HTTP/1.1");
  client.print("Host: ");
  client.println(host);
  client.println("User-Agent: SD Card Reader toolkit");
  client.println("Content-Type: multipart/form-data; boundary=----WebKitFormBoundaryjg2qVIUS8teOAbN3");
  //client.println("Content-Type: application/x-www-form-urlencoded");
  client.print("Content-Length: ");           //file size  //change it
  client.println(request_body.length());
  client.println(request_body);

  if (client.connected()) {

//    while (client.connected()) {
//      String line = client.readStringUntil('\r');
//      Serial.print(line);
//    }
    client.stop();  // DISCONNECT FROM THE SERVER
    SD.mkdir(IGC_ARCHIVE);
    //rename(uFile, IGC_ARCHIVE + "/" + uFile);
    return true;
  } else {
    return false;
  }
}

void listDir(String root_path, bool debug) {
  //Serial.printf("Listing directory: %s\n", dirname);
  String line_1;
  String line_2;
  String line_3;
  String line_4;
  int count = 0;

  File root = SD.open(root_path);
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    // check for folders starting with 202 (i.e 2020-11-22)
    Serial.println(file.name());
    if (file.isDirectory()) continue;
    if ( /*String(file.name()).indexOf("202") == 0 &&*/ String(file.name()).endsWith(".igc")) {
      if (debug) Serial.println("UPLOAD: " + root_path + file.name());

      // last 4 files for the 0.96" OLED display
      line_4 = line_3;
      line_3 = line_2;
      line_2 = line_1;
      line_1 = file.name();
      line_1 += Upload_to_API(file.name())? "*" : "-";

      // every file with full name
      line_1.replace("PSY-CHO-", "");
      line_1.replace(".igc", "");

      Serial.print("Uploading file: ");
      Serial.println(line_1);
//      // every file (short name) in directory
//      display.clearDisplay();
//
//      display.setTextSize(1);
//      display.setTextColor(WHITE);
//
//      display.setCursor(0, 0);
//      //display.println(line_4);
//      display.println(root_path + "  " + ++count);
//      display.display();
//
//      display.setCursor(0, 16);
//      display.println(line_3);
//      display.display();
//
//      display.setCursor(0, 32);
//      display.println(line_2);
//      display.display();
//
//      display.setCursor(0, 48);
//      display.println(line_1);
//      display.display();

      //delay(1000);
    }
//    esp_task_wdt_reset();
    file = root.openNextFile();
  }
}

void IGC_Upload(){
  uWiFi_setup();
  //listDir(IGC_DIRECTORY, false);

  Upload_to_API("/igc/2020-11-15-PSY-CHO-21.igc");

  WiFi_setup();
  return;
}
