/**
 * This module's identifier.
 */
new Module:g_moduleghoul;

new g_ghoulUpgradeConfigCache[UpgradeConfigs];

new iExtraHealth[MAXPLAYERS+1];

/**
 * Register this module.
 */
ghoul_Register()
{
	// Define all the module's data as layed out by enum ModuleData in project.inc.
	new moduledata[ModuleData];
	
	moduledata[ModuleData_Disabled] = false;
	moduledata[ModuleData_Hidden] = false;
	strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "ghoul");
	strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "ghoul");
	strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "ghoul");
	
	new Module:dependencies[MM_DATA_DEPENDENCIES];
	dependencies[0] = g_moduleCore;
	dependencies[1] = INVALID_MODULE;
	moduledata[ModuleData_Dependencies] = dependencies;

	new Module:interlocks[MM_DATA_INTERLOCKS];
	interlocks[0] = INVALID_MODULE;
	moduledata[ModuleData_Interlocks] = interlocks;

	moduledata[ModuleData_TeamLock] = 0;
	moduledata[ModuleData_MaxLevel] = 10;
	moduledata[ModuleData_Cost] = 5;
	moduledata[ModuleData_iCost] = 5;

	// Send this array of data to the module manager.
	g_moduleghoul = ModuleMgr_Register(moduledata);

	// Now register the events we're going to use.
	#if defined EVENT_MANAGER
		EventMgr_RegisterEvent(g_moduleghoul, "Event_OnEventsRegister", "ghoul_OnEventsRegister");
	#endif  
  
	// Register config file(s) that this module will use.
	#if defined CONFIG_MANAGER
		ConfigMgr_Register(g_moduleghoul, "ghoul_OnConfigReload", "configs/thc_rpg/ghoul.txt");
	#endif
}

/**
 * Register all events here.
 */
public ghoul_OnEventsRegister()
{
	// Register all the events needed for this module.
	EventMgr_RegisterEvent(g_moduleghoul, "Event_OnMapStart",			 "ghoul_OnMapStart");
	EventMgr_RegisterEvent(g_moduleghoul, "Event_OnClientPutInServer",	 "ghoul_OnClientPutInServer");
	#if defined PROJECT_GAME_CSS
		EventMgr_RegisterEvent(g_moduleghoul, "Event_PlayerDeath",			 "ghoul_PlayerDeath");
		EventMgr_RegisterEvent(g_moduleghoul, "Event_PlayerSpawn",		   "ghoul_PlayerSpawn");
	#endif
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public ghoul_OnEventsReady()
{
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public ghoul_OnConfigReload(configindex)
{
	#if defined LOG_MANAGER
		LogMgr_Print(g_moduleghoul, LogType_Normal, "ghoulConfigReload", "Reloaded ghoul module's config (index %d)", configindex);
	#endif

	ConfigMgr_CacheKv(g_moduleghoul, CM_CONFIGINDEX_FIRST, "ghoulModule_ConfigCache");
}

/**
  * Read config values
  */
public KvCache:ghoulModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
	// Read Upgrade config
	if(StrEqual(sectionname, "ghoul", false))
	{
		g_ghoulUpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
		g_ghoulUpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
		g_ghoulUpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
		g_ghoulUpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
		g_ghoulUpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");
		g_ghoulUpgradeConfigCache[UpgradeConfig_Amount] = KvGetFloat(kv, "amount");

		if(g_ghoulUpgradeConfigCache[UpgradeConfig_Disable]==1)
			ModuleMgr_Disable(g_moduleghoul);
		
		ModuleMgr_WriteCell(g_moduleghoul, ModuleData_MaxLevel, g_ghoulUpgradeConfigCache[UpgradeConfig_MaxLevel]);
		ModuleMgr_WriteCell(g_moduleghoul, ModuleData_Cost, g_ghoulUpgradeConfigCache[UpgradeConfig_Cost]);
		ModuleMgr_WriteCell(g_moduleghoul, ModuleData_iCost, g_ghoulUpgradeConfigCache[UpgradeConfig_iCost]);
	}
}
#endif

/**
 * The map has started.
 */
public ghoul_OnMapStart()
{
	#if defined CONFIG_MANAGER
		ConfigMgr_CacheKv(g_moduleghoul, CM_CONFIGINDEX_FIRST, "ghoulModule_ConfigCache");
	#endif
}

public ghoul_OnClientPutInServer(client)
{
	iExtraHealth[client] = 0;
}

public ghoul_PlayerDeath(victim, attacker, const String:weapon[], bool:headshot)
{
	if(AreValidPlayers(victim, attacker))
	{
		iExtraHealth[victim] = 0;
		new level = GetPlayerUpgradeLevel(attacker, g_moduleghoul);
		if(level > 0 && iExtraHealth[attacker] < 500 && (StrEqual(weapon, "knife", false) || StrEqual(weapon, "hegrenade")))
		{
			iExtraHealth[attacker] += RoundToNearest(g_ghoulUpgradeConfigCache[UpgradeConfig_Amount]*level);
			SetModifiedHealth(attacker, GetModifiedHealth(attacker)+RoundToNearest(g_ghoulUpgradeConfigCache[UpgradeConfig_Amount]*level));
			SetEntityHealth(attacker, GetClientHealth(attacker)+RoundToNearest(g_ghoulUpgradeConfigCache[UpgradeConfig_Amount]*level));
		}
	}
}

public ghoul_PlayerSpawn(client)
{
	if(IsValidPlayer(client))
	{
		new level = GetPlayerUpgradeLevel(client, g_moduleghoul);
		if(level > 0)
		{
			SetModifiedHealth(client, GetModifiedHealth(client)+iExtraHealth[client]);
			SetEntityHealth(client, GetModifiedHealth(client));
		}
		else
			iExtraHealth[client] = 0;
	}
}