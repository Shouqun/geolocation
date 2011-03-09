#include <Python.h>

#include "geolocation.h"

_system_get_wireless_provider system_get_wireless_provider = NULL;




static PyObject* geolocation_get_wireless_provider(PyObject *self,
                                                   PyObject *args)
{
  PyObject *result, *item;
  int i=0;
  WirelessData *dataItem;

  PWirelessDataChain dataChain = system_get_wireless_provider();
  int count = wireless_data_count(dataChain);
  
  printf("Count: %d\n", count);
  
  result = PyList_New(0);
  
  dataItem = dataChain->next;
  while(dataItem != NULL) {
    //the item value
    item = Py_BuildValue("{s:s, s:s, s:i}",
                            "SSID", dataItem->ssid,
                            "MAC", dataItem->mac,
                            "Quality", dataItem->quality);
    PyList_Append(result, item);

    dataItem = dataItem->next;
  }
  
  relase_wireless_data_chain(dataChain);

  return Py_BuildValue("O", result);
}

static PyMethodDef GeolocationMethods[] = {
  {"get_wireless_provider", geolocation_get_wireless_provider, METH_NOARGS, "get wireless providers"},
  {NULL, NULL, 0, NULL}
};


PyMODINIT_FUNC initgeolocation(void )
{
  system_get_wireless_provider = system_get_wireless_provider_win;
  (void) Py_InitModule ("geolocation", GeolocationMethods);
}

int main(int argc, char *argv[])
{
  Py_SetProgramName(argv[0]);
  
  Py_Initialize();

  initgeolocation();
}

