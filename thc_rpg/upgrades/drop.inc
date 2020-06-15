/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          drop.inc
 *  Type:          Upgrade
 *  Description:   Gives you the ability to move faster when shooting
 *  Author:        SenatoR
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
new Module:g_moduledrop;

new g_dropUpgradeConfigCache[UpgradeConfigs];

/**
 * Register this module.
 */
drop_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "drop");
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "drop");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "drop");

    new Module:dependencies[MM_DATA_DEPENDENCIES];
    dependencies[0] = g_moduleCore;
    dependencies[1] = INVALID_MODULE;
    moduledata[ModuleData_Dependencies] = dependencies;
    
    new Module:interlocks[MM_DATA_INTERLOCKS];
    interlocks[0] = INVALID_MODULE;
    moduledata[ModuleData_Interlocks] = interlocks;
    
    moduledata[ModuleData_TeamLock] = 0;
    moduledata[ModuleData_MaxLevel] = 6;
    moduledata[ModuleData_Cost] = 10;
    moduledata[ModuleData_iCost] = 10;

    // Send this array of data to the module manager.
    g_moduledrop = ModuleMgr_Register(moduledata);

    // Now register the events we're going to use.
    #if defined EVENT_MANAGER
        EventMgr_RegisterEvent(g_moduledrop, "Event_OnEventsRegister", "drop_OnEventsRegister");
    #endif
    
    // Register config file(s) that this module will use.
    #if defined CONFIG_MANAGER
        ConfigMgr_Register(g_moduledrop, "drop_OnConfigReload", "configs/thc_rpg/drop.txt");
    #endif
}

/**
 * Register all events here.
 */
public drop_OnEventsRegister()
{
    // Register all the events needed for this module.
    EventMgr_RegisterEvent(g_moduledrop, "Event_OnMapStart",                   "drop_OnMapStart");
	#if defined PROJECT_GAME_CSS
		EventMgr_RegisterEvent(g_moduledrop, "Event_PlayerHurt",              "drop_PlayerHurt");
	#endif
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public drop_OnEventsReady()
{
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public drop_OnConfigReload(configindex)
{
    #if defined LOG_MANAGER
        LogMgr_Print(g_moduledrop, LogType_Normal, "dropConfigReload", "Reloaded speed module's config (index %d)", configindex);
    #endif

    ConfigMgr_CacheKv(g_moduledrop, CM_CONFIGINDEX_FIRST, "dropModule_ConfigCache");
}

/**
 * Read config values
 */
public KvCache:dropModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
    // Read Upgrade config
    if(StrEqual(sectionname, "drop", false))
    {
        g_dropUpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
        g_dropUpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
        g_dropUpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
        g_dropUpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
        g_dropUpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");
        g_dropUpgradeConfigCache[UpgradeConfig_Percent] = KvGetFloat(kv, "percent");

        if(g_dropUpgradeConfigCache[UpgradeConfig_Disable]==1)
            ModuleMgr_Disable(g_moduledrop);

        ModuleMgr_WriteCell(g_moduledrop, ModuleData_TeamLock, g_dropUpgradeConfigCache[UpgradeConfig_TeamLock]);
        ModuleMgr_WriteCell(g_moduledrop, ModuleData_MaxLevel, g_dropUpgradeConfigCache[UpgradeConfig_MaxLevel]);
        ModuleMgr_WriteCell(g_moduledrop, ModuleData_Cost, g_dropUpgradeConfigCache[UpgradeConfig_Cost]);
        ModuleMgr_WriteCell(g_moduledrop, ModuleData_iCost, g_dropUpgradeConfigCache[UpgradeConfig_iCost]);
    }
    else
        ModuleMgr_Disable(g_moduledrop);
}
#endif

/**
 * The map has started.
 */
public drop_OnMapStart()
{
    #if defined CONFIG_MANAGER
        ConfigMgr_CacheKv(g_moduledrop, CM_CONFIGINDEX_FIRST, "dropModule_ConfigCache");
    #endif
}

/**
 * Client has been damaged.
 * 
 * @param victim        The index of the hurt client.
 * @param attacker      The index of the attacking client.
 * @param health        How much health the client has after the damage.
 * @param armor         How much armor the client has after the damage.
 * @param weapon        The weapon classname used to hurt the victim. (No weapon_ prefix)
 * @param dmg_health    The amount of health the victim lost.
 * @param dmg_armor     The amount of armor the victim lost.
 * @param hitgroup      The hitgroup index of the victim that was damaged.
 */
public drop_PlayerHurt(victim, attacker, health, armor, const String:weapon[], dmg_health, dmg_armor, hitgroup)
{
	if(AreValidPlayers(victim, attacker))
	{
		new level = GetPlayerUpgradeLevel(attacker, g_moduledrop);
		if(level > 0)
		{
			if(StrEqual(weapon, "scout")||StrEqual(weapon, "ssg08"))
			{
				new Float:chance = g_dropUpgradeConfigCache[UpgradeConfig_Percent] * level;
				if (chance >= GetRandomFloat(0.0, 1.0))
				{
					ClientCommand(victim, "drop");
					TransMgr_PrintText(attacker, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "DROP", victim);
					TransMgr_PrintText(victim, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "NOOB", attacker);
				}
			}
		}
	}
}