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
new Module:g_moduleTeleportDecoy;

/**
 * Function for outside files to use to return the module's identifier.
 */
stock Module:TeleportDecoy_GetIdentifier() { return g_moduleTeleportDecoy; }

/**
 * Global variables
 */
new g_teleportdecoyUpgradeConfigCache[UpgradeConfigs];

/**
 * Register this module.
 */
TeleportDecoy_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];

    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "teleportdecoy");
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "TeleportDecoy");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "Gives you the ability to teleport with decoy");

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
    g_moduleTeleportDecoy = ModuleMgr_Register(moduledata);

    // Now register the events we're going to use.
    #if defined EVENT_MANAGER
        EventMgr_RegisterEvent(g_moduleTeleportDecoy, "Event_OnEventsRegister", "TeleportDecoy_OnEventsRegister");
    #endif

    // Register config file(s) that this module will use.
    #if defined CONFIG_MANAGER
        ConfigMgr_Register(g_moduleTeleportDecoy, "TeleportDecoy_OnConfigReload", "configs/thc_rpg/teleportdecoy.txt");
    #endif
}

/**
 * Register all events here.
 */
public TeleportDecoy_OnEventsRegister()
{
	EventMgr_RegisterEvent(g_moduleTeleportDecoy, "Event_OnMapStart",              "TeleportDecoy_OnMapStart");
	#if defined PROJECT_GAME_CSS
		EventMgr_RegisterEvent(g_moduleTeleportDecoy, "Event_DecoyFiring",  "TeleportDecoy_DecoyFiring");
	#endif
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public TeleportDecoy_OnEventsReady()
{
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public TeleportDecoy_OnConfigReload(configindex)
{
    #if defined LOG_MANAGER
        LogMgr_Print(g_moduleTeleportDecoy, LogType_Normal, "teleportdecoyConfigReload", "Reloaded teleportdecoy module's config (index %d)", configindex);
    #endif

    ConfigMgr_CacheKv(g_moduleTeleportDecoy, CM_CONFIGINDEX_FIRST, "teleportdecoyModule_ConfigCache");
}

/**
  * Read config values
  */
public KvCache:teleportdecoyModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
    // Read Upgrade config
    if(StrEqual(sectionname, "teleportdecoy", false))
    {
        g_teleportdecoyUpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
        g_teleportdecoyUpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
        g_teleportdecoyUpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
        g_teleportdecoyUpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
        g_teleportdecoyUpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");
		g_teleportdecoyUpgradeConfigCache[UpgradeConfig_Percent] = KvGetFloat(kv, "percent");

        if(g_teleportdecoyUpgradeConfigCache[UpgradeConfig_Disable]==1)
          ModuleMgr_Disable(g_moduleTeleportDecoy);

        ModuleMgr_WriteCell(g_moduleTeleportDecoy, ModuleData_MaxLevel, g_teleportdecoyUpgradeConfigCache[UpgradeConfig_MaxLevel]);
        ModuleMgr_WriteCell(g_moduleTeleportDecoy, ModuleData_Cost, g_teleportdecoyUpgradeConfigCache[UpgradeConfig_Cost]);
        ModuleMgr_WriteCell(g_moduleTeleportDecoy, ModuleData_iCost, g_teleportdecoyUpgradeConfigCache[UpgradeConfig_iCost]);
    }
}
#endif



/**
 * The map has started.
 */
public TeleportDecoy_OnMapStart()
{
    #if defined CONFIG_MANAGER
        ConfigMgr_CacheKv(g_moduleTeleportDecoy, CM_CONFIGINDEX_FIRST, "teleportdecoyModule_ConfigCache");
    #endif
}

/**
 * flashbang has detonated.
 *
 * @param event         Handle to event. This could be INVALID_HANDLE if every plugin hooking
 *                      this event has set the hook mode EventHookMode_PostNoCopy.
 * @param name          String containing the name of the event.
 * @param dontBroadcast True if event was not broadcast to clients, false otherwise.
 */
public TeleportDecoy_DecoyFiring(iClient, entityid, Float:x, Float:y, Float:z)
{
	if(IsValidPlayer(iClient))
	{
		new level = GetPlayerUpgradeLevel(iClient, g_moduleTeleportDecoy);
		if(level > 0)
		{	
			if((GetRandomFloat(0.0, 1.0) <= (g_teleportdecoyUpgradeConfigCache[UpgradeConfig_Percent]*level)) && IsPlayerAlive(iClient))
			{
				new Float:f_Pos[3];
				f_Pos[0] = x;
				f_Pos[1] = y;
				f_Pos[2] = z;
				TeleportEntity(iClient, f_Pos, NULL_VECTOR, NULL_VECTOR);
				RemoveEdict(entityid);
			}
			else
			{
				RemoveEdict(entityid);
			}
		}
	}
}