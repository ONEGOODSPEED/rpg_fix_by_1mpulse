/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:		  scout.inc
 *  Type:		  Upgrade
 *  Description:   Gives you the ability to move faster when shooting
 *  Author:		ArsiRC
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
new Module:g_modulescout;

new g_scoutUpgradeConfigCache[UpgradeConfigs];

new bool:bRoundEnd;

/**
 * Register this module.
 */
scout_Register()
{
	// Define all the module's data as layed out by enum ModuleData in project.inc.
	new moduledata[ModuleData];
	
	moduledata[ModuleData_Disabled] = false;
	moduledata[ModuleData_Hidden] = false;
	strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "scout");
	strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "scout");
	strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "scout");

	new Module:dependencies[MM_DATA_DEPENDENCIES];
	dependencies[0] = g_moduleCore;
	dependencies[1] = INVALID_MODULE;
	moduledata[ModuleData_Dependencies] = dependencies;
	
	new Module:interlocks[MM_DATA_INTERLOCKS];
	interlocks[0] = INVALID_MODULE;
	moduledata[ModuleData_Interlocks] = interlocks;
	
	moduledata[ModuleData_TeamLock] = 0;
	moduledata[ModuleData_MaxLevel] = 10;
	moduledata[ModuleData_Cost] = 10;
	moduledata[ModuleData_iCost] = 5;

	// Send this array of data to the module manager.
	g_modulescout = ModuleMgr_Register(moduledata);

	// Now register the events we're going to use.
	#if defined EVENT_MANAGER
		EventMgr_RegisterEvent(g_modulescout, "Event_OnEventsRegister", "scout_OnEventsRegister");
	#endif
	
	// Register config file(s) that this module will use.
	#if defined CONFIG_MANAGER
		ConfigMgr_Register(g_modulescout, "scout_OnConfigReload", "configs/thc_rpg/scout.txt");
	#endif
}

/**
 * Register all events here.
 */
