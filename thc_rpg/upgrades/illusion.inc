new Module:g_moduleillusion;

new g_illusionUpgradeConfigCache[UpgradeConfigs];

#define EF_BONEMERGE                (1 << 0)
#define EF_NOSHADOW                 (1 << 4)
#define EF_NORECEIVESHADOW          (1 << 6)
#define EF_PARENT_ANIMATES          (1 << 9)

new iModel[MAXPLAYERS+1] = {INVALID_ENT_REFERENCE, ...};

illusion_Register()
{
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "illusion");
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "illusion");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "illusion");

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

    g_moduleillusion = ModuleMgr_Register(moduledata);
    
    #if defined EVENT_MANAGER
        EventMgr_RegisterEvent(g_moduleillusion, "Event_OnEventsRegister", "illusion_OnEventsRegister");
    #endif
        
    #if defined CONFIG_MANAGER
        ConfigMgr_Register(g_moduleillusion, "illusion_OnConfigReload", "configs/thc_rpg/illusion.txt");
    #endif
}

public illusion_OnEventsRegister()
{
    EventMgr_RegisterEvent(g_moduleillusion, "Event_OnMapStart",              "illusion_OnMapStart");
    EventMgr_RegisterEvent(g_moduleillusion, "Event_OnClientPutInServer",     "illusion_OnClientPutInServer");
    EventMgr_RegisterEvent(g_moduleillusion, "Event_OnClientDisconnect",      "illusion_OnClientDisconnect");
    #if defined PROJECT_GAME_CSS
        EventMgr_RegisterEvent(g_moduleillusion, "Event_RoundEnd",                "illusion_RoundEnd");
        EventMgr_RegisterEvent(g_moduleillusion, "Event_PlayerDeath",             "illusion_PlayerDeath");
      #endif
}

public illusion_OnEventsReady()
{
}

#if defined CONFIG_MANAGER

public illusion_OnConfigReload(configindex)
{
    #if defined LOG_MANAGER
        LogMgr_Print(g_moduleillusion, LogType_Normal, "illusionConfigReload", "Reloaded illusion module's config (index %d)", configindex);
    #endif

    ConfigMgr_CacheKv(g_moduleillusion, CM_CONFIGINDEX_FIRST, "illusionModule_ConfigCache");
}

public KvCache:illusionModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
    if(StrEqual(sectionname, "illusion", false))
    {
        g_illusionUpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
        g_illusionUpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
        g_illusionUpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
        g_illusionUpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
        g_illusionUpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");
		g_illusionUpgradeConfigCache[UpgradeConfig_Percent] = KvGetFloat(kv, "percent");

        if(g_illusionUpgradeConfigCache[UpgradeConfig_Disable]==1)
          ModuleMgr_Disable(g_moduleillusion);

        ModuleMgr_WriteCell(g_moduleillusion, ModuleData_MaxLevel, g_illusionUpgradeConfigCache[UpgradeConfig_MaxLevel]);
        ModuleMgr_WriteCell(g_moduleillusion, ModuleData_Cost, g_illusionUpgradeConfigCache[UpgradeConfig_Cost]);
        ModuleMgr_WriteCell(g_moduleillusion, ModuleData_iCost, g_illusionUpgradeConfigCache[UpgradeConfig_iCost]);
    }
    else
        ModuleMgr_Disable(g_moduleillusion);
}
#endif

public illusion_OnMapStart()
{
    #if defined CONFIG_MANAGER
        ConfigMgr_CacheKv(g_moduleillusion, CM_CONFIGINDEX_FIRST, "illusionModule_ConfigCache");
    #endif
}

public illusion_OnClientPutInServer(client)
{
	SDKHook(client, SDKHook_OnTakeDamage, OnTakeDamage_illusion);
}

public illusion_OnClientDisconnect(client)
{
	RemoveIcon(client);
}

public illusion_RoundEnd(client, winner)
{
	RemoveIcon(client);
}

public illusion_PlayerDeath(victim, attacker, const String:weapon[], bool:headshot)
{
	RemoveIcon(victim);
}

public Action:OnTakeDamage_illusion(iClient, &iAttaker, &inflictor, &Float:damage, &damagetype)
{
	new health;
	health = GetClientHealth(iClient);
	new level = GetPlayerUpgradeLevel(iClient, g_moduleillusion);
	if((health <= 100) && (level > 0))
	{
		if(GetRandomFloat(0.0, 1.0) <= (g_illusionUpgradeConfigCache[UpgradeConfig_Percent]*level))
		{
			new iAlpha_level=GetPlayerUpgradeLevel(iClient, g_modulestealth);
			new iAlpha=(255/g_stealthUpgradeConfigCache[UpgradeConfig_MaxLevel])*(g_stealthUpgradeConfigCache[UpgradeConfig_MaxLevel]-iAlpha_level);
			if(iAlpha<g_stealthUpgradeConfigCache[UpgradeConfig_MinAlpha])
			{
				iAlpha=g_stealthUpgradeConfigCache[UpgradeConfig_MinAlpha];
			}
			RemoveIcon(iClient);
			new String:buffer[255];
			GetClientModel(iClient, buffer, sizeof(buffer));
			new iModel_b;
			iModel_b = EntRefToEntIndex(iModel[iClient]);
			if(iModel_b == INVALID_ENT_REFERENCE)
			{
				new Ent = CreateEntityByName("prop_dynamic_override");
				DispatchKeyValue(Ent, "model", buffer);
				DispatchKeyValue(Ent, "disablereceiveshadows", "1");
				DispatchKeyValue(Ent, "disableshadows", "1");
				DispatchKeyValue(Ent, "solid", "0");
				DispatchKeyValue(Ent, "spawnflags", "256");
				DispatchKeyValue(Ent, "rendermode", "1");
				if(iAlpha_level > 0) 
				{
					SetEntityRenderMode(Ent, RENDER_TRANSCOLOR);
					Entity_SetRenderColor(Ent, -1, -1, -1, iAlpha);
					//DispatchKeyValue(Ent, "renderamt", iAlpha);
				}
				else
				{
					SetEntityRenderMode(Ent, RENDER_TRANSCOLOR);
					Entity_SetRenderColor(Ent, -1, -1, -1, 255);
					//DispatchKeyValue(Ent, "renderamt", iAlpha);
				}
				SetEntProp(Ent, Prop_Send, "m_CollisionGroup", 11);
				DispatchSpawn(Ent);
				SetEntProp(Ent, Prop_Send, "m_fEffects", EF_BONEMERGE|EF_NOSHADOW|EF_NORECEIVESHADOW|EF_PARENT_ANIMATES);
				SetVariantString("!activator");
				new Float:fPos[3];
				GetClientAbsOrigin(iClient, fPos);
				TeleportEntity(Ent, fPos, NULL_VECTOR, NULL_VECTOR);
				iModel[iClient] = EntIndexToEntRef(Ent);
				CreateTimer(5.0, TimeriClient_illusion, any:iClient);
			}
		}
	}
}

public Action:TimeriClient_illusion(Handle:timer, any:iClient)
{
	RemoveIcon(iClient);
}

RemoveIcon(iClient)
{
	new iEnt = EntRefToEntIndex(iModel[iClient]);
	if(iEnt != INVALID_ENT_REFERENCE) AcceptEntityInput(iEnt, "Kill");
	iModel[iClient] = INVALID_ENT_REFERENCE;
}