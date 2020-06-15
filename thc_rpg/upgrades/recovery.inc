new Handle:arrayWeapons[MAXPLAYERS+1]; 
/**
 * This module's identifier.
 */
new Module:g_modulerecovery;

new g_recoveryUpgradeConfigCache[UpgradeConfigs];

/**
 * Register this module.
 */
recovery_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "recovery");
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "recovery");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "recovery");
    
    new Module:dependencies[MM_DATA_DEPENDENCIES];
    dependencies[0] = g_moduleCore;
    dependencies[1] = INVALID_MODULE;
    moduledata[ModuleData_Dependencies] = dependencies;

    new Module:interlocks[MM_DATA_INTERLOCKS];
    interlocks[0] = INVALID_MODULE;
    moduledata[ModuleData_Interlocks] = interlocks;

    moduledata[ModuleData_TeamLock] = 0;
    moduledata[ModuleData_MaxLevel] = 3;
    moduledata[ModuleData_Cost] = 15;
    moduledata[ModuleData_iCost] = 15;

    // Send this array of data to the module manager.
    g_modulerecovery = ModuleMgr_Register(moduledata);

    // Now register the events we're going to use.
    #if defined EVENT_MANAGER
        EventMgr_RegisterEvent(g_modulerecovery, "Event_OnEventsRegister", "recovery_OnEventsRegister");
    #endif  
  
    // Register config file(s) that this module will use.
    #if defined CONFIG_MANAGER
        ConfigMgr_Register(g_modulerecovery, "recovery_OnConfigReload", "configs/thc_rpg/recovery.txt");
    #endif
}

/**
 * Register all events here.
 */