public scout_OnEventsRegister()
{
	// Register all the events needed for this module.
	EventMgr_RegisterEvent(g_modulescout, "Event_OnMapStart",				   "scout_OnMapStart");
	#if defined PROJECT_GAME_CSS
		EventMgr_RegisterEvent(g_modulescout, "Event_PlayerHurt",			  "scout_PlayerHurt");
		EventMgr_RegisterEvent(g_modulescout, "Event_RoundStart",              "scout_RoundStart");
		EventMgr_RegisterEvent(g_modulescout, "Event_RoundEnd",                "scout_RoundEnd");
	#endif
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public scout_OnEventsReady()
{
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public scout_OnConfigReload(configindex)
{
	#if defined LOG_MANAGER
		LogMgr_Print(g_modulescout, LogType_Normal, "scoutConfigReload", "Reloaded speed module's config (index %d)", configindex);
	#endif

	ConfigMgr_CacheKv(g_modulescout, CM_CONFIGINDEX_FIRST, "scoutModule_ConfigCache");
}

/**
 * Read config values
 */
public KvCache:scoutModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
	// Read Upgrade config
	if(StrEqual(sectionname, "scout", false))
	{
		g_scoutUpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
		g_scoutUpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
		g_scoutUpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
		g_scoutUpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
		g_scoutUpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");
		g_scoutUpgradeConfigCache[UpgradeConfig_Amount] = KvGetFloat(kv, "amount");
		g_scoutUpgradeConfigCache[UpgradeConfig_Mode] = KvGetNum(kv, "mode");
		g_scoutUpgradeConfigCache[UpgradeConfig_Effects] = KvGetNum(kv, "effects");
		
		if(g_scoutUpgradeConfigCache[UpgradeConfig_Disable]==1)
			ModuleMgr_Disable(g_modulescout);

		ModuleMgr_WriteCell(g_modulescout, ModuleData_TeamLock, g_scoutUpgradeConfigCache[UpgradeConfig_TeamLock]);
		ModuleMgr_WriteCell(g_modulescout, ModuleData_MaxLevel, g_scoutUpgradeConfigCache[UpgradeConfig_MaxLevel]);
		ModuleMgr_WriteCell(g_modulescout, ModuleData_Cost, g_scoutUpgradeConfigCache[UpgradeConfig_Cost]);
		ModuleMgr_WriteCell(g_modulescout, ModuleData_iCost, g_scoutUpgradeConfigCache[UpgradeConfig_iCost]);
	}
	else
		ModuleMgr_Disable(g_modulescout);
}
#endif

/**
 * The map has started.
 */
public scout_OnMapStart()
{
	#if defined CONFIG_MANAGER
		ConfigMgr_CacheKv(g_modulescout, CM_CONFIGINDEX_FIRST, "scoutModule_ConfigCache");
	#endif
}

/**
 * Round has started.
 */
public scout_RoundStart()
{
	bRoundEnd = false;
}

/**
 * Round has ended.
 *
 * @param winner    The index of the winning team.
 */
public scout_RoundEnd(client, winner)
{
	bRoundEnd = true;
}

/**
 * Client has been damaged.
 * 
 * @param victim		The index of the hurt client.
 * @param attacker	  The index of the attacking client.
 * @param health		How much health the client has after the damage.
 * @param armor		 How much armor the client has after the damage.
 * @param weapon		The weapon classname used to hurt the victim. (No weapon_ prefix)
 * @param dmg_health	The amount of health the victim lost.
 * @param dmg_armor	 The amount of armor the victim lost.
 * @param hitgroup	  The hitgroup index of the victim that was damaged.
 */
public scout_PlayerHurt(victim, attacker, health, armor, const String:weapon[], dmg_health, dmg_armor, hitgroup)
{
	if(AreValidPlayers(victim, attacker))
	{
		new level = GetPlayerUpgradeLevel(attacker, g_modulescout);
		if(level > 0)
		{		
			if(StrEqual(weapon, "scout", false)||StrEqual(weapon, "ssg08", false))
			{
				new count = 0;
				new Handle:datapack = CreateDataPack();
				WritePackCell(datapack, victim);
				WritePackCell(datapack, attacker);
				WritePackCell(datapack, count);
				if(g_scoutUpgradeConfigCache[UpgradeConfig_Effects]==1 || g_scoutUpgradeConfigCache[UpgradeConfig_Effects]==3)
				{
					SetColor(victim,0,255,0, -1);
				}
				CreateTimer(1.0, Timer_ScoutAttack, datapack, TIMER_FLAG_NO_MAPCHANGE);
			}
		}
	}
}

public Action:Timer_ScoutAttack(Handle:timer, Handle:datapack)
{
	if(!bRoundEnd)
	{
		ResetPack(datapack);
		new victim = ReadPackCell(datapack);
		new attacker = ReadPackCell(datapack);
		new level = GetPlayerUpgradeLevel(attacker, g_modulescout);
		if(IsValidPlayer(victim))
		{
			if(IsValidPlayer(attacker) && victim != attacker && IsPlayerAlive(victim))
			{
				new damage = RoundToNearest(level*g_scoutUpgradeConfigCache[UpgradeConfig_Amount]);
				RPGDamage(victim, damage, attacker, 0, "");
				new count = ReadPackCell(datapack);
				count++;
				if(count < g_scoutUpgradeConfigCache[UpgradeConfig_Mode])
				{
					datapack = CreateDataPack();
					WritePackCell(datapack, victim);
					WritePackCell(datapack, attacker);
					WritePackCell(datapack, count);
					if(g_scoutUpgradeConfigCache[UpgradeConfig_Effects]==2 || g_scoutUpgradeConfigCache[UpgradeConfig_Effects]==3)
					{
						FlashScreenAdvanced(victim,RGBA_COLOR_GREEN);
					}
					CreateTimer(1.0, Timer_ScoutAttack, datapack, TIMER_FLAG_NO_MAPCHANGE);
					return Plugin_Stop;
				}
			}
			if(g_scoutUpgradeConfigCache[UpgradeConfig_Effects]==1 || g_scoutUpgradeConfigCache[UpgradeConfig_Effects]==3)
			{
				SetColor(victim,255,255,255, -1);
			}
		}
	}
	return Plugin_Stop;
}