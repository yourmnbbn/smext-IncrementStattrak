#ifndef __INCREMENTSTATTRAK_GCHELPER_HPP__
#define __INCREMENTSTATTRAK_GCHELPER_HPP__

#include <memory>
#include <steam_api.h>
#include <isteamgamecoordinator.h>


struct GCMsgHdr_t
{
	uint32	m_eMsg;					// The message type
	uint32	m_nSrcGCDirIndex;		// The GC index that this message was sent from (set to the same as the current GC if not routed through another GC)
};

class GCHelper
{
public:
    bool Init()
    {
        m_pGameCoordinator = (ISteamGameCoordinator*)SteamGameServerClient()->GetISteamGenericInterface(SteamGameServer_GetHSteamUser(), SteamGameServer_GetHSteamPipe(), STEAMGAMECOORDINATOR_INTERFACE_VERSION);
        m_Inited = m_pGameCoordinator != nullptr;
        return m_Inited;
    }

    bool BInited()
    {
        return m_Inited;
    }

    bool SendMessageToGC(uint32_t type, google::protobuf::Message& msg)
    {
        if(!m_pGameCoordinator)
            return false;
        
        auto size = msg.ByteSize() + sizeof(GCMsgHdr_t);
        std::unique_ptr<char[]> memBlock = std::make_unique<char[]>(size);

        type |= (1 << 31);

        GCMsgHdr_t* header = (GCMsgHdr_t*)memBlock.get();
        header->m_eMsg = type;
        header->m_nSrcGCDirIndex = 0;

        msg.SerializeToArray(memBlock.get() + sizeof(GCMsgHdr_t), size - sizeof(GCMsgHdr_t));
        return m_pGameCoordinator->SendMessage(type, memBlock.get(), size) == k_EGCResultOK;
    }

private:
    ISteamGameCoordinator*	m_pGameCoordinator = nullptr;
    bool                    m_Inited = false;
}g_GCHelper;


GCHelper& GetGCHelper()
{
    return g_GCHelper;
}

#endif