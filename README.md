# Gaming-Lag-Switch
Actually work Lag Switch for EFT, DayZ and etc. games (UDP/TCP inbound/outbound packets)

<img src="https://cdn.discordapp.com/attachments/439961603744464899/1143544993395515392/image.png"></img>

### Usage
Use Visual Studio 2019, be sure to add to the DirectX project.
Select "Release" and compile. Compiled files goes in root x64 directory.<br>
**Please star and fork this repo!**

### Example block/unblock inbound udp packets
```cpp
#include <fwpmu.h>
#pragma comment (lib, "fwpuclnt.lib")

HANDLE engineHandle;
if (DWORD result = FwpmEngineOpen0(NULL, RPC_C_AUTHN_WINNT, NULL, NULL, &engineHandle))
{
    logs.AddLog(xor ("\n[-] Error 1 - 0x%s"), to_string(result));
}
FWPM_FILTER0 filter;
SecureZeroMemory(&filter, sizeof(filter));
FWPM_FILTER_CONDITION0 conditions[2];
conditions[0].fieldKey = FWPM_CONDITION_IP_PROTOCOL;
conditions[0].conditionValue.type = FWP_UINT8;
conditions[0].conditionValue.uint8 = 17; //rfc 1700
conditions[0].matchType = FWP_MATCH_EQUAL;
conditions[1].fieldKey = FWPM_CONDITION_DIRECTION;
conditions[1].conditionValue.type = FWP_UINT32;
conditions[1].conditionValue.uint32 = FWP_DIRECTION_INBOUND;
conditions[1].matchType = FWP_MATCH_EQUAL;
filter.action.type = FWP_ACTION_BLOCK;
filter.displayData.name = GetWC("GLSblockudpINBOUND");
filter.layerKey = FWPM_LAYER_DATAGRAM_DATA_V4;
filter.numFilterConditions = 2;
filter.weight.type = FWP_EMPTY;
filter.filterCondition = conditions;

//Block
DWORD result = FwpmFilterAdd0(engineHandle, &filter, NULL, &filter.filterId);

//Unblock
DWORD result = FwpmFilterDeleteById0(engineHandle, filter.filterId);
```

## Me
in **Discord**: xvorost#5394<br>
