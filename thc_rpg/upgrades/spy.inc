/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          spy.inc
 *  Type:          Upgrade
 *  Description:   Gives you the ability to move faster when shooting
 *  Author:        ArsiRC
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
new Module:g_modulespy;

new g_spyUpgradeConfigCache[UpgradeConfigs];

/**
 * Register this module.
 */
spy_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "spy");
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "spy");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "spy");

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
    g_modulespy = ModuleMgr_Register(moduledata);

    // Now register the events we're going to use.
    #if defined EVENT_MANAGER
        EventMgr_RegisterEvent(g_modulespy, "Event_OnEventsRegister", "spy_OnEventsRegister");
    #endif
    
    // Register config file(s) that this module will use.
    #if defined CONFIG_MANAGER
        ConfigMgr_Register(g_modulespy, "spy_OnConfigReload", "configs/thc_rpg/spy.txt");
    #endif
}

/**
 * Register all events here.
 */
public spy_OnEventsRegister()
{
    // Register all the events needed for this module.
    EventMgr_RegisterEvent(g_modulespy, "Event_OnMapStart",                   "spy_OnMapStart");
	#if defined PROJECT_GAME_CSS
		EventMgr_RegisterEvent(g_modulespy, "Event_PlayerHurt",              "spy_PlayerHurt");
	#endif
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public spy_OnEventsReady()
{
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public spy_OnConfigReload(configindex)
{
    #if defined LOG_MANAGER
        LogMgr_Print(g_modulespy, LogType_Normal, "spyConfigReload", "Reloaded speed module's config (index %d)", configindex);
    #endif

    ConfigMgr_CacheKv(g_modulespy, CM_CONFIGINDEX_FIRST, "spyModule_ConfigCache");
}

/**
 * Read config values
 */
public KvCache:spyModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
    // Read Upgrade config
    if(StrEqual(sectionname, "spy", false))
    {
        g_spyUpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
        g_spyUpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
        g_spyUpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
        g_spyUpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
        g_spyUpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");
        g_spyUpgradeConfigCache[UpgradeConfig_Percent] = KvGetFloat(kv, "percent");
        g_spyUpgradeConfigCache[UpgradeConfig_Mode] = KvGetNum(kv, "weapon");

        if(g_spyUpgradeConfigCache[UpgradeConfig_Disable]==1)
            ModuleMgr_Disable(g_modulespy);

        ModuleMgr_WriteCell(g_modulespy, ModuleData_TeamLock, g_spyUpgradeConfigCache[UpgradeConfig_TeamLock]);
        ModuleMgr_WriteCell(g_modulespy, ModuleData_MaxLevel, g_spyUpgradeConfigCache[UpgradeConfig_MaxLevel]);
        ModuleMgr_WriteCell(g_modulespy, ModuleData_Cost, g_spyUpgradeConfigCache[UpgradeConfig_Cost]);
        ModuleMgr_WriteCell(g_modulespy, ModuleData_iCost, g_spyUpgradeConfigCache[UpgradeConfig_iCost]);
    }
    else
        ModuleMgr_Disable(g_modulespy);
}
#endif

/**
 * The map has started.
 */
public spy_OnMapStart()
{
    #if defined CONFIG_MANAGER
        ConfigMgr_CacheKv(g_modulespy, CM_CONFIGINDEX_FIRST, "spyModule_ConfigCache");
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

public spy_PlayerHurt(victim, attacker, health, armor, const String:weapon[], dmg_health, dmg_armor, hitgroup)
{
	if(AreValidPlayers(victim, attacker))
	{
		new level = GetPlayerUpgradeLevel(attacker, g_modulespy);
		if(level > 0)
		{
			if(g_spyUpgradeConfigCache[UpgradeConfig_Mode]==0)
			{
				if(GetRandomFloat(0.0, 1.0) <= g_spyUpgradeConfigCache[UpgradeConfig_Percent]*level && GetClientHealth(victim) <= 0)
				{
					SetSpyModel(victim,attacker);
					PerformFade(attacker, 300, {0, 255, 0, 51});
				}
			}
			else if(g_spyUpgradeConfigCache[UpgradeConfig_Mode]==1)
			{
				if(StrEqual(weapon, "knife", false))
				{
					if(GetRandomFloat(0.0, 1.0) <= g_spyUpgradeConfigCache[UpgradeConfig_Percent]*level && GetClientHealth(victim) <= 0)
					{
						SetSpyModel(victim,attacker);
						PerformFade(attacker, 300, {0, 255, 0, 51});
					}
				}
			}
		}
	}
}

SetSpyModel(victim,attacker)
{
	decl String:buffer[PLATFORM_MAX_PATH];
	GetClientModel(victim, buffer, sizeof(buffer));
	SetEntityModel(attacker, buffer);
}

PerformFade(client, duration, const color[4]) { 
    if (client) 
	{ 
		new Handle:message = StartMessageOne("Fade", client); 
		PbSetInt(message, "duration", duration); 
		PbSetInt(message, "hold_time", 0); 
		PbSetInt(message, "flags", 0x0001); 
		PbSetColor(message, "clr", color); 
		EndMessage(); 
    } 
}