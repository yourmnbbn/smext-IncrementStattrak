/**
 * vim: set ts=4 :
 * =============================================================================
 * SourceMod Sample Extension
 * Copyright (C) 2004-2008 AlliedModders LLC.  All rights reserved.
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, AlliedModders LLC gives you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2," the
 * "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, AlliedModders LLC grants
 * this exception to all derivative works.  AlliedModders LLC defines further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>.
 *
 * Version: $Id$
 */

#include "extension.h"
#include "is_message.pb.h"
#include "GCHelper.hpp"
#include <server_class.h>
#include <iserverunknown.h>
#include <iservernetworkable.h>

/**
 * @file extension.cpp
 * @brief Implement extension code here.
 */

IncrementStattrak g_IncrementStattrak;		/**< Global singleton for extension's main interface */

SMEXT_LINK(&g_IncrementStattrak);
SH_DECL_HOOK1_void(IServerGameDLL, GameServerSteamAPIActivated, SH_NOATTRIB, 0, bool);

#ifdef POSIX
	#define GetItemIDOffset 15
#else
	#define GetItemIDOffset 14
#endif

//https://github.com/alliedmodders/sourcemod/blob/0c8e6e29184bf58851954019a2060d84f0c556f9/extensions/sdkhooks/util.cpp#L37
bool UTIL_ContainsDataTable(SendTable* pTable, const char* name)
{
	const char* pname = pTable->GetName();
	int props = pTable->GetNumProps();
	SendProp* prop;
	SendTable* table;

	if (pname && strcmp(name, pname) == 0)
		return true;

	for (int i = 0; i < props; i++)
	{
		prop = pTable->GetProp(i);

		if ((table = prop->GetDataTable()) != nullptr)
		{
			pname = table->GetName();
			if (pname && strcmp(name, pname) == 0)
			{
				return true;
			}

			if (UTIL_ContainsDataTable(table, name))
			{
				return true;
			}
		}
	}

	return false;
}

void Hook_GameServerSteamAPIActivated(bool bActivated)
{
    if(!bActivated || !GetGCHelper().Init())
    {
        smutils->LogError(myself, "Steam api initialization failed!");
    }
}

static cell_t IS_IncrementStattrak(IPluginContext *pContext, const cell_t *params)
{
    if(!GetGCHelper().BInited())
    {
        return pContext->ThrowNativeError("Steam api initialization failed!");
    }

	//Following code partialy taken from https://github.com/komashchenko/PTaH/blob/fa943881e6a792bf5fe7f27d75397431346e20f2/natives.cpp#L175
    CBaseEntity* pEntity = gamehelpers->ReferenceToEntity(params[3]);
	if (!pEntity)
	{
		return pContext->ThrowNativeError("Entity %d is invalid", params[3]);
	}

	IServerNetworkable* pNet = reinterpret_cast<IServerUnknown*>(pEntity)->GetNetworkable();
	if (!pNet || !UTIL_ContainsDataTable(pNet->GetServerClass()->m_pTable, "DT_EconEntity"))
	{
		return pContext->ThrowNativeError("Entity %d is not CEconEntity", params[3]);
	}

	static uint32_t offset = 0;
	if (offset == 0)
	{
		sm_sendprop_info_t info;
		gamehelpers->FindSendPropInfo("CEconEntity", "m_Item", &info);
		offset = info.actual_offset;
	}
	
    //CEconItemView ptr
	void* pEconItemView = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(pEntity) + offset);

	//CEconItemView::GetItemID()
	uint64_t itemID = ((uint64_t(__cdecl*)(void*))(*(void***)pEconItemView)[GetItemIDOffset])(pEconItemView);

    //Send message here
	CMsgIncrementKillCountAttribute msg;
	msg.set_killer_account_id(params[1]);
	msg.set_victim_account_id(params[2]);
	msg.set_item_id(itemID);
	msg.set_event_type(0);
	msg.set_amount(params[4]);

	return GetGCHelper().SendMessageToGC(k_EMsgGC_IncrementKillCountAttribute, msg);
}

const sp_nativeinfo_t g_ExtensionNatives[] =
{
	{ "IS_IncrementStattrak",		IS_IncrementStattrak },
	{ NULL,							NULL }
};

bool IncrementStattrak::SDK_OnLoad(char *error, size_t maxlen, bool late)
{
    SH_ADD_HOOK(IServerGameDLL, GameServerSteamAPIActivated, gamedll, SH_STATIC(&Hook_GameServerSteamAPIActivated), true);

    sharesys->AddNatives(myself, g_ExtensionNatives);
	sharesys->RegisterLibrary(myself, "IncrementStattrak");

    return true;
}

void IncrementStattrak::SDK_OnUnload()
{
    SH_REMOVE_HOOK(IServerGameDLL, GameServerSteamAPIActivated, gamedll, SH_STATIC(&Hook_GameServerSteamAPIActivated), true);
}

