/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          fastreload.inc
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
new Module:g_modulefastreload;

new g_fastreloadUpgradeConfigCache[UpgradeConfigs];

/**
 * Register this module.
 */
fastreload_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "fastreload");
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "fastreload");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "fastreload");

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
    g_modulefastreload = ModuleMgr_Register(moduledata);

    // Now register the events we're going to use.
    #if defined EVENT_MANAGER
        EventMgr_RegisterEvent(g_modulefastreload, "Event_OnEventsRegister", "fastreload_OnEventsRegister");
    #endif
    
    // Register config file(s) that this module will use.
    #if defined CONFIG_MANAGER
        ConfigMgr_Register(g_modulefastreload, "fastreload_OnConfigReload", "configs/thc_rpg/fastreload.txt");
    #endif
}

/**
 * Register all events here.
 */
public fastreload_OnEventsRegister()
{
    // Register all the events needed for this module.
    EventMgr_RegisterEvent(g_modulefastreload, "Event_OnMapStart",                   "fastreload_OnMapStart");
	#if defined PROJECT_GAME_CSS
		EventMgr_RegisterEvent(g_modulefastreload, "Event_WeaponReload",           "fastreload_WeaponReload");
	#endif
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public fastreload_OnEventsReady()
{
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public fastreload_OnConfigReload(configindex)
{
    #if defined LOG_MANAGER
        LogMgr_Print(g_modulefastreload, LogType_Normal, "fastreloadConfigReload", "Reloaded speed module's config (index %d)", configindex);
    #endif

    ConfigMgr_CacheKv(g_modulefastreload, CM_CONFIGINDEX_FIRST, "fastreloadModule_ConfigCache");
}

/**
 * Read config values
 */
public KvCache:fastreloadModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
    // Read Upgrade config
    if(StrEqual(sectionname, "fastreload", false))
    {
        g_fastreloadUpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
        g_fastreloadUpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
        g_fastreloadUpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
        g_fastreloadUpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
        g_fastreloadUpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");
        g_fastreloadUpgradeConfigCache[UpgradeConfig_Percent] = KvGetFloat(kv, "Percent");

        if(g_fastreloadUpgradeConfigCache[UpgradeConfig_Disable]==1)
            ModuleMgr_Disable(g_modulefastreload);

        ModuleMgr_WriteCell(g_modulefastreload, ModuleData_TeamLock, g_fastreloadUpgradeConfigCache[UpgradeConfig_TeamLock]);
        ModuleMgr_WriteCell(g_modulefastreload, ModuleData_MaxLevel, g_fastreloadUpgradeConfigCache[UpgradeConfig_MaxLevel]);
        ModuleMgr_WriteCell(g_modulefastreload, ModuleData_Cost, g_fastreloadUpgradeConfigCache[UpgradeConfig_Cost]);
        ModuleMgr_WriteCell(g_modulefastreload, ModuleData_iCost, g_fastreloadUpgradeConfigCache[UpgradeConfig_iCost]);
    }
    else
        ModuleMgr_Disable(g_modulefastreload);
}
#endif

/**
 * The map has started.
 */
public fastreload_OnMapStart()
{
    #if defined CONFIG_MANAGER
        ConfigMgr_CacheKv(g_modulefastreload, CM_CONFIGINDEX_FIRST, "fastreloadModule_ConfigCache");
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
public fastreload_WeaponReload(client, const String:weapon[])
{
	if(IsValidPlayer(client))
	{
		new level = GetPlayerUpgradeLevel(client, g_modulefastreload);
		if(level > 0 && GetPlayerWeaponSlot(client, 2) > 0)
		{
			new Float:chance = g_fastreloadUpgradeConfigCache[UpgradeConfig_Percent] * level;
			if (chance >= GetRandomFloat(0.0, 1.0))
			{
				if (!StrEqual(weapon, "m3") && !StrEqual(weapon, "weapon_xm1014"))
				{
					SetEntPropFloat(client, Prop_Send, "m_flNextAttack", GetGameTime());
					ClientCommand(client, "lastinv");
					CreateTimer(0.01, LastInv_Timer, client, TIMER_FLAG_NO_MAPCHANGE);
				}
			}
		}
	}
}

public Action:LastInv_Timer(Handle:timer, any:client)
{
	if (IsClientInGame(client)) ClientCommand(client, "lastinv");
	return Plugin_Stop;
}