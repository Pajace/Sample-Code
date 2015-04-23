#include <WinSock2.h>
#include <IPHlpApi.h>
#include <iostream>
#include <iomanip>

#pragma  comment(lib, "IPHLPAPI.lib")

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

using namespace std;

// PIP_ADAPTER_INFO
bool PrintIPAdapterInfoList(){
	// 宣告一個 IP_ADAPTER_INFO 的 Point
	PIP_ADAPTER_INFO pIpAdapterInfo = NULL;

	// 宣告初始化的記憶體大小為一個 IP_ADAPTER_INFO 長度
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);

	// 跟系統要求分配 heap memory (他是一棵樹喔)
	pIpAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(sizeof(IP_ADAPTER_INFO));
	if (pIpAdapterInfo == NULL) {
		cerr << "Error allocating memory needed to call GetAdaptersInfo()" << endl;
		return false;
	}

	// 如果電腦上不只一張網卡，通常都會 buffer overflow, ulOutBufLen 裡會有需要的大小
	// 因此把剛剛分配到的記憶體給歸還, 再根據 ulOutBufLen 要一次新的大小
	if (GetAdaptersInfo(pIpAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
		FREE(pIpAdapterInfo);
		pIpAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(ulOutBufLen);
		if (pIpAdapterInfo == NULL){
			cerr << "Error allocating memory needed to call GetAdaptersInfo()" << endl;
			return false;
		}
	}

	// 成功要到後，再次確認沒有任何錯誤就可以對這棵樹 (IP_ADAPTER_INFO) 進行操作啦
	if (GetAdaptersInfo(pIpAdapterInfo, &ulOutBufLen) == NO_ERROR) {
		PIP_ADAPTER_INFO pAdapter = pIpAdapterInfo;
		while(pAdapter) {
			cout << "Description: " << pAdapter->Description << ", Index = " << pAdapter->Index << endl;
			pAdapter = pAdapter->Next;
		}
	}

	// 最要一定要記得把空間歸還給系統，不然會造成 memory leak
	if (pIpAdapterInfo) {
		FREE(pIpAdapterInfo);
		pIpAdapterInfo = NULL;
	}
}

bool GetAdapterInfoPointByIndex(int index, PIP_ADAPTER_INFO *pIpAdapterInfoHead, PIP_ADAPTER_INFO *pIpAdapterInfo) {

	PIP_ADAPTER_INFO pAdapterInfo = NULL;

	ULONG uLoutBufLen = sizeof(IP_ADAPTER_INFO);
	pAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(sizeof(IP_ADAPTER_INFO));
	if (pAdapterInfo == NULL) {
		cout << "Error allocating memory needed to call Win32 API - GetAdaptersinfo" << endl;
		return false;
	}

	if (GetAdaptersInfo(pAdapterInfo, &uLoutBufLen) == ERROR_BUFFER_OVERFLOW) {
		FREE(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(uLoutBufLen);
		if (pAdapterInfo == NULL){
			cout << "Error allocating memory needed to call Win32 API - GetAdaptersInfo()" << endl;
			return false;
		}
	}

	if (GetAdaptersInfo(pAdapterInfo, &uLoutBufLen) == NO_ERROR) {
		PIP_ADAPTER_INFO pAdapter = pAdapterInfo;
		while(pAdapter) {
			if (pAdapter->Index==index){
				*pIpAdapterInfo = pAdapter;
				*pIpAdapterInfoHead = pAdapterInfo;
				return true;
			} else
				pAdapter = pAdapter->Next;
		}
	}

	if (pAdapterInfo) {
		FREE(pAdapterInfo);
		pAdapterInfo = NULL;
	}
	return false;
}

void PrintIPAdapterInfo(PIP_ADAPTER_INFO pIpAdapterInfo){
	cout << "Adapter Name  : " << pIpAdapterInfo->AdapterName << endl;

	cout << "Mac Address   : " ;
	cout.setf(ios::hex, ios::basefield);
	for(int i=0; i<pIpAdapterInfo->AddressLength; i++) {
		cout << uppercase << (int)pIpAdapterInfo->Address[i];
		if (i!=pIpAdapterInfo->AddressLength-1)
			cout << "-";
		else
			cout << endl;
	}
	cout.unsetf(ios::hex);

	//cout << "Address Length: " << pIpAdapterInfo->AddressLength << endl;

	cout << "ComboIndex    : " << pIpAdapterInfo->ComboIndex << endl;
	cout << "Gateway IP    : " << pIpAdapterInfo->GatewayList.IpAddress.String << endl;
	cout << "Gateway Mask  : " << pIpAdapterInfo->GatewayList.IpMask.String << endl;
	cout << "Description   : " << pIpAdapterInfo->Description << endl;
	cout << "Index         : " << pIpAdapterInfo->Index << endl;
	cout << "Type          : ";
	switch (pIpAdapterInfo->Type)
	{
	case MIB_IF_TYPE_OTHER:
		cout << "Other\n" << endl;
		break;
	case MIB_IF_TYPE_ETHERNET:
		cout << "Ethernet\n" << endl;
		break;
	case MIB_IF_TYPE_TOKENRING:
		cout << "Token Ring\n" << endl;
		break;
	case MIB_IF_TYPE_FDDI:
		cout << "FDDI\n" << endl;
		break;
	case MIB_IF_TYPE_PPP:
		cout << "PPP\n" << endl;
		break;
	case MIB_IF_TYPE_LOOPBACK:
		cout << "Lookback\n" << endl;
		break;
	case MIB_IF_TYPE_SLIP:
		cout << "Slip\n" << endl;
		break;
	default:
		cout << "Unknown type ["  << pIpAdapterInfo->Type << "]" << endl;
		break;
	}

}

int main(){
	PIP_ADAPTER_INFO pIpAdapterInfo = NULL;
	PIP_ADAPTER_INFO pIpAdapterInfoHead = NULL;

	PrintIPAdapterInfoList();

	cout << "請選擇你所要檢視的網卡 index :";
	int index = 0;
	cin >> index;

	bool isGet = GetAdapterInfoPointByIndex(index, &pIpAdapterInfoHead, &pIpAdapterInfo);
	if (isGet)
		PrintIPAdapterInfo(pIpAdapterInfo);
	else
		cout << "你輸入的網卡 index 錯誤!!" << endl;

	if (pIpAdapterInfoHead) {
		FREE(pIpAdapterInfoHead);
		pIpAdapterInfoHead = NULL;
	}

	system("Pause");
}