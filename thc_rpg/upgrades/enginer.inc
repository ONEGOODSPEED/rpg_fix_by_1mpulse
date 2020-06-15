/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:		  enginer.inc
 *  Type:		  Upgrade
 *  Description:   Gives you the ability to slowly raise the HP of nearby teammates
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
new Module:g_moduleenginer;

new g_enginerUpgradeConfigCache[UpgradeConfigs];

new Handle:g_hEnginerPlayerIsArmoring[MAXPLAYERS+1] = {INVALID_HANDLE,...};

new GettingXPArmoring[MAXPLAYERS+1];

/**
 * Register this module.
 */
enginer_Register()
{
	// Define all the module's data as layed out by enum ModuleData in project.inc.
	new moduledata[ModuleData];
	
	moduledata[ModuleData_Disabled] = false;
	moduledata[ModuleData_Hidden] = false;
	strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "enginer");
	strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "Enginer");
	strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "Gives you the ability to slowly raise the armor of nearby teammates");

	new Module:dependencies[MM_DATA_DEPENDENCIES];
	dependencies[0] = g_moduleCore;
	dependencies[1] = INVALID_MODULE;
	moduledata[ModuleData_Dependencies] = dependencies;

	new Module:interlocks[MM_DATA_INTERLOCKS];
	interlocks[0] = INVALID_MODULE;
	moduledata[ModuleData_Interlocks] = interlocks;
	
	moduledata[ModuleData_TeamLock] = 0;
	moduledata[ModuleData_MaxLevel] = 5;
	moduledata[ModuleData_Cost] = 15;
	moduledata[ModuleData_iCost] = 20;

	// Send this array of data to the module manager.
	g_moduleenginer = ModuleMgr_Register(moduledata);

	// Now register the events we're going to use.
	#if defined EVENT_MANAGER
		EventMgr_RegisterEvent(g_moduleenginer, "Event_OnEventsRegister", "enginer_OnEventsRegister");
	#endif

	// Register config file(s) that this module will use.
	#if defined CONFIG_MANAGER
		ConfigMgr_Register(g_moduleenginer, "enginer_OnConfigReload", "configs/thc_rpg/enginer.txt");
	#endif
}

/**
 * Register all events here.
 */
public enginer_OnEventsRegister()
{
	// Register all the events needed for this module.
	EventMgr_RegisterEvent(g_moduleenginer, "Event_OnMyModuleEnable",			 "enginer_OnMyModuleEnable");
	EventMgr_RegisterEvent(g_moduleenginer, "Event_OnMyModuleDisable",			"enginer_OnMyModuleDisable");
	EventMgr_RegisterEvent(g_moduleenginer, "Event_OnMapStart",				   "enginer_OnMapStart");
	EventMgr_RegisterEvent(g_moduleenginer, "Event_OnClientDisconnect",		   "enginer_OnClientDisconnect");
	#if defined PROJECT_GAME_CSS
		EventMgr_RegisterEvent(g_moduleenginer, "Event_PlayerSpawn",			  "enginer_PlayerSpawn");
	#endif
	
	// Custom events
	EventMgr_RegisterEvent(g_moduleenginer, "Event_OnUpgradePurchase",			"enginer_OnUpgradePurchase");
	EventMgr_RegisterEvent(g_moduleenginer, "Event_OnUpgradeSell",				"enginer_OnUpgradeSell");
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public enginer_OnEventsReady()
{
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public enginer_OnConfigReload(configindex)
{
	#if defined LOG_MANAGER
		LogMgr_Print(g_moduleenginer, LogType_Normal, "enginerConfigReload", "Reloaded enginer module's config (index %d)", configindex);
	#endif

	ConfigMgr_CacheKv(g_moduleenginer, CM_CONFIGINDEX_FIRST, "enginerModule_ConfigCache");
}

/**
 * Read config values
 */
public KvCache:enginerModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
	// Read Upgrade config
	if(StrEqual(sectionname, "enginer", false))
	{
		g_enginerUpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
		g_enginerUpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
		g_enginerUpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
		g_enginerUpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
		g_enginerUpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");
		g_enginerUpgradeConfigCache[UpgradeConfig_Interval] = KvGetFloat(kv, "interval");
		g_enginerUpgradeConfigCache[UpgradeConfig_Amount] = KvGetFloat(kv, "amount");

		if(g_enginerUpgradeConfigCache[UpgradeConfig_Disable]==1)
			ModuleMgr_Disable(g_moduleenginer);

		ModuleMgr_WriteCell(g_moduleenginer, ModuleData_TeamLock, g_enginerUpgradeConfigCache[UpgradeConfig_TeamLock]);
		ModuleMgr_WriteCell(g_moduleenginer, ModuleData_MaxLevel, g_enginerUpgradeConfigCache[UpgradeConfig_MaxLevel]);
		ModuleMgr_WriteCell(g_moduleenginer, ModuleData_Cost, g_enginerUpgradeConfigCache[UpgradeConfig_Cost]);
		ModuleMgr_WriteCell(g_moduleenginer, ModuleData_iCost, g_enginerUpgradeConfigCache[UpgradeConfig_iCost]);
	}
	else
		ModuleMgr_Disable(g_moduleenginer);
}
#endif

/**
 * The module that hooked this event callback has been enabled.
 *
 * @param refusalmsg	The string that is printed if Plugin_Handled is returned and it is non-empty.
 * @param maxlen		The max length of the string.
 *
 * @return	  Return Plugin_Handled to stop enable, and Plugin_Continue to allow it.
 */
public Action:enginer_OnMyModuleEnable(String:refusalmsg[], maxlen)
{
	new size=GetArraySize(g_hVecPlayers);
	for(new x=0;x<size;x++)
	{
		new Handle:vecPlayer=GetArrayCell(g_hVecPlayers,x);
		new client=GetArrayCell(vecPlayer,VECTOR_PLAYERS_CLIENT);
		if(IsValidPlayer(client))
			if(GetClientTeam(client)!=g_enginerUpgradeConfigCache[UpgradeConfig_TeamLock])
			{
				enginer_StopArmoring(client);
				if(GetPlayerUpgradeLevel(client,g_moduleenginer)>0 && g_hEnginerPlayerIsArmoring[client]==INVALID_HANDLE)
					g_hEnginerPlayerIsArmoring[client]=CreateTimer(g_enginerUpgradeConfigCache[UpgradeConfig_Interval],Timer_OnEnginerInterval,client,TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
			}
	}
	
	return Plugin_Continue;
}

/**
 * The module that hooked this event callback has been disabled.
 *
 * @param refusalmsg	The string that is printed if Plugin_Handled is returned and it is non-empty.
 * @param maxlen		The max length of the string.
 *
 * @return	  Return Plugin_Handled to stop disable, and Plugin_Continue to allow it.
 */
public Action:enginer_OnMyModuleDisable(String:refusalmsg[], maxlen)
{
	new size=GetArraySize(g_hVecPlayers);
	for(new x=0;x<size;x++)
	{
		new Handle:vecPlayer=GetArrayCell(g_hVecPlayers,x);
		new client=GetArrayCell(vecPlayer,VECTOR_PLAYERS_CLIENT);
		if(IsValidPlayer(client))
			enginer_StopArmoring(client);
	}

	return Plugin_Continue;
}

/**
 * The map has started.
 */
public enginer_OnMapStart()
{
	#if defined CONFIG_MANAGER
		ConfigMgr_CacheKv(g_moduleenginer, CM_CONFIGINDEX_FIRST, "enginerModule_ConfigCache");
	#endif
}

/**
 * Client is disconnecting from the server.
 *
 * @param client	The client index.
 */
public enginer_OnClientDisconnect(client)
{
	enginer_StopArmoring(client);
}

/**
 * Client has spawned.
 *
 * @param client	The client index.
 *
 */
public enginer_PlayerSpawn(client)
{
	if(GetClientTeam(client)!=g_enginerUpgradeConfigCache[UpgradeConfig_TeamLock])
	{
		enginer_StopArmoring(client);
		if(GetPlayerUpgradeLevel(client,g_moduleenginer)>0 && g_hEnginerPlayerIsArmoring[client]==INVALID_HANDLE)
			g_hEnginerPlayerIsArmoring[client]=CreateTimer(g_enginerUpgradeConfigCache[UpgradeConfig_Interval],Timer_OnEnginerInterval,client,TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
	}
	else
		enginer_StopArmoring(client);
}

/**
 * Client has purchased an Upgrade
 */
public enginer_OnUpgradePurchase(client, level)
{
	enginer_StopArmoring(client);
	g_hEnginerPlayerIsArmoring[client]=CreateTimer(g_enginerUpgradeConfigCache[UpgradeConfig_Interval],Timer_OnEnginerInterval,client,TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
}

/**
 * Client has sold an Upgrade
 */
public enginer_OnUpgradeSell(client, level)
{
	enginer_StopArmoring(client);
	if(level>0)
	{
		if(g_hEnginerPlayerIsArmoring[client]==INVALID_HANDLE)
			g_hEnginerPlayerIsArmoring[client]=CreateTimer(g_enginerUpgradeConfigCache[UpgradeConfig_Interval],Timer_OnEnginerInterval,client,TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
	}
}

public Action:Timer_OnEnginerInterval(Handle:timer,any:client)
{
	if(IsValidPlayer(client))
	{
		if(IsPlayerAlive(client))
		{
			new newarmor, oldarmor;
			new level=GetPlayerUpgradeLevel(client,g_moduleenginer);
			new team=GetClientTeam(client);
			
			for(new teammate=1;teammate<=MaxClients;teammate++)
			{
				if(IsClientInGame(teammate))
				{
					if(AreValidPlayers(client,teammate)&&(team==GetClientTeam(teammate)||g_CoreConfigCache[CoreConfig_freeforall])&&IsPlayerAlive(teammate)) 
					{
						oldarmor = GetClientHealth(teammate);
						newarmor=GetClientHealth(teammate)+RoundToNearest(level*g_enginerUpgradeConfigCache[UpgradeConfig_Amount]);
						if(newarmor>GetModifiedHealth(teammate))
							newarmor=GetModifiedHealth(teammate);
						SetEntData(teammate, g_iArmorOffset, newarmor, 4, true);
						SetXP(client, newarmor-oldarmor, 0);
						GettingXPArmoring[client] += newarmor-oldarmor;

						if(g_enginerUpgradeConfigCache[UpgradeConfig_Effects]==2 || g_enginerUpgradeConfigCache[UpgradeConfig_Effects]==3)
						{
							if(!IsValidEntity(EffectEnt[client]))
							if(newarmor != GetModifiedHealth(teammate))
								CreateEffect(client);
						}
					}
				}
			}
		}
		else
		{
			g_hEnginerPlayerIsArmoring[client]=INVALID_HANDLE;
			return Plugin_Stop;
		}
		return Plugin_Continue;
	}

	g_hEnginerPlayerIsArmoring[client]=INVALID_HANDLE;
	return Plugin_Stop;
}

public enginer_StopArmoring(client)
{
	if(g_hEnginerPlayerIsArmoring[client]!=INVALID_HANDLE)
	{
		KillTimer(g_hEnginerPlayerIsArmoring[client]);
		g_hEnginerPlayerIsArmoring[client]=INVALID_HANDLE;
	}
}