/**
 * This module's identifier.
 */
new Module:g_moduleFireRate;

new g_FireRateUpgradeConfigCache[UpgradeConfigs];
new Float:g_fModifyNextAttack[MAXPLAYERS+1];

/**
 * Register this module.
 */
FireRate_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "FireRate");
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "firerate");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "Increases the firerate of weapons.");

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
    g_moduleFireRate = ModuleMgr_Register(moduledata);
    
    // Now register the events we're going to use.
    #if defined EVENT_MANAGER
        EventMgr_RegisterEvent(g_moduleFireRate, "Event_OnEventsRegister", "FireRate_OnEventsRegister");
    #endif
        
    // Register config file(s) that this module will use.
    #if defined CONFIG_MANAGER
        ConfigMgr_Register(g_moduleFireRate, "FireRate_OnConfigReload", "configs/thc_rpg/firerate.txt");
    #endif
}

/**
 * Register all events here.
 */
public FireRate_OnEventsRegister()
{
    EventMgr_RegisterEvent(g_moduleFireRate, "Event_OnMapStart",              "FireRate_OnMapStart");
    EventMgr_RegisterEvent(g_moduleFireRate, "Event_OnClientPutInServer",     "FireRate_OnClientPutInServer");
    EventMgr_RegisterEvent(g_moduleFireRate, "Event_OnClientDisconnect",      "FireRate_OnClientDisconnect");
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public FireRate_OnEventsReady()
{
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public FireRate_OnConfigReload(configindex)
{
    #if defined LOG_MANAGER
        LogMgr_Print(g_moduleFireRate, LogType_Normal, "FireRateConfigReload", "Reloaded FireRate module's config (index %d)", configindex);
    #endif

    ConfigMgr_CacheKv(g_moduleFireRate, CM_CONFIGINDEX_FIRST, "FireRateModule_ConfigCache");
}

/**
  * Read config values
  */
public KvCache:FireRateModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
    // Read Upgrade config
    if(StrEqual(sectionname, "firerate", false))
    {
        g_FireRateUpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
        g_FireRateUpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
        g_FireRateUpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
        g_FireRateUpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
        g_FireRateUpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");

        if(g_FireRateUpgradeConfigCache[UpgradeConfig_Disable]==1)
          ModuleMgr_Disable(g_moduleFireRate);

        ModuleMgr_WriteCell(g_moduleFireRate, ModuleData_MaxLevel, g_FireRateUpgradeConfigCache[UpgradeConfig_MaxLevel]);
        ModuleMgr_WriteCell(g_moduleFireRate, ModuleData_Cost, g_FireRateUpgradeConfigCache[UpgradeConfig_Cost]);
        ModuleMgr_WriteCell(g_moduleFireRate, ModuleData_iCost, g_FireRateUpgradeConfigCache[UpgradeConfig_iCost]);
    }
    else
        ModuleMgr_Disable(g_moduleFireRate);
}
#endif

public FireRate_OnMapStart()
{
    #if defined CONFIG_MANAGER
        ConfigMgr_CacheKv(g_moduleFireRate, CM_CONFIGINDEX_FIRST, "FireRateModule_ConfigCache");
    #endif
}

public FireRate_OnClientPutInServer(client)
{
	SDKHook(client, SDKHook_PostThink, Hook_OnPostThink_FireRate);
	SDKHook(client, SDKHook_PostThinkPost, Hook_OnPostThinkPost_FireRate);
}

public FireRate_OnClientDisconnect(client)
{
	g_fModifyNextAttack[client] = 0.0;
}

public Hook_OnPostThink_FireRate(client)
{
	new iButtons = GetClientButtons(client);
	if(iButtons & IN_ATTACK)
	{
		// Dead players can't shoot.
		if(!IsPlayerAlive(client))
			return;

		// Player didn't buy this upgrade yet.
		new iLevel = GetPlayerUpgradeLevel(client, g_moduleFireRate);
		if(iLevel <= 0)
			return;

		// Is he holding a weapon?
		new iWeaponIndex = GetEntPropEnt(client, Prop_Send, "m_hActiveWeapon");
		if (iWeaponIndex == INVALID_ENT_REFERENCE)
			return;
		
		// Credits to blodia for his weapon mod
		// The weapon can't be fired yet.
		if (GetGameTime() < GetEntPropFloat(iWeaponIndex, Prop_Send, "m_flNextPrimaryAttack"))
			return;
		
		// The player can't attack yet.
		if (GetGameTime() < GetEntPropFloat(client, Prop_Send, "m_flNextAttack"))
			return;
		
		// Empty clip.. Can't attack at all.
		if (GetEntProp(iWeaponIndex, Prop_Send, "m_iClip1") <= 0)
			return;
		
		// Only care for weapons with bullets/ammo
		if (GetEntProp(iWeaponIndex, Prop_Send, "m_iPrimaryAmmoType") < 0)
			return;
		
		g_fModifyNextAttack[client] = 1.0 / (1.0 + float(iLevel) * 0.1);
	}
}

public Hook_OnPostThinkPost_FireRate(client)
{
	if(g_fModifyNextAttack[client] <= 0.0)
		return;
	
	if(!IsPlayerAlive(client))
	{
		g_fModifyNextAttack[client] = 0.0;
		return;
	}
	
	new iWeapon = GetEntPropEnt(client, Prop_Send, "m_hActiveWeapon");
	if (iWeapon != INVALID_ENT_REFERENCE)
	{
		new Float:flNextPrimaryAttack = GetEntPropFloat(iWeapon, Prop_Send, "m_flNextPrimaryAttack");
		
		flNextPrimaryAttack -= GetGameTime();
		flNextPrimaryAttack *= g_fModifyNextAttack[client];
		flNextPrimaryAttack += GetGameTime();
		
		SetEntPropFloat(iWeapon, Prop_Send, "m_flNextPrimaryAttack", flNextPrimaryAttack);
	}

	g_fModifyNextAttack[client] = 0.0;
}