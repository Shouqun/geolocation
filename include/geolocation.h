#ifndef _GEO_LOCATION_H
#define _GEO_LOCATION_H



#define GEO_LOCATION_UNUSED(x) (void)x

typedef struct _WirelessData{
  char *ssid;
  char *mac;
  int  quality;
  struct _WirelessData *next;
}WirelessData, *PWirelessDataChain;

PWirelessDataChain allocate_wireless_data_chain();

void relase_wireless_data_chain(PWirelessDataChain);

void create_new_wireless_data(PWirelessDataChain chain, 
                              char *ssid,
                              char *mac,
                              int quality);

int wireless_data_count(PWirelessDataChain chain);



typedef WirelessData* (*_system_get_wireless_provider)(void);

WirelessData* system_get_wireless_provider_win();


#endif