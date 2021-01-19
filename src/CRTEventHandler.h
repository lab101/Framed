

#pragma once

#include <ole2.h>
#include <rtscom.h>
#include <rtscom_i.c>

typedef unsigned long	uint32;





// Current settings
bool g_bInverted = false;
float g_Pressure = 0.0f;


// Stylus handler
bool g_bTriedToCreateRTSHandler = false;
class CRTSEventHandler* g_pRTSHandler = NULL;
IRealTimeStylus* g_pRTSStylus = NULL;

// Tablet context to pressure sensitivity
const uint32 c_nMaxContexts = 4;
uint32	g_nContexts = 0;

struct
{
	TABLET_CONTEXT_ID	iTabletContext;
	uint32				iPressure;
	float				PressureRcp;
} g_lContexts[c_nMaxContexts];









//-------------------------------------------------------------------------------------------------
// RealTimeStylus syncevent plugin
class CRTSEventHandler : public IStylusSyncPlugin
{
public:

	CRTSEventHandler() : m_nRef(1), m_pPunkFTMarshaller(NULL) { }
	virtual ~CRTSEventHandler()
	{
		if (m_pPunkFTMarshaller != NULL)
			m_pPunkFTMarshaller->Release();
	}

	// IStylusSyncPlugin inherited methods

	// Methods whose data we use
	STDMETHOD(Packets)(IRealTimeStylus* pStylus, const StylusInfo* pStylusInfo, ULONG nPackets, ULONG nPacketBuf, LONG* pPackets, ULONG* nOutPackets, LONG** ppOutPackets)
	{
		uint32 iCtx = c_nMaxContexts;
		for (uint32 i = 0; i < g_nContexts; i++)
		{
			if (g_lContexts[i].iTabletContext == pStylusInfo->tcid)
			{
				iCtx = i;
				break;
			}
		}

		// If we are not getting pressure values, just ignore it
		if (iCtx >= c_nMaxContexts)
			return S_OK;

		// Get properties
		ULONG nProps = nPacketBuf / nPackets;

		// Pointer to the last packet in the batch - we don't really care about intermediates for this example
		LONG* pLastPacket = pPackets + (nPacketBuf - nProps);

		g_Pressure = pLastPacket[g_lContexts[iCtx].iPressure] * g_lContexts[iCtx].PressureRcp;
		g_bInverted = (pStylusInfo->bIsInvertedCursor != 0);

		return S_OK;
	}
	STDMETHOD(DataInterest)(RealTimeStylusDataInterest* pEventInterest)
	{
		*pEventInterest = (RealTimeStylusDataInterest)(RTSDI_Packets);
		return S_OK;
	}

	// Methods you can add if you need the alerts - don't forget to change DataInterest!
	STDMETHOD(StylusDown)(IRealTimeStylus*, const StylusInfo*, ULONG, LONG* _pPackets, LONG**) { return S_OK; }
	STDMETHOD(StylusUp)(IRealTimeStylus*, const StylusInfo*, ULONG, LONG* _pPackets, LONG**) { return S_OK; }
	STDMETHOD(RealTimeStylusEnabled)(IRealTimeStylus*, ULONG, const TABLET_CONTEXT_ID*) { return S_OK; }
	STDMETHOD(RealTimeStylusDisabled)(IRealTimeStylus*, ULONG, const TABLET_CONTEXT_ID*) { return S_OK; }
	STDMETHOD(StylusInRange)(IRealTimeStylus*, TABLET_CONTEXT_ID, STYLUS_ID) { return S_OK; }
	STDMETHOD(StylusOutOfRange)(IRealTimeStylus*, TABLET_CONTEXT_ID, STYLUS_ID) { return S_OK; }
	STDMETHOD(InAirPackets)(IRealTimeStylus*, const StylusInfo*, ULONG, ULONG, LONG*, ULONG*, LONG**) { return S_OK; }
	STDMETHOD(StylusButtonUp)(IRealTimeStylus*, STYLUS_ID, const GUID*, POINT*) { return S_OK; }
	STDMETHOD(StylusButtonDown)(IRealTimeStylus*, STYLUS_ID, const GUID*, POINT*) { return S_OK; }
	STDMETHOD(SystemEvent)(IRealTimeStylus*, TABLET_CONTEXT_ID, STYLUS_ID, SYSTEM_EVENT, SYSTEM_EVENT_DATA) { return S_OK; }
	STDMETHOD(TabletAdded)(IRealTimeStylus*, IInkTablet*) { return S_OK; }
	STDMETHOD(TabletRemoved)(IRealTimeStylus*, LONG) { return S_OK; }
	STDMETHOD(CustomStylusDataAdded)(IRealTimeStylus*, const GUID*, ULONG, const BYTE*) { return S_OK; }
	STDMETHOD(Error)(IRealTimeStylus*, IStylusPlugin*, RealTimeStylusDataInterest, HRESULT, LONG_PTR*) { return S_OK; }
	STDMETHOD(UpdateMapping)(IRealTimeStylus*) { return S_OK; }

	// IUnknown methods
	STDMETHOD_(ULONG, AddRef)()
	{
		return InterlockedIncrement(&m_nRef);
	}
	STDMETHOD_(ULONG, Release)()
	{
		ULONG nNewRef = InterlockedDecrement(&m_nRef);
		if (nNewRef == 0)
			delete this;

		return nNewRef;
	}
	STDMETHOD(QueryInterface)(REFIID riid, LPVOID* ppvObj)
	{
		if ((riid == IID_IStylusSyncPlugin) || (riid == IID_IUnknown))
		{
			*ppvObj = this;
			AddRef();
			return S_OK;
		}
		else if ((riid == IID_IMarshal) && (m_pPunkFTMarshaller != NULL))
		{
			return m_pPunkFTMarshaller->QueryInterface(riid, ppvObj);
		}

		*ppvObj = NULL;
		return E_NOINTERFACE;
	}

	LONG m_nRef;					// COM object reference count
	IUnknown* m_pPunkFTMarshaller;  // free-threaded marshaller
};


//-------------------------------------------------------------------------------------------------
void ReleaseRTS()
{
	if (g_pRTSStylus)
	{
		g_pRTSStylus->Release();
		g_pRTSStylus = NULL;
	}

	if (g_pRTSHandler)
	{
		g_pRTSHandler->Release();
		g_pRTSHandler = NULL;
	}
}


//-------------------------------------------------------------------------------------------------
// Create stylus
bool CreateRTS(HWND hWnd)
{
	// Release, just in case
	ReleaseRTS();

	// Create stylus
	HRESULT hr = CoCreateInstance(CLSID_RealTimeStylus, NULL, CLSCTX_ALL, IID_PPV_ARGS(&g_pRTSStylus));
	if (FAILED(hr))
		return false;

	// Attach RTS object to a window
	hr = g_pRTSStylus->put_HWND((HANDLE_PTR)hWnd);
	if (FAILED(hr))
	{
		ReleaseRTS();
		return false;
	}

	// Create eventhandler
	g_pRTSHandler = new CRTSEventHandler();

	// Create free-threaded marshaller for this object and aggregate it.
	hr = CoCreateFreeThreadedMarshaler(g_pRTSHandler, &g_pRTSHandler->m_pPunkFTMarshaller);
	if (FAILED(hr))
	{
		ReleaseRTS();
		return false;
	}

	// Add handler object to the list of synchronous plugins in the RTS object.
	hr = g_pRTSStylus->AddStylusSyncPlugin(0, g_pRTSHandler);
	if (FAILED(hr))
	{
		ReleaseRTS();
		return false;
	}

	// Set data we want - we're not actually using all of this, but we're gonna get X and Y anyway so might as well set it
	GUID lWantedProps[] = { GUID_PACKETPROPERTY_GUID_X, GUID_PACKETPROPERTY_GUID_Y, GUID_PACKETPROPERTY_GUID_NORMAL_PRESSURE };
	g_pRTSStylus->SetDesiredPacketDescription(3, lWantedProps);
	g_pRTSStylus->put_Enabled(true);

	// Detect what tablet context IDs will give us pressure data
	{
		g_nContexts = 0;
		ULONG nTabletContexts = 0;
		TABLET_CONTEXT_ID* piTabletContexts;
		HRESULT res = g_pRTSStylus->GetAllTabletContextIds(&nTabletContexts, &piTabletContexts);
		for (ULONG i = 0; i < nTabletContexts; i++)
		{
			IInkTablet* pInkTablet;
			if (SUCCEEDED(g_pRTSStylus->GetTabletFromTabletContextId(piTabletContexts[i], &pInkTablet)))
			{
				float ScaleX, ScaleY;
				ULONG nPacketProps;
				PACKET_PROPERTY* pPacketProps;
				g_pRTSStylus->GetPacketDescriptionData(piTabletContexts[i], &ScaleX, &ScaleY, &nPacketProps, &pPacketProps);

				for (ULONG j = 0; j < nPacketProps; j++)
				{
					if (pPacketProps[j].guid != GUID_PACKETPROPERTY_GUID_NORMAL_PRESSURE)
						continue;

					g_lContexts[g_nContexts].iTabletContext = piTabletContexts[i];
					g_lContexts[g_nContexts].iPressure = j;
					g_lContexts[g_nContexts].PressureRcp = 1.0f / pPacketProps[j].PropertyMetrics.nLogicalMax;
					g_nContexts++;
					break;
				}
				CoTaskMemFree(pPacketProps);
			}
		}

		// If we can't get pressure information, no use in having the tablet context
		if (g_nContexts == 0)
		{
			ReleaseRTS();
			return false;
		}
	}

	return true;
}

