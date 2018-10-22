#include <iostream>
#include <windows.h>
#include <mmdeviceapi.h>
#include "inc.h"
#include <Functiondiscoverykeys_devpkey.h>
#include "IPolicyConfig.h"

using namespace std;

HRESULT SetDefaultAudioPlaybackDevice(LPCWSTR devID)
{
	IPolicyConfigVista *pPolicyConfig;
	ERole reserved = eConsole;

	HRESULT hr = CoCreateInstance(__uuidof(CPolicyConfigVistaClient), NULL, CLSCTX_ALL, __uuidof(IPolicyConfigVista), (LPVOID *)&pPolicyConfig);
	if (SUCCEEDED(hr))
	{
		hr = pPolicyConfig->SetDefaultEndpoint(devID, reserved);
		pPolicyConfig->Release();
	}
	return hr;
}

std::wstring GetEndpointName(IMMDevice* pEndpoint)
{
	IPropertyStore *pProps = NULL;
	THROW_IF_FAILED(pEndpoint->OpenPropertyStore(STGM_READ, &pProps));

	PROPVARIANT varName;
	PropVariantInit(&varName);

	THROW_IF_FAILED(pProps->GetValue(PKEY_Device_FriendlyName, &varName));
	return { varName.pwszVal };
}

IMMDevice* GetEndpointByName(std::wstring const name)
{
	IMMDeviceEnumerator* pEnumerator;
	THROW_IF_FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator));
	IMMDeviceCollection* pEndpoints;
	THROW_IF_FAILED(pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pEndpoints));

	UINT count;
	pEndpoints->GetCount(&count);

	IMMDevice* pDefaultEndpoint;
	THROW_IF_FAILED(pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDefaultEndpoint));

	for (UINT i = 0; i < count; i++)
	{
		IMMDevice* pEndpoint;
		THROW_IF_FAILED(pEndpoints->Item(i, &pEndpoint));

		if (GetEndpointName(pEndpoint) == name)
		{
			return pEndpoint;
		}
	}
	return nullptr;
}

void ToggleSoundDeviceEndpoint()
{
	//THROW_IF_FAILED(CoInitialize(nullptr));

	IMMDeviceEnumerator* pEnumerator;
	THROW_IF_FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator));

	IMMDevice* pCurrentDefaultEndpoint;
	THROW_IF_FAILED(pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pCurrentDefaultEndpoint));
	
	auto targetEndpointName = [&]() {
		// Endpoint 2: "Speakers (2- High Definition Audio Device)" ({ 0.0.0.00000000 }.{2cf3933e - 2a95 - 4442 - a391 - eb3fddf3bf41})
		// Endpoint 3: "Headset Earphone (HyperX 7.1 Audio)" ({0.0.0.00000000}.{5823122c-a24d-403f-b2cf-ead92b06e275})
		auto speakersName = std::wstring(L"Speakers (2- High Definition Audio Device)");
		auto headphonesName = std::wstring(L"Headset Earphone (HyperX 7.1 Audio)");

		auto currentDefaultEndpointName = GetEndpointName(pCurrentDefaultEndpoint);

		if (currentDefaultEndpointName == speakersName)
		{
			return headphonesName;
		}
		if (currentDefaultEndpointName == headphonesName)
		{
			return speakersName;
		}
		return headphonesName;
	}();

	auto targetEndpoint = GetEndpointByName(targetEndpointName);

	LPWSTR id;
	THROW_IF_FAILED(targetEndpoint->GetId(&id));

	SetDefaultAudioPlaybackDevice(id);
}