public recovery_OnEventsRegister()
{
    // Register all the events needed for this module.
	EventMgr_RegisterEvent(g_modulerecovery, "Event_OnMapStart",             "recovery_OnMapStart");
	EventMgr_RegisterEvent(g_modulerecovery, "Event_OnClientDisconnect",      "recovery_OnClientDisconnect");
	EventMgr_RegisterEvent(g_modulerecovery, "Event_OnConfigsExecuted",       "recovery_OnConfigsExecuted");
	//EventMgr_RegisterEvent(g_modulerecovery, "Event_OnClientPutInServer",     "recovery_OnClientPutInServer");
	#if defined PROJECT_GAME_CSS
		//EventMgr_RegisterEvent(g_modulerecovery, "Event_PlayerDeath",             "recovery_PlayerDeath");
		//EventMgr_RegisterEvent(g_modulerecovery, "Event_PlayerHurt",             "recovery_PlayerHurt");
		EventMgr_RegisterEvent(g_modulerecovery, "Event_RoundEnd",                "recovery_RoundEnd");
		EventMgr_RegisterEvent(g_modulerecovery, "Event_PlayerSpawn",           "recovery_PlayerSpawn");
	#endif
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public recovery_OnEventsReady()
{
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public recovery_OnConfigReload(configindex)
{
    #if defined LOG_MANAGER
        LogMgr_Print(g_modulerecovery, LogType_Normal, "recoveryConfigReload", "Reloaded recovery module's config (index %d)", configindex);
    #endif

    ConfigMgr_CacheKv(g_modulerecovery, CM_CONFIGINDEX_FIRST, "recoveryModule_ConfigCache");
}

/**
  * Read config values
  */
public KvCache:recoveryModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
    // Read Upgrade config
    if(StrEqual(sectionname, "recovery", false))
    {
        g_recoveryUpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
        g_recoveryUpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
        g_recoveryUpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
        g_recoveryUpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
        g_recoveryUpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");

        if(g_recoveryUpgradeConfigCache[UpgradeConfig_Disable]==1)
            ModuleMgr_Disable(g_modulerecovery);
		
        ModuleMgr_WriteCell(g_modulerecovery, ModuleData_MaxLevel, g_recoveryUpgradeConfigCache[UpgradeConfig_MaxLevel]);
        ModuleMgr_WriteCell(g_modulerecovery, ModuleData_Cost, g_recoveryUpgradeConfigCache[UpgradeConfig_Cost]);
        ModuleMgr_WriteCell(g_modulerecovery, ModuleData_iCost, g_recoveryUpgradeConfigCache[UpgradeConfig_iCost]);
    }
}
#endif

/**
 * The map has started.
 */
public recovery_OnMapStart()
{
	#if defined CONFIG_MANAGER
		ConfigMgr_CacheKv(g_modulerecovery, CM_CONFIGINDEX_FIRST, "recoveryModule_ConfigCache");
	#endif
}

public recovery_OnConfigsExecuted()
{
	for (new i = 1; i <= MaxClients; i++)
	{
		arrayWeapons[i] = CreateArray(128);
	}
}

public Action:CS_OnCSWeaponDrop(client, weaponIndex)
{
	if (!IsValidPlayer(client) || !IsValidEdict(weaponIndex))
		return;
	
	new level = GetPlayerUpgradeLevel(client, g_modulerecovery);
	if(weaponIndex < 1 || GetClientHealth(client) > 0 || level < 1)
		return;	
	
	decl String:weapon[64];
	GetEdictClassname(weaponIndex, weapon, sizeof(weapon));
	if(!StrEqual(weapon, "weapon_c4"))
	{
		if(level >= 1 && (StrEqual(weapon, "weapon_hegrenade") || StrEqual(weapon, "weapon_flashbang") || StrEqual(weapon, "weapon_smokegrenade") || StrEqual(weapon, "weapon_molotov") || StrEqual(weapon, "weapon_incgrenade") || StrEqual(weapon, "weapon_decoy")))
		{
			PushArrayString(arrayWeapons[client], weapon);
			PrintToConsole(client, "[DEBUG] Save weapon [WeaponDrop:Grenade]: %s", weapon);
		}
		else if(level >= 2 && (StrEqual(weapon, "weapon_glock") || StrEqual(weapon, "weapon_usp") || StrEqual(weapon, "weapon_p228") || StrEqual(weapon, "weapon_deagle") || StrEqual(weapon, "weapon_elite") || StrEqual(weapon, "weapon_fiveseven") || StrEqual(weapon, "weapon_p250") || StrEqual(weapon, "weapon_tec9") || StrEqual(weapon, "weapon_hkp2000")|| StrEqual(weapon, "weapon_usp_silencer")|| StrEqual(weapon, "weapon_cz75a")))
		{
			PushArrayString(arrayWeapons[client], weapon);
			PrintToConsole(client, "[DEBUG] Save weapon [WeaponDrop:Pistol]: %s", weapon);
		}
		else if(level >= 3)
		{
			PushArrayString(arrayWeapons[client], weapon);
			PrintToConsole(client, "[DEBUG] Save weapon [WeaponDrop:Other]: %s", weapon);
		}
	}
}

public OnEntityDestroyed(entity)
{
	if(!IsValidEdict(entity))
		return;
	
	decl String:classname[64];
	GetEdictClassname(entity, classname, sizeof(classname));
	if(StrContains(classname, "weapon_") != -1)
	{
		new client = GetEntPropEnt(entity, Prop_Data, "m_hOwnerEntity");
		if(!IsValidPlayer(client, false)) // >.< true = fail...
			return;
		
		if(IsPlayerAlive(client))
			return;
				
		PrintToConsole(client, "[DEBUG] Should save weapon [EntityDestr:All]: %s", classname);
		new level = GetPlayerUpgradeLevel(client, g_modulerecovery);
		if(level > 0)
		{
			if(level >= 1 && (StrEqual(classname, "weapon_hegrenade") || StrEqual(classname, "weapon_flashbang") || StrEqual(classname, "weapon_smokegrenade") || StrEqual(classname, "weapon_molotov") || StrEqual(classname, "weapon_incgrenade") || StrEqual(classname, "weapon_decoy")))
			{
				PushArrayString(arrayWeapons[client], classname);
				PrintToConsole(client, "[DEBUG] Save weapon [EntityDestr:Grenade]: %s", classname);
			}
			else if(level >= 2 && (StrEqual(classname, "weapon_glock") || StrEqual(classname, "weapon_usp") || StrEqual(classname, "weapon_p228") || StrEqual(classname, "weapon_deagle") || StrEqual(classname, "weapon_elite") || StrEqual(classname, "weapon_fiveseven") || StrEqual(classname, "weapon_p250") || StrEqual(classname, "weapon_tec9") || StrEqual(classname, "hkp2000")|| StrEqual(classname, "usp_silencer")|| StrEqual(classname, "weapon_cz75a")))
			{
				PushArrayString(arrayWeapons[client], classname);
				PrintToConsole(client, "[DEBUG] Save weapon [EntityDestr:Pistol]: %s", classname);
			}
			else if(level >= 3)
			{
				PushArrayString(arrayWeapons[client], classname);
				PrintToConsole(client, "[DEBUG] Save weapon [EntityDestr:Other]: %s", classname);
			}
		}
	}
}

public recovery_RoundEnd(winner)
{
	CreateTimer(4.5, TimerRoundEnd);
}

public Action:TimerRoundEnd(Handle:timer)
{
	for(new client = 1; client <= MaxClients; client++)
	{
		if(!IsValidPlayer(client))
			continue;
		if(!IsPlayerAlive(client))
			continue;
			
		new level = GetPlayerUpgradeLevel(client, g_modulerecovery);
		if(level < 1)
			continue;
		
		for(new slot = 0; slot <= 5; slot++)
		{
			new wpn = GetPlayerWeaponSlot(client, slot);
			if(wpn == -1)
				continue;
			
			decl String:weapon[64];
			GetEdictClassname(wpn, weapon, sizeof(weapon));

			if(level >= 1 && (StrEqual(weapon, "weapon_hegrenade") || StrEqual(weapon, "weapon_flashbang") || StrEqual(weapon, "weapon_smokegrenade") || StrEqual(weapon, "weapon_molotov") || StrEqual(weapon, "weapon_incgrenade") || StrEqual(weapon, "weapon_decoy")))
			{
				PushArrayString(arrayWeapons[client], weapon);
				PrintToConsole(client, "[DEBUG] Save weapon [RoundEnd:Grenade]: %s", weapon);
			}
			else if(level >= 2 && (StrEqual(weapon, "weapon_glock") || StrEqual(weapon, "weapon_usp") || StrEqual(weapon, "weapon_p228") || StrEqual(weapon, "weapon_deagle") || StrEqual(weapon, "weapon_elite") || StrEqual(weapon, "weapon_fiveseven") || StrEqual(weapon, "weapon_p250") || StrEqual(weapon, "weapon_tec9") || StrEqual(weapon, "hkp2000")|| StrEqual(weapon, "usp_silencer")|| StrEqual(weapon, "weapon_cz75a")))
			{
				PushArrayString(arrayWeapons[client], weapon);
				PrintToConsole(client, "[DEBUG] Save weapon [RoundEnd:Pistol]: %s", weapon);
			}
			else if(level >= 3)
			{
				PushArrayString(arrayWeapons[client], weapon);
				PrintToConsole(client, "[DEBUG] Save weapon [RoundEnd:Other]: %s", weapon);
			}
		}
	}
}

public recovery_PlayerSpawn(client)
{
	if(IsValidPlayer(client))
	{
		new level = GetPlayerUpgradeLevel(client, g_modulerecovery);
		if(level > 0)
		{
			new weapons = GetArraySize(arrayWeapons[client]);
			if (!weapons)
				return;
			for (new x=0; x <= 3; x++)
			{
				new weapon = GetPlayerWeaponSlot(client, x);
				if(!IsValidEdict(weapon))
					continue;
				decl String:sWeapon[64];
				GetEdictClassname(weapon, sWeapon, sizeof(sWeapon));
				if(StrEqual(sWeapon, "weapon_c4") || StrEqual(sWeapon, "weapon_knife"))
					continue;
				if((x == 1 && level < 2) || (x == 0 && level < 3))
					continue;
				if(RemovePlayerItem(client, weapon))
				{
					//RemoveEdict(weapon);
					PrintToConsole(client, "[DEBUG] Remove weapon [PlayerSpawn]: %s", weapon);
				}
				//AcceptEntityInput(weapon, "Kill");
			}
			for(new i, String:weapon[64]; i < weapons; i++)
			{
				GetArrayString(arrayWeapons[client], i, weapon, sizeof(weapon));
				if(StrEqual(weapon, "weapon_knife") || StrEqual(weapon, "weapon_c4"))
					continue;
				
				GivePlayerItem(client, weapon);
				PrintToConsole(client, "[DEBUG] Give weapon [PlayerSpawn]: %s", weapon);
				//PrintToChat(client, "[DEBUG] Recovery: %i give weapon \"%s\"", i, weapon);
			}
			ClearArray(arrayWeapons[client]);
		}
	}
}

public recovery_OnClientDisconnect(client)
{
	new weapons = GetArraySize(arrayWeapons[client]);
	if(weapons > 0)
		ClearArray(arrayWeapons[client]);
}