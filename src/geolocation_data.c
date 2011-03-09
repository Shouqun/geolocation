#include <Python.h>
#include "geolocation.h"

PWirelessDataChain allocate_wireless_data_chain()
{
  PWirelessDataChain chain = PyMem_New(WirelessData, 1);

  chain->mac = NULL;
  chain->quality = 0;
  chain->ssid = NULL;
  chain->next = NULL;

  return chain;
}

void create_new_wireless_data(PWirelessDataChain chain, 
                              char *ssid,
                              char *mac,
                              int quality)
{
  WirelessData *data;

  if (mac == NULL )
    return;

  data = allocate_wireless_data_chain();

  data->mac = PyMem_Malloc( strlen(mac)+1 );
  strcpy(data->mac, mac);

  if (ssid == NULL) {
    ssid = "";
  } 
  data->ssid =PyMem_Malloc( strlen(ssid)+1 );
  strcpy( data->ssid, ssid);

  data->quality = quality;

  //insert into current chain
  data->next = chain->next;
  chain->next = data;
}


void release_wireless_data(WirelessData *data)
{
  if ( data == NULL )
    return;

  if (data->mac != NULL)
    PyMem_Free(data->mac);
  if (data->ssid != NULL)
    PyMem_Free(data->ssid);
  PyMem_Del(data);
}


void relase_wireless_data_chain(PWirelessDataChain chain)
{
  WirelessData *item;
  while ( (item = chain->next) != NULL) {    
    chain->next = item->next;

    release_wireless_data(item);
  }

  //free the chain header
  release_wireless_data(chain);
}

int wireless_data_count(PWirelessDataChain chain)
{
  int count = 0;
  WirelessData *item = chain->next;
  while( (item) != NULL) {
    count++;
    item = item->next;
  }
  return count;
}
