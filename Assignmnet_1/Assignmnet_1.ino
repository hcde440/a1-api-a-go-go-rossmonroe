/*Program Description
 * This program calls on two API using get requests to json data.
 * Firs NASA is called to get the current location of the ISS over earth, then the second part grabs the most recent traffic incident from map quest within Seattle.
*/



#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

//Below is the wifi info and API keys.

const char* ssid = "Monroe";
const char* pass = "borstad1961";
String nasaKey = "GrRle7PU65jvsmbUemsaZOn3tGhfez95POs68hQI";
String mapquestKey = "TWVS45JTm6FcoGnmLM25RrpGCrHWQGRv";



//These data structures below grab data from the Jsonbuffer to keep it clear so it can be reused.
typedef struct {
  String con; //Verify connection works
  String tm;  //Timestamp of when the data is retreived.
  String lat; //Current latitude
  String lon; //Current Longitude
} issInfo;

issInfo iss; //allows the data to be passed to the struct through a call of iss.

typedef struct {
  String sev; //Severity level of traffic incident.
  String st;  //Time of incident.
  String fd;  //Description of incident.
} trafficData;

trafficData traffic; //allows the data to be passed to the struct through a call of traffic.

void setup() { //Initial setup to establish wifi connection, then calls two functions to retrieve API data from NASA and Map Quest.
  Serial.begin(115200);
  delay(10);

  Serial.print("Connecting to "); Serial.println(*ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println(); Serial.println("WiFi connected"); Serial.println();

  issLocation(); //Calls function to get ISS location then prints the data that gets passed to the struct.

  Serial.println();
  Serial.println("Heres some information about the ISS.");
  Serial.println("The connection was a " + iss.con);
  Serial.println("The timestamp is " + iss.tm);
  Serial.println("Current latitude: " + iss.lat);
  Serial.println("Current longitude: " + iss.lon);

  localTraffic(); //Calls function to get Traffic Incident data then prints the data that gets passed to the struct.
  
  Serial.println();
  Serial.println("Heres local traffic incidents in Seattle.");
  Serial.println("Severity level: (0-4) " + traffic.sev);
  Serial.println("Time: " + traffic.st);
  Serial.println("Description: " + traffic.fd);

}


void loop() {
}


void issLocation() { //A funtion that grabs ISS information with checkpoints in case the request gets denied, fails, or there is a parsing error.
  HTTPClient theClient;
  Serial.println();
  Serial.println("Making HTTP request to NASA");
  theClient.begin("http://api.open-notify.org/iss-now.json");
  int httpCode = theClient.GET();

  if (httpCode > 0) {
    if (httpCode == 200) {
      Serial.println("Received HTTP payload.");
      DynamicJsonBuffer jsonBuffer;
      String payload = theClient.getString();
      Serial.println("Parsing...");
      JsonObject& root = jsonBuffer.parse(payload);

      // Test if parsing succeeds.
      if (!root.success()) {
        Serial.println("parseObject() failed");
        Serial.println(payload);
        return;
      }
      iss.con = root["message"].as<String>();
      iss.tm = root["timestamp"].as<String>();
      iss.lat = root["iss_position"]["latitude"].as<String>();
      iss.lon = root["iss_position"]["longitude"].as<String>();
    }
    else {
      Serial.println(httpCode);
      Serial.println("Something went wrong with connecting to the endpoint.");
    }
  }
}

void localTraffic() { //A funtion that grabs Traffic information with checkpoints in case the request gets denied, fails, or there is a parsing error.
  HTTPClient theClient;
  Serial.println();
  Serial.println("Making HTTP request to Map Quest");
  theClient.begin("http://www.mapquestapi.com/traffic/v2/incidents?key=" + mapquestKey + "&boundingBox=47.70167531192826%2C-122.09964752197266%2C47.50444632608873%2C-122.56107330322266&filters=incidents");
  int httpCode = theClient.GET();
  if (httpCode > 0) {
    if (httpCode == 200) {
      Serial.println("Received HTTP payload.");
      DynamicJsonBuffer jsonBuffer;
      String payload = theClient.getString();
      Serial.println("Parsing...");
      JsonObject& root = jsonBuffer.parse(payload);

      // Test if parsing succeeds.
      if (!root.success()) {
        Serial.println("parseObject() failed");
        Serial.println(payload);
        return;
      }
      traffic.sev = root["incidents"][0]["severity"].as<String>(); //Because of the array size I call the first object in the array which is ordered according to the most recent incident.
      traffic.st = root["incidents"][0]["startTime"].as<String>();
      traffic.fd = root["incidents"][0]["fullDesc"].as<String>();
    }
    else {
      Serial.println(httpCode);
      Serial.println("Something went wrong with connecting to the endpoint.");
    }
  }
}
