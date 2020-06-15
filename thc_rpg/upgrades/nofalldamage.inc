/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          nofalldamage.inc
 *  Type:          Upgrade
 *  Description:   Gives you ability to avoid fall damage
 *	Author:		   Unknown
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
new Module:g_modulenofalldamage;

new g_nofalldamageUpgradeConfigCache[UpgradeConfigs];
/**
 * Register this module.
 */
nofalldamage_Register()
{
	new moduledata[ModuleData];
	moduledata[ModuleData_Disabled] = false;
	moduledata[ModuleData_Hidden] = false;
	strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "nofalldamage");
	strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "nofalldamage");
	strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "nofalldamage");
	
	new Module:dependencies[MM_DATA_DEPENDENCIES];
	dependencies[0] = g_moduleCore;
	dependencies[1] = INVALID_MODULE;
	moduledata[ModuleData_Dependencies] = dependencies;
	
	new Module:interlocks[MM_DATA_INTERLOCKS];
	interlocks[0] = INVALID_MODULE;
	moduledata[ModuleData_Interlocks] = interlocks;
	
	moduledata[ModuleData_TeamLock] = 0;
	moduledata[ModuleData_MaxLevel] = 20;
	moduledata[ModuleData_Cost] = 10000;
	moduledata[ModuleData_iCost] = 8000;
	
	// Send this array of data to the module manager
	g_modulenofalldamage = ModuleMgr_Register(moduledata);
	
	// Now register the events we're going to use
	#if defined EVENT_MANAGER
		EventMgr_RegisterEvent(g_modulenofalldamage, "Event_OnEventsRegister", "nofalldamage_OnEventsRegister");
	#endif
	
	// Register config file(s) that this module will use
	#if defined CONFIG_MANAGER
		ConfigMgr_Register(g_modulenofalldamage, "nofalldamage_OnConfigReload", "configs/thc_rpg/nofalldamage.txt");
	#endif
}

public nofalldamage_OnEventsRegister()
{
	EventMgr_RegisterEvent(g_modulenofalldamage, "Event_OnMapStart",				   "nofalldamage_OnMapStart");
	#if defined PROJECT_GAME_CSS
		EventMgr_RegisterEvent(g_modulenofalldamage, "Event_PlayerHurt", "nofalldamage_PlayerHurt");
	#endif
}

public nofalldamage_OnMapStart()
{
    #if defined CONFIG_MANAGER
        ConfigMgr_CacheKv(g_modulenofalldamage, CM_CONFIGINDEX_FIRST, "nofalldamageModule_ConfigCache");
    #endif
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public nofalldamage_OnConfigReload(configindex)
{
	ConfigMgr_CacheKv(g_modulenofalldamage, CM_CONFIGINDEX_FIRST, "nofalldamageModule_ConfigCache");
}

public KvCache:nofalldamageModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
	if(StrEqual(sectionname, "nofalldamage", false))
	{
		g_nofalldamageUpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
		g_nofalldamageUpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
		g_nofalldamageUpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
		g_nofalldamageUpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
		g_nofalldamageUpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");
		g_nofalldamageUpgradeConfigCache[UpgradeConfig_Percent] = KvGetFloat(kv, "percent");
		
		if(g_nofalldamageUpgradeConfigCache[UpgradeConfig_Disable]==1)
			ModuleMgr_Disable(g_modulenofalldamage);
		
		ModuleMgr_WriteCell(g_modulenofalldamage, ModuleData_TeamLock, g_nofalldamageUpgradeConfigCache[UpgradeConfig_TeamLock]);
		ModuleMgr_WriteCell(g_modulenofalldamage, ModuleData_MaxLevel, g_nofalldamageUpgradeConfigCache[UpgradeConfig_MaxLevel]);
		ModuleMgr_WriteCell(g_modulenofalldamage, ModuleData_Cost, g_nofalldamageUpgradeConfigCache[UpgradeConfig_Cost]);
		ModuleMgr_WriteCell(g_modulenofalldamage, ModuleData_iCost, g_nofalldamageUpgradeConfigCache[UpgradeConfig_iCost]);
	}
	else
		ModuleMgr_Disable(g_modulenofalldamage);
}
#endif

/**
 * Client has been damaged.
 *
 * @param victim        The index of the hurt client.
 * @param attacker      The index of the attacking client.
 * @param health        How much health the client has after the damage.
 * @param armor         How much armor the client has after the mirrordmg.
 * @param weapon        The weapon classname used to hurt the victim. (No weapon_ prefix)
 * @param dmg_health    The amount of health the victim lost.
 * @param dmg_armor     The amount of armor the victim lost.
 * @param hitgroup      The hitgroup index of the victim that was damaged.
 */
public nofalldamage_PlayerHurt(victim, attacker, health, armor, const String:weapon[], dmg_health, dmg_armor, hitgroup)
{
 	if(IsValidPlayer(victim) && attacker == 0)
	{
		if(GetClientTeam(victim) != g_nofalldamageUpgradeConfigCache[UpgradeConfig_TeamLock])
		{
			new level = GetPlayerUpgradeLevel(victim, g_modulenofalldamage);
			if(level > 0)
			{
				SetEntityHealth(victim, health + RoundFloat((float(dmg_health)*g_nofalldamageUpgradeConfigCache[UpgradeConfig_Percent])*level));
			}
		}
	}
}