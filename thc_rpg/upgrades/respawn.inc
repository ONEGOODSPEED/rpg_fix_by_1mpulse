
new respawn_amount[MAXPLAYERS+1];
new Module:g_modulerespawn;
new g_respawnUpgradeConfigCache[UpgradeConfigs];
new Handle:RespawnDelTimer[MAXPLAYERS+1];

respawn_Register()
{
	new moduledata[ModuleData];
	
	moduledata[ModuleData_Disabled] = false;
	moduledata[ModuleData_Hidden] = false;
	strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "respawn");
	strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "respawn");
	strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "respawn");

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

	g_modulerespawn = ModuleMgr_Register(moduledata);

	#if defined EVENT_MANAGER
		EventMgr_RegisterEvent(g_modulerespawn, "Event_OnEventsRegister", "respawn_OnEventsRegister");
	#endif
	
	#if defined CONFIG_MANAGER
		ConfigMgr_Register(g_modulerespawn, "respawn_OnConfigReload", "configs/thc_rpg/respawn.txt");
	#endif
}

/**
 * Register all events here.
 */
public respawn_OnEventsRegister()
{
    // Register all the events needed for this module.
    EventMgr_RegisterEvent(g_modulerespawn, "Event_OnMapStart",                   "respawn_OnMapStart");
	EventMgr_RegisterEvent(g_modulerespawn, "Event_OnClientDisconnect",		   "respawn_OnClientDisconnect");
	#if defined PROJECT_GAME_CSS
		EventMgr_RegisterEvent(g_modulerespawn, "Event_PlayerDeath",              "respawn_PlayerDeath");
		EventMgr_RegisterEvent(g_modulerespawn, "Event_PlayerSpawn",              "respawn_PlayerSpawn");
		EventMgr_RegisterEvent(g_modulerespawn, "Event_RoundEnd",				  "respawn_RoundEnd");
	#endif
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public respawn_OnEventsReady()
{
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public respawn_OnConfigReload(configindex)
{
    #if defined LOG_MANAGER
        LogMgr_Print(g_modulerespawn, LogType_Normal, "respawnConfigReload", "Reloaded speed module's config (index %d)", configindex);
    #endif

    ConfigMgr_CacheKv(g_modulerespawn, CM_CONFIGINDEX_FIRST, "respawnModule_ConfigCache");
}

/**
 * Read config values
 */
public KvCache:respawnModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
    // Read Upgrade config
    if(StrEqual(sectionname, "respawn", false))
    {
		g_respawnUpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
		g_respawnUpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
		g_respawnUpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
		g_respawnUpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
		g_respawnUpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");
		g_respawnUpgradeConfigCache[UpgradeConfig_Percent] = KvGetFloat(kv, "percent");
		g_respawnUpgradeConfigCache[UpgradeConfig_Mode] = KvGetNum(kv, "amount");
		g_respawnUpgradeConfigCache[UpgradeConfig_Type] = KvGetNum(kv, "type");
		g_respawnUpgradeConfigCache[UpgradeConfig_Interval] = KvGetFloat(kv, "interval");
		if(g_respawnUpgradeConfigCache[UpgradeConfig_Disable]==1)
		ModuleMgr_Disable(g_modulerespawn);
		ModuleMgr_WriteCell(g_modulerespawn, ModuleData_TeamLock, g_respawnUpgradeConfigCache[UpgradeConfig_TeamLock]);
		ModuleMgr_WriteCell(g_modulerespawn, ModuleData_MaxLevel, g_respawnUpgradeConfigCache[UpgradeConfig_MaxLevel]);
		ModuleMgr_WriteCell(g_modulerespawn, ModuleData_Cost, g_respawnUpgradeConfigCache[UpgradeConfig_Cost]);
		ModuleMgr_WriteCell(g_modulerespawn, ModuleData_iCost, g_respawnUpgradeConfigCache[UpgradeConfig_iCost]);
    }
    else
        ModuleMgr_Disable(g_modulerespawn);
}
#endif

/**
 * The map has started.
 */
public respawn_OnMapStart()
{
    #if defined CONFIG_MANAGER
        ConfigMgr_CacheKv(g_modulerespawn, CM_CONFIGINDEX_FIRST, "respawnModule_ConfigCache");
    #endif
}
new Float:DeadPosition[3];

public respawn_PlayerDeath(victim, attacker, const String:weapon[], bool:headshot)
{
	GetClientAbsOrigin(victim, DeadPosition);
	if (!IsPlayerAlive(victim))
	{
		new level = GetPlayerUpgradeLevel(victim, g_modulerespawn);
		new limit = g_respawnUpgradeConfigCache[UpgradeConfig_Mode];
		if(level > 0 && GetRandomFloat(0.0, 1.0) <= g_respawnUpgradeConfigCache[UpgradeConfig_Percent]*level)
		{
			if (respawn_amount[victim] >= limit)
			{
				TransMgr_PrintText(victim, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Respawn amount",limit);
			}
			else if (respawn_amount[victim] < limit)
			{
				new Float:fTimerResp = g_respawnUpgradeConfigCache[UpgradeConfig_Interval];
				RespawnDelTimer[victim] = CreateTimer(fTimerResp, Respawn, victim, TIMER_FLAG_NO_MAPCHANGE);
				TransMgr_PrintText(victim, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Respawn",RoundToNearest(fTimerResp));
			}
			respawn_amount[victim]++;
		}
	}
}

public Action:Respawn(Handle:timer, any:client)
{
	RespawnDelTimer[client] = INVALID_HANDLE;
	if (IsClientInGame(client) && !IsPlayerAlive(client) && GetClientTeam(client) > 1)
	{
		RespawnType(client);
	}
}

public respawn_PlayerSpawn(client)
{
	if (RespawnDelTimer[client] != INVALID_HANDLE)
	{
		KillTimer(RespawnDelTimer[client]);
		RespawnDelTimer[client] = INVALID_HANDLE;
	}
}


public respawn_OnClientDisconnect(client)
{
	if (RespawnDelTimer[client] != INVALID_HANDLE)
	{
		KillTimer(RespawnDelTimer[client]);
		RespawnDelTimer[client] = INVALID_HANDLE;
	}
}

public respawn_RoundEnd(winner)
{
	for (new i = 1; i <= MaxClients; i++)
	{
		respawn_amount[i] = 0;
	}
}

RespawnType(client)
{
	if(g_respawnUpgradeConfigCache[UpgradeConfig_Type] == 1)
	{
		CS_RespawnPlayer(client);
	}
	else if(g_respawnUpgradeConfigCache[UpgradeConfig_Type] == 2)
	{
		CS_RespawnPlayer(client);
		TeleportEntity(client, DeadPosition, NULL_VECTOR, NULL_VECTOR);
	}
}