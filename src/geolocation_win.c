#include <windows.h>
#include <wlanapi.h>
#include <ObjBase.h>
#include <wctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <Python.h>

#include "geolocation.h"

#pragma comment(lib, "wlanapi.lib")
#pragma comment(lib, "ole32.lib")

#ifndef UNICODE
#define UNICODE
#endif


PWLAN_INTERFACE_INFO_LIST _get_wlan_interface_list(HANDLE hClient)
{
  DWORD dwResult;
  PWLAN_INTERFACE_INFO_LIST pIfList = NULL;

  dwResult = WlanEnumInterfaces(hClient, NULL, &pIfList);
  if (dwResult != ERROR_SUCCESS) {
    wprintf(L"WlanEnumInterfaces failed with error: %u\n", dwResult);
  } else {
    wprintf(L"Num Entries: %lu\n", pIfList->dwNumberOfItems);
    wprintf(L"Current Index: %lu\n", pIfList->dwIndex);
  }
  return pIfList;
}


void _print_interface_information(PWLAN_INTERFACE_INFO pIfInfo, int i)
{
  int iRet = 0;

  WCHAR GuidString[39] = {0};

  iRet = StringFromGUID2(&pIfInfo->InterfaceGuid, (LPOLESTR) &GuidString, 
    sizeof(GuidString)/sizeof(*GuidString)); 

  if (iRet == 0)
    wprintf(L"StringFromGUID2 failed\n");
  else {
    wprintf(L"  InterfaceGUID[%d]: %ws\n",i, GuidString);
  }

  wprintf(L"  Interface Description[%d]: %ws", i, 
    pIfInfo->strInterfaceDescription);
  wprintf(L"\n");
  wprintf(L"  Interface State[%d]:\t ", i);
  switch (pIfInfo->isState) {      
      case wlan_interface_state_not_ready:
        wprintf(L"Not ready\n");
        break;
      case wlan_interface_state_connected:
        wprintf(L"Connected\n");
        break;
      case wlan_interface_state_ad_hoc_network_formed:
        wprintf(L"First node in a ad hoc network\n");
        break;
      case wlan_interface_state_disconnecting:
        wprintf(L"Disconnecting\n");
        break;
      case wlan_interface_state_disconnected:
        wprintf(L"Not connected\n");
        break;
      case wlan_interface_state_associating:
        wprintf(L"Attempting to associate with a network\n");
        break;
      case wlan_interface_state_discovering:
        wprintf(L"Auto configuration is discovering settings for the network\n");
        break;
      case wlan_interface_state_authenticating:
        wprintf(L"In process of authenticating\n");
        break;
      default:
        wprintf(L"Unknown state %ld\n", pIfInfo->isState);
        break;
  }
  wprintf(L"\n");
}

void _query_wireless_provider(HANDLE hClient, 
                              PWLAN_INTERFACE_INFO pIfInfo,
                              PWirelessDataChain chain)
{
  //////////////////////////////////////////////////////////////////////////
  //query BSSID and MAC address
  char ssid[DOT11_SSID_MAX_LENGTH +1];
  char mac[32];
  DWORD dwResult;
  PWLAN_BSS_LIST pWlanBssList;
  PWLAN_BSS_ENTRY pWlanBssEntry;
  int  d, k, m;
  int iRSSI;

  dwResult = WlanGetNetworkBssList(hClient,
    &pIfInfo->InterfaceGuid,
    NULL,//&pBssEntry->dot11Ssid,
    dot11_BSS_type_any,
    FALSE,//pBssEntry->bSecurityEnabled,
    NULL,
    &pWlanBssList);
  if (dwResult != ERROR_SUCCESS) {
    printf("  Error get BSS List\n");
  } else {
    printf("  Num of BSS Entry: %d\n", pWlanBssList->dwNumberOfItems);
    for(d = 0; d<pWlanBssList->dwNumberOfItems; d++) {
      pWlanBssEntry = (WLAN_BSS_ENTRY*) & pWlanBssList->wlanBssEntries[d];

      for (k = 0; k < pWlanBssEntry->dot11Ssid.uSSIDLength; k++) {
        ssid[k] = (char) pWlanBssEntry->dot11Ssid.ucSSID[k];
      }
      ssid[pWlanBssEntry->dot11Ssid.uSSIDLength] = '\0';
      
      printf("  SSID: %s, %d\n", ssid, strlen(ssid));

      printf("    MAC address of BSS: ");
      
      sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x", 
                    pWlanBssEntry->dot11Bssid[0],
                    pWlanBssEntry->dot11Bssid[1],
                    pWlanBssEntry->dot11Bssid[2],
                    pWlanBssEntry->dot11Bssid[3],
                    pWlanBssEntry->dot11Bssid[4],
                    pWlanBssEntry->dot11Bssid[5]);
      printf(" %s\n", mac);


      printf("   Signal Quality[%u]\n", pWlanBssEntry->uLinkQuality);

      memset(ssid, sizeof(ssid), 0);
      create_new_wireless_data(chain, ssid, mac, pWlanBssEntry->uLinkQuality);
    }
  }
}

WirelessData* system_get_wireless_provider_win()
{
  PWirelessDataChain resultChain = NULL;
  HANDLE hClient = NULL;
  DWORD dwMaxClient = 2;
  DWORD dwCurVersion = 0;
  DWORD dwResult = 0;
  DWORD dwRetVal = 0;
  
  unsigned int i;

  PWLAN_INTERFACE_INFO_LIST pIfList = NULL;
  PWLAN_INTERFACE_INFO pIfInfo = NULL;

  int iRSSI = 0;

  resultChain = allocate_wireless_data_chain();

  dwResult = WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &hClient);
  if (dwResult != ERROR_SUCCESS) {
    wprintf(L"WlanOpenHandle failed with error: %u\n", dwResult);
    return resultChain;
    // You can use FormatMessage here to find out why the function failed
  } 
  
  pIfList = _get_wlan_interface_list(hClient);
  
  if (pIfList == NULL)
    return resultChain;
   
  for (i = 0; i < (int) pIfList->dwNumberOfItems; i++) {
    pIfInfo = (WLAN_INTERFACE_INFO *) &pIfList->InterfaceInfo[i];
   
    _query_wireless_provider(hClient, pIfInfo, resultChain);
  }

  return resultChain;
}
