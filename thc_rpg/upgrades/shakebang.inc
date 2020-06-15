/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          shakebang.inc
 *  Type:          Upgrade
 *  Description:   Gives you the ability to shake the enemies screen with your flashbang
 *  Author:        TryNoob
 *
 *  Copyright (C) 2009-2011  ArsiRC
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * ============================================================================
 */

/**
 * This module's identifier.
 */
new Module:g_moduleShakebang;

/**
 * Function for outside files to use to return the module's identifier.
 */
stock Module:Shakebang_GetIdentifier() { return g_moduleShakebang; }

/**
 * Global variables
 */
new g_shakebangUpgradeConfigCache[UpgradeConfigs];

new Float:flFlashMaxAlpha[MAXPLAYERS+1] = {0.0, ...};

/**
 * Register this module.
 */
Shakebang_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];

    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "shakebang");
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "ShakeBang");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "Gives you the ability to shake the enemies screen with your flashbang");

    new Module:dependencies[MM_DATA_DEPENDENCIES];
    dependencies[0] = g_moduleCore;
    dependencies[1] = INVALID_MODULE;
    moduledata[ModuleData_Dependencies] = dependencies;

    new Module:interlocks[MM_DATA_INTERLOCKS];
    interlocks[0] = INVALID_MODULE;
    moduledata[ModuleData_Interlocks] = interlocks;

    moduledata[ModuleData_TeamLock] = 0;
    moduledata[ModuleData_MaxLevel] = 5;
    moduledata[ModuleData_Cost] = 10;
    moduledata[ModuleData_iCost] = 10;

    // Send this array of data to the module manager.
    g_moduleShakebang = ModuleMgr_Register(moduledata);

    // Now register the events we're going to use.
    #if defined EVENT_MANAGER
        EventMgr_RegisterEvent(g_moduleShakebang, "Event_OnEventsRegister", "Shakebang_OnEventsRegister");
    #endif

    // Register config file(s) that this module will use.
    #if defined CONFIG_MANAGER
        ConfigMgr_Register(g_moduleShakebang, "Shakebang_OnConfigReload", "configs/thc_rpg/shakebang.txt");
    #endif
}

/**
 * Register all events here.
 */
public Shakebang_OnEventsRegister()
{
	EventMgr_RegisterEvent(g_moduleShakebang, "Event_OnMapStart",              "Shakebang_OnMapStart");
	#if defined PROJECT_GAME_CSS
		EventMgr_RegisterEvent(g_moduleShakebang, "Event_FlashbangDetonated",  "Shakebang_FlashbangDetonated");
		EventMgr_RegisterEvent(g_moduleShakebang, "Event_PlayerBlind", "Shakebang_PlayerBlind");
	#endif
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public Shakebang_OnEventsReady()
{
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public Shakebang_OnConfigReload(configindex)
{
    #if defined LOG_MANAGER
        LogMgr_Print(g_moduleShakebang, LogType_Normal, "shakebangConfigReload", "Reloaded shakebang module's config (index %d)", configindex);
    #endif

    ConfigMgr_CacheKv(g_moduleShakebang, CM_CONFIGINDEX_FIRST, "shakebangModule_ConfigCache");
}

/**
  * Read config values
  */
public KvCache:shakebangModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
    // Read Upgrade config
    if(StrEqual(sectionname, "shakebang", false))
    {
        g_shakebangUpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
        g_shakebangUpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
        g_shakebangUpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
        g_shakebangUpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
        g_shakebangUpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");

        if(g_shakebangUpgradeConfigCache[UpgradeConfig_Disable]==1)
          ModuleMgr_Disable(g_moduleShakebang);

        ModuleMgr_WriteCell(g_moduleShakebang, ModuleData_MaxLevel, g_shakebangUpgradeConfigCache[UpgradeConfig_MaxLevel]);
        ModuleMgr_WriteCell(g_moduleShakebang, ModuleData_Cost, g_shakebangUpgradeConfigCache[UpgradeConfig_Cost]);
        ModuleMgr_WriteCell(g_moduleShakebang, ModuleData_iCost, g_shakebangUpgradeConfigCache[UpgradeConfig_iCost]);
    }
}
#endif



/**
 * The map has started.
 */
public Shakebang_OnMapStart()
{
    #if defined CONFIG_MANAGER
        ConfigMgr_CacheKv(g_moduleShakebang, CM_CONFIGINDEX_FIRST, "shakebangModule_ConfigCache");
    #endif
}

public Shakebang_PlayerBlind(client)
{
	if(IsValidPlayer(client))
	{
		flFlashMaxAlpha[client] = GetEntPropFloat(client, Prop_Send, "m_flFlashMaxAlpha");
		SetEntDataFloat(client, g_hFlashAlpha, 0.1);
		SetEntDataFloat(client, g_hFlashDuration, 0.1);
	}
}

/**
 * flashbang has detonated.
 *
 * @param event         Handle to event. This could be INVALID_HANDLE if every plugin hooking
 *                      this event has set the hook mode EventHookMode_PostNoCopy.
 * @param name          String containing the name of the event.
 * @param dontBroadcast True if event was not broadcast to clients, false otherwise.
 */
public Shakebang_FlashbangDetonated(thrower, Float:x, Float:y, Float:z)
{
	if(IsValidPlayer(thrower))
	{
		new level = GetPlayerUpgradeLevel(thrower, g_moduleShakebang);
		if(level > 0)
		{
			for(new victim = 1; victim <= MaxClients; victim++)
			{
				Shake(thrower, victim, flFlashMaxAlpha[victim], level*1.5);
			}
		}
	}
}

Shake(thrower, victim, Float:flAmplitude, Float:flDuration)
{
	if(AreValidPlayers(thrower, victim))
	{
		if(thrower != victim && GetClientTeam(thrower) != GetClientTeam(victim))
		{
			new Handle:hBf = StartMessageOne("Shake", victim);
			if(hBf != INVALID_HANDLE)
			{
				PbSetInt(hBf, "command", 0);
				PbSetFloat(hBf, "local_amplitude", flAmplitude);
				PbSetFloat(hBf, "frequency", 1.0);
				PbSetFloat(hBf, "duration", flDuration);
				EndMessage();
			}
			flFlashMaxAlpha[victim] = 0.0;
		}
	}
}