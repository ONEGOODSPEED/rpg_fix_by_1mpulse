new Module:g_moduleregen_ammo;

new g_regen_ammoUpgradeConfigCache[UpgradeConfigs];

// CS:GO specific
new Handle:g_hGiveReserveAmmo;
new Handle:g_hResupplyTimer;

AmmoRegen_OnPluginStart()
{
	new Handle:hGConf = LoadGameConfigFile("thebestrpg_regenammo.games");
	if (hGConf == INVALID_HANDLE)
	{
		SetFailState("Can't find thebestrpg_regenammo.games.txt gamedata file.");
	}
	
	StartPrepSDKCall(SDKCall_Entity);
	if (!PrepSDKCall_SetFromConf(hGConf, SDKConf_Signature, "CBaseCombatWeapon::GiveReserveAmmo"))
	{
		CloseHandle(hGConf);
		SetFailState("Can't find CBaseCombatWeapon::GiveReserveAmmo signature.");
	}
	PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain); // ammo position 1 = primary, 2 = secondary
	PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain); // amount of ammo to give
	PrepSDKCall_AddParameter(SDKType_Bool, SDKPass_Plain); // suppress ammo pickup sound
	PrepSDKCall_AddParameter(SDKType_CBasePlayer, SDKPass_Pointer, VDECODE_FLAG_ALLOWNULL); // optional owner. tries weapon owner if null.
	// if owner has ammo of that weapon's ammotype in his m_iAmmo, add to this array like before.
	// if the owner doesn't have ammo in m_iAmmo, use the new m_iPrimaryReserveAmmoCount props.
	PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain); // return amount of bullets missing until the max is reached including what we're about to add with this call..
	g_hGiveReserveAmmo = EndPrepSDKCall();
	CloseHandle(hGConf);
	if (g_hGiveReserveAmmo == INVALID_HANDLE)
	{
		SetFailState("Failed to prepare CBaseCombatWeapon::GiveReserveAmmo SDK call.");
	}
}

regen_ammo_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "regen_ammo");
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "RegenerateAmmo");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "Gives you the ability to steadily regain ammo for the active gun until it is fully replenished");

    new Module:dependencies[MM_DATA_DEPENDENCIES];
    dependencies[0] = g_moduleCore;
    dependencies[1] = INVALID_MODULE;
    moduledata[ModuleData_Dependencies] = dependencies;

    new Module:interlocks[MM_DATA_INTERLOCKS];
    interlocks[0] = INVALID_MODULE;
    moduledata[ModuleData_Interlocks] = interlocks;

    moduledata[ModuleData_TeamLock] = 0;
    moduledata[ModuleData_MaxLevel] = 5;
    moduledata[ModuleData_Cost] = 5;
    moduledata[ModuleData_iCost] = 10;

    // Send this array of data to the module manager.
    g_moduleregen_ammo = ModuleMgr_Register(moduledata);

    // Now register the events we're going to use.
    #if defined EVENT_MANAGER
        EventMgr_RegisterEvent(g_moduleregen_ammo, "Event_OnEventsRegister", "regen_ammo_OnEventsRegister");
    #endif  
  
    // Register config file(s) that this module will use.
    #if defined CONFIG_MANAGER
        ConfigMgr_Register(g_moduleregen_ammo, "regen_ammo_OnConfigReload", "configs/thc_rpg/regen_ammo.txt");
    #endif
}

/**
 * Register all events here.
 */
public regen_ammo_OnEventsRegister()
{
    // Register all the events needed for this module.
    EventMgr_RegisterEvent(g_moduleregen_ammo, "Event_OnMapStart",           "regen_ammo_OnMapStart");
	EventMgr_RegisterEvent(g_moduleregen_ammo, "Event_OnMapEnd",                "regen_ammo_OnMapEnd");
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public regen_ammo_OnEventsReady()
{
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public regen_ammo_OnConfigReload(configindex)
{
    #if defined LOG_MANAGER
        LogMgr_Print(g_moduleregen_ammo, LogType_Normal, "regen_ammoConfigReload", "Reloaded regen_ammo module's config (index %d)", configindex);
    #endif

    ConfigMgr_CacheKv(g_moduleregen_ammo, CM_CONFIGINDEX_FIRST, "regen_ammoModule_ConfigCache");
}

/**
  * Read config values
  */
public KvCache:regen_ammoModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
    // Read Upgrade config
    if(StrEqual(sectionname, "regen_ammo", false))
    {
        g_regen_ammoUpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
        g_regen_ammoUpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
        g_regen_ammoUpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
        g_regen_ammoUpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
        g_regen_ammoUpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");
        g_regen_ammoUpgradeConfigCache[UpgradeConfig_Interval] = KvGetFloat(kv, "interval");
        g_regen_ammoUpgradeConfigCache[UpgradeConfig_Amount] = KvGetFloat(kv, "amount");

        if(g_regen_ammoUpgradeConfigCache[UpgradeConfig_Disable]==1)
            ModuleMgr_Disable(g_moduleregen_ammo);

        ModuleMgr_WriteCell(g_moduleregen_ammo, ModuleData_TeamLock, g_regen_ammoUpgradeConfigCache[UpgradeConfig_TeamLock]);
        ModuleMgr_WriteCell(g_moduleregen_ammo, ModuleData_MaxLevel, g_regen_ammoUpgradeConfigCache[UpgradeConfig_MaxLevel]);
        ModuleMgr_WriteCell(g_moduleregen_ammo, ModuleData_Cost, g_regen_ammoUpgradeConfigCache[UpgradeConfig_Cost]);
        ModuleMgr_WriteCell(g_moduleregen_ammo, ModuleData_iCost, g_regen_ammoUpgradeConfigCache[UpgradeConfig_iCost]);
    }
    else
        ModuleMgr_Disable(g_moduleregen_ammo);
}
#endif

public regen_ammo_OnMapStart()
{
    #if defined CONFIG_MANAGER
        ConfigMgr_CacheKv(g_moduleregen_ammo, CM_CONFIGINDEX_FIRST, "regen_ammoModule_ConfigCache");
    #endif
	
	StartResupplyTimer();
}

public regen_ammo_OnMapEnd()
{
    g_hResupplyTimer = INVALID_HANDLE;
}

public Action:Timer_Resupply(Handle:timer)
{
	new iLevel, iPrimaryAmmoType;
	for(new i=1;i<=MaxClients;i++)
	{
		if(!IsClientInGame(i))
			continue;
			
		if(!IsPlayerAlive(i))
			continue;
			
		// Player didn't buy this upgrade yet.
		iLevel = GetPlayerUpgradeLevel(i,g_moduleregen_ammo);
		if(iLevel <= 0)
			continue;
		
		LOOP_CLIENTWEAPONS(i, iWeapon, iIndex)
		{
			iPrimaryAmmoType = Weapon_GetPrimaryAmmoType(iWeapon);
			// Grenades and knives have m_iClip1 = -1 or m_iPrimaryAmmoType = -1 respectively.
			// Don't try to refill those.
			if(iPrimaryAmmoType < 0 || Weapon_GetPrimaryClip(iWeapon) < 0)
				continue;
			
			if (g_hGiveReserveAmmo != INVALID_HANDLE)
			{
				SDKCall(g_hGiveReserveAmmo, iWeapon, 1, iLevel, true, -1);
			}
		}
	}
	
	return Plugin_Continue;
}

StartResupplyTimer()
{
	if (g_hResupplyTimer != INVALID_HANDLE)
	{
		CloseHandle(g_hResupplyTimer);
		g_hResupplyTimer = INVALID_HANDLE;
	}
	g_hResupplyTimer = CreateTimer(g_regen_ammoUpgradeConfigCache[UpgradeConfig_Interval], Timer_Resupply, _, TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
}