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
	// �ŧi�@�� IP_ADAPTER_INFO �� Point
	PIP_ADAPTER_INFO pIpAdapterInfo = NULL;

	// �ŧi��l�ƪ��O����j�p���@�� IP_ADAPTER_INFO ����
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);

	// ��t�έn�D���t heap memory (�L�O�@�ʾ��)
	pIpAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(sizeof(IP_ADAPTER_INFO));
	if (pIpAdapterInfo == NULL) {
		cerr << "Error allocating memory needed to call GetAdaptersInfo()" << endl;
		return false;
	}

	// �p�G�q���W���u�@�i���d�A�q�`���| buffer overflow, ulOutBufLen �̷|���ݭn���j�p
	// �]��������t�쪺�O���鵹�k��, �A�ھ� ulOutBufLen �n�@���s���j�p
	if (GetAdaptersInfo(pIpAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
		FREE(pIpAdapterInfo);
		pIpAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(ulOutBufLen);
		if (pIpAdapterInfo == NULL){
			cerr << "Error allocating memory needed to call GetAdaptersInfo()" << endl;
			return false;
		}
	}

	// ���\�n���A�A���T�{�S��������~�N�i�H��o�ʾ� (IP_ADAPTER_INFO) �i��ާ@��
	if (GetAdaptersInfo(pIpAdapterInfo, &ulOutBufLen) == NO_ERROR) {
		PIP_ADAPTER_INFO pAdapter = pIpAdapterInfo;
		while(pAdapter) {
			cout << "Description: " << pAdapter->Description << ", Index = " << pAdapter->Index << endl;
			pAdapter = pAdapter->Next;
		}
	}

	// �̭n�@�w�n�O�o��Ŷ��k�ٵ��t�ΡA���M�|�y�� memory leak
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

	cout << "�п�ܧA�ҭn�˵������d index :";
	int index = 0;
	cin >> index;

	bool isGet = GetAdapterInfoPointByIndex(index, &pIpAdapterInfoHead, &pIpAdapterInfo);
	if (isGet)
		PrintIPAdapterInfo(pIpAdapterInfo);
	else
		cout << "�A��J�����d index ���~!!" << endl;

	if (pIpAdapterInfoHead) {
		FREE(pIpAdapterInfoHead);
		pIpAdapterInfoHead = NULL;
	}

	system("Pause");
}