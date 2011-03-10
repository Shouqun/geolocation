import json
import geolocation

class GeoLocation:
  
  def __init__ (self):
    self.__wifi_data = [];

  def getGeoRequestData(self, version= "1.1.0",
                        host = "maps.google.com",
                        request_address = "true",
                        access_token = "access_token"):
    #get the wifi provider data 
    wifi_data = geolocation.get_wireless_provider();
  
    json_text = self.formatJsonRequest(wifi_data, version, host,
                            request_address, access_token)
    #return the JSON request string
    return json_text;

  def formatJsonRequest(self, wifi_data, version,
                        host, request_address, access_token):
    #get the header section
    data_object = self.formatRequestHeader(version, host,
                            request_address, access_token);

    #pack the wifi_towers datalist
    wifi_list = [];
    for wifi in wifi_data:
      wifi_item= {};
      wifi_item["mac_address"] = wifi["MAC"]
      wifi_item["signal_strength"] = wifi["Quality"]
      wifi_item["age"] = 0;
      wifi_list.append(wifi_item);
    
    data_object["wifi_towers"] = wifi_list

    print json.dumps(data_object);
    

  def formatRequestHeader (self, version, host,
                           request_address, access_token):
    #format the result map header section
    result_map = {"version": version,
                "host": host,
                "access_token" : access_token,
                "request_address": request_address}
    return result_map;
    

if __name__ == "__main__":
  geo = GeoLocation();
  geo.getGeoRequestData();

