new Module:g_modulehelmet;

new g_helmetUpgradeConfigCache[UpgradeConfigs];

/**
 * Register this module.
 */
helmet_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "helmet");
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "helmet");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "Chance give player helmet.");

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
    g_modulehelmet = ModuleMgr_Register(moduledata);
    
    // Now register the events we're going to use.
    #if defined EVENT_MANAGER
        EventMgr_RegisterEvent(g_modulehelmet, "Event_OnEventsRegister", "helmet_OnEventsRegister");
    #endif
        
    // Register config file(s) that this module will use.
    #if defined CONFIG_MANAGER
        ConfigMgr_Register(g_modulehelmet, "helmet_OnConfigReload", "configs/thc_rpg/helmet.txt");
    #endif
}

/**
 * Register all events here.
 */
public helmet_OnEventsRegister()
{
    EventMgr_RegisterEvent(g_modulehelmet, "Event_OnMapStart",              "helmet_OnMapStart");
    #if defined PROJECT_GAME_CSS
        EventMgr_RegisterEvent(g_modulehelmet, "Event_PlayerSpawn",             "helmet_PlayerSpawn");
      #endif
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public helmet_OnEventsReady()
{
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public helmet_OnConfigReload(configindex)
{
    #if defined LOG_MANAGER
        LogMgr_Print(g_modulehelmet, LogType_Normal, "helmetConfigReload", "Reloaded helmet module's config (index %d)", configindex);
    #endif

    ConfigMgr_CacheKv(g_modulehelmet, CM_CONFIGINDEX_FIRST, "helmetModule_ConfigCache");
}

/**
  * Read config values
  */
public KvCache:helmetModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
    // Read Upgrade config
    if(StrEqual(sectionname, "helmet", false))
    {
        g_helmetUpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
        g_helmetUpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
        g_helmetUpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
        g_helmetUpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
        g_helmetUpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");
		g_helmetUpgradeConfigCache[UpgradeConfig_Percent] = KvGetFloat(kv, "percent");

        if(g_helmetUpgradeConfigCache[UpgradeConfig_Disable]==1)
          ModuleMgr_Disable(g_modulehelmet);

        ModuleMgr_WriteCell(g_modulehelmet, ModuleData_MaxLevel, g_helmetUpgradeConfigCache[UpgradeConfig_MaxLevel]);
        ModuleMgr_WriteCell(g_modulehelmet, ModuleData_Cost, g_helmetUpgradeConfigCache[UpgradeConfig_Cost]);
        ModuleMgr_WriteCell(g_modulehelmet, ModuleData_iCost, g_helmetUpgradeConfigCache[UpgradeConfig_iCost]);
    }
    else
        ModuleMgr_Disable(g_modulehelmet);
}
#endif

public helmet_OnMapStart()
{
    #if defined CONFIG_MANAGER
        ConfigMgr_CacheKv(g_modulehelmet, CM_CONFIGINDEX_FIRST, "helmetModule_ConfigCache");
    #endif
}

public helmet_PlayerSpawn(client)
{
	if (!client)
		return;
	
	if (GetEntProp(client, Prop_Send, "m_bHasHelmet"))
		return;
	
	new iLevel = GetPlayerUpgradeLevel(client, g_modulehelmet);
	if(iLevel <= 0)
		return;
	
	new Float:fChance = float(iLevel) * g_helmetUpgradeConfigCache[UpgradeConfig_Percent];
	if ((GetRandomFloat(0.0, 1.0) <= fChance) && IsPlayerAlive(client))
		SetEntProp(client, Prop_Send, "m_bHasHelmet", 1);
}