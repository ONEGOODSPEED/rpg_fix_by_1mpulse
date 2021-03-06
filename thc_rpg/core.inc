/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          core.inc
 *  Type:          Module
 *  Description:   Plugins Core Management
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
// IS DEFINED IN PROJECT.INC

new bool:pluginisending=false;
new Re_killtimer;


/**
 * Register this module.
 */
Core_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];

    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = true;
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "Core");
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "core");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "Plugins Core Management");
    // End the last one with INVALID_MODULE.
    moduledata[ModuleData_Dependencies][0] = INVALID_MODULE;

    // Send this array of data to the module manager.
    g_moduleCore = ModuleMgr_Register(moduledata);

    // Now register the events we're going to use.
    #if defined EVENT_MANAGER
        EventMgr_RegisterEvent(g_moduleCore, "Event_OnEventsRegister", "Core_OnEventsRegister");
    #endif

    // Register config file(s) that this module will use.
    #if defined CONFIG_MANAGER
        ConfigMgr_Register(g_moduleCore, "Core_OnConfigReload", "configs/thc_rpg/core.txt");
    #endif
}

/**
 * Register all events here.
 */
public Core_OnEventsRegister()
{
    // Register all the events needed for this module.
    EventMgr_RegisterEvent(g_moduleCore, "Event_OnAllPluginsLoaded",        "Core_OnAllPluginsLoaded");
    EventMgr_RegisterEvent(g_moduleCore, "Event_OnPluginEnd",               "Core_OnPluginEnd");
    EventMgr_RegisterEvent(g_moduleCore, "Event_OnMyModuleDisable",         "Core_OnMyModuleDisable");
    EventMgr_RegisterEvent(g_moduleCore, "Event_OnMapStart",                "Core_OnMapStart");
    EventMgr_RegisterEvent(g_moduleCore, "Event_OnConfigsExecuted",         "Core_OnConfigsExecuted");
    EventMgr_RegisterEvent(g_moduleCore, "Event_OnClientDisconnect",        "Core_OnClientDisconnect");
    #if defined PROJECT_GAME_CSS
        EventMgr_RegisterEvent(g_moduleCore, "Event_RoundStart",            "Core_RoundStart");
        EventMgr_RegisterEvent(g_moduleCore, "Event_RoundEnd",              "Core_RoundEnd");
        EventMgr_RegisterEvent(g_moduleCore, "Event_PlayerTeam",            "Core_PlayerTeam");
        EventMgr_RegisterEvent(g_moduleCore, "Event_PlayerSpawn",           "Core_PlayerSpawn");
        EventMgr_RegisterEvent(g_moduleCore, "Event_PlayerHurt",            "Core_PlayerHurt");
        EventMgr_RegisterEvent(g_moduleCore, "Event_PlayerDeath",           "Core_PlayerDeath");
        EventMgr_RegisterEvent(g_moduleCore, "Event_BombPlanted",           "Core_BombPlanted");
        EventMgr_RegisterEvent(g_moduleCore, "Event_BombDefused",           "Core_BombDefused");
        EventMgr_RegisterEvent(g_moduleCore, "Event_BombExploded",          "Core_BombExploded");
        EventMgr_RegisterEvent(g_moduleCore, "Event_HostageRescued",        "Core_HostageRescued");
        //EventMgr_RegisterEvent(g_moduleCore, "Event_HostageRescuedAll",     "Core_HostageRescuedAll");
    #endif
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public Core_OnEventsReady()
{
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public Core_OnConfigReload(configindex)
{
    #if defined LOG_MANAGER
        LogMgr_Print(g_moduleCore, LogType_Normal, "CoreConfigReload", "Reloaded core module's config (index %d)", configindex);
    #endif

    ConfigMgr_CacheKv(g_moduleCore, CM_CONFIGINDEX_FIRST, "CoreModule_ConfigCache");
}

/**
  * Read config values
  */
public KvCache:CoreModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
		//g_CoreConfigCache[CoreConfig_auto_save] = KvGetFloat(kv, "auto_save", 10.0);
		g_CoreConfigCache[CoreConfig_adminmenu_enable] = KvGetNum(kv, "admin_menu");
		decl String:buffer[64];
		//1mpulse
		decl String:path[128];
		KvGetString(kv, "admin_flags", path, 128, "z");
		for(new i = 0; i < 128; i++) path[i] = CharToLower(path[i]);
		fg = ReadFlagString(path);
		//
		new AdminFlag:flag;
		KvGetString(kv, "admin_flags", buffer, 2, "z");
		if(FindFlagByChar(buffer[0], flag))
			sAdminFlags = FlagToBit(flag);
		else
			sAdminFlags = ADMFLAG_ROOT;
		
		KvGetString(kv, "vip_flags", buffer, 2, "z");
		if(FindFlagByChar(buffer[0], flag))
			sVipFlags = FlagToBit(flag);
		else
			sVipFlags = ADMFLAG_ROOT;
			
		g_CoreConfigCache[CoreConfig_bot_enable] = KvGetNum(kv, "bot_enable");
		g_CoreConfigCache[CoreConfig_bot_control] = KvGetNum(kv, "bot_control");
		g_CoreConfigCache[CoreConfig_bot_maxlevel] = KvGetNum(kv, "bot_maxlevel");
		g_CoreConfigCache[CoreConfig_exp_max] = KvGetNum(kv, "exp_max");
		g_CoreConfigCache[CoreConfig_exp_start] = KvGetNum(kv, "exp_start");
		g_CoreConfigCache[CoreConfig_exp_inc] = KvGetNum(kv, "exp_inc");
		g_CoreConfigCache[CoreConfig_exp_kill] = KvGetNum(kv, "exp_kill");
		g_CoreConfigCache[CoreConfig_max_exp_kill] = KvGetNum(kv, "max_exp_kill");
		g_CoreConfigCache[CoreConfig_exp_headshot] = KvGetNum(kv, "exp_headshot");
		g_CoreConfigCache[CoreConfig_exp_damage] = KvGetFloat(kv, "exp_damage");
		g_CoreConfigCache[CoreConfig_exp_knifedmg] = KvGetFloat(kv, "exp_knifedmg");
		g_CoreConfigCache[CoreConfig_exp_objectives] = KvGetNum(kv, "exp_objectives");
		g_CoreConfigCache[CoreConfig_exp_mode] = KvGetNum(kv, "exp_mode");
		decl String:TeamExp[64], String:Breaks[2][32];
		KvGetString(kv, "exp_teamwin", TeamExp, sizeof(TeamExp), "");
		if (strlen(TeamExp) == 0)
		{
			g_CoreConfigCache[CoreConfig_exp_teamwin] = 100.0;
		}
		else
		{
			ExplodeString(TeamExp, " - ", Breaks, 2, 32);
			new Float:random = GetRandomFloat(StringToFloat(Breaks[0]), StringToFloat(Breaks[1]));
			g_CoreConfigCache[CoreConfig_exp_teamwin] = random;
		}
		decl String:PlantExp[64], String:Breaks1[2][32];
		KvGetString(kv, "exp_bombplanted", PlantExp, sizeof(PlantExp), "");
		if (strlen(PlantExp) == 0)
		{
			g_CoreConfigCache[CoreConfig_exp_bombplanted] = 100.0;
		}
		else
		{
			ExplodeString(PlantExp, " - ", Breaks1, 2, 32);
			new Float:random = GetRandomFloat(StringToFloat(Breaks1[0]), StringToFloat(Breaks1[1]));
			g_CoreConfigCache[CoreConfig_exp_bombplanted] = random;
		}
		decl String:DefusExp[64], String:Breaks2[2][32];
		KvGetString(kv, "exp_bombdefused", DefusExp, sizeof(DefusExp), "");
		if (strlen(DefusExp) == 0)
		{
			g_CoreConfigCache[CoreConfig_exp_bombdefused] = 100.0;
		}
		else
		{
			ExplodeString(DefusExp, " - ", Breaks2, 2, 32);
			new Float:random = GetRandomFloat(StringToFloat(Breaks2[0]), StringToFloat(Breaks2[1]));
			g_CoreConfigCache[CoreConfig_exp_bombdefused] = random;
		}
		decl String:ExplodeExp[64], String:Breaks3[2][32];
		KvGetString(kv, "exp_bombexploded", ExplodeExp, sizeof(ExplodeExp), "");
		if (strlen(ExplodeExp) == 0)
		{
			g_CoreConfigCache[CoreConfig_exp_bombexploded] = 100.0;
		}
		else
		{
			ExplodeString(ExplodeExp, " - ", Breaks3, 2, 32);
			new Float:random = GetRandomFloat(StringToFloat(Breaks3[0]), StringToFloat(Breaks3[1]));
			g_CoreConfigCache[CoreConfig_exp_bombexploded] = random;
		}
		g_CoreConfigCache[CoreConfig_exp_hostage] = KvGetFloat(kv, "exp_hostage");
		g_CoreConfigCache[CoreConfig_credits_inc] = KvGetNum(kv, "credits_inc");
		g_CoreConfigCache[CoreConfig_credits_start] = KvGetNum(kv, "credits_start");
		g_CoreConfigCache[CoreConfig_save_percent] = KvGetFloat(kv, "save_percent");
		g_CoreConfigCache[CoreConfig_min_players] = KvGetNum(kv, "min_players");
		g_CoreConfigCache[CoreConfig_advert] = KvGetNum(kv, "advert");
		g_CoreConfigCache[CoreConfig_announcements] = KvGetNum(kv, "announcements");
		g_CoreConfigCache[CoreConfig_freeforall] = KvGetNum(kv, "freeforall");
		g_CoreConfigCache[CoreConfig_maxspawnhp] = KvGetNum(kv, "maxspawnhp");
		g_CoreConfigCache[CoreConfig_shopmenu] = KvGetNum(kv, "shopmenu");
		g_CoreConfigCache[CoreConfig_fragmenu] = KvGetNum(kv, "fragmenu");
		g_CoreConfigCache[CoreConfig_credits_frag] = KvGetNum(kv, "creditsfrag");
		g_CoreConfigCache[CoreConfig_restmenu] = KvGetNum(kv, "restmenu");
		g_CoreConfigCache[CoreConfig_resetstatmenu] = KvGetNum(kv, "resetstatsmenu");
		g_CoreConfigCache[CoreConfig_restore_percent] = KvGetNum(kv, "restore_percent");
		g_CoreConfigCache[CoreConfig_showhud] = KvGetNum(kv, "showhud");
		KvGetString(kv, "cmd",		 g_CoreConfigCache[CoreConfig_cmd], sizeof(g_CoreConfigCache[CoreConfig_cmd]));
		KvGetString(kv, "tag",		 g_CoreConfigCache[CoreConfig_tag], sizeof(g_CoreConfigCache[CoreConfig_tag]));
		if(g_CoreConfigCache[CoreConfig_exp_start]<=0)
		{
		g_CoreConfigCache[CoreConfig_exp_start] = g_CoreConfigCache[CoreConfig_exp_inc];
		}
}
#endif

/**
 * All plugins have loaded.
 */
public Core_OnAllPluginsLoaded()
{
	InitPlayersVector();    // Create vector to hold players data

	InitOffsets();          // Find offsets and defines functions to use them

	HookCmds();             // Hook commands
	HookAdminCmds();        // Hook Admincommands

	ConnectSQL();           // Connect to SQL database
	CreateTimer(2.0, UpdateDB, TIMER_FLAG_NO_MAPCHANGE); // update SQL database
	CreateTimer(1.0, LogMap, TIMER_FLAG_NO_MAPCHANGE);
}

public Action:Info_Timer(Handle:timer, any:client)
{
	if (IsClientInGame(client) && !IsFakeClient(client))
	{
		if (IsPlayerAlive(client) && GetClientTeam(client) > 1 && Re_killtimer == 0)
		{
			decl String:Message[1024];
			new Getinvis = GetClientAlpha(client);
			new GetLvl = GetLevel(client);
			new GetCredit = GetCredits(client);
			new GetXp = GetXP(client);
			new GetReqXp = GetReqXP(client);
			new Gethealth = GetClientHealth(client);
			new GetArmor = GetClientArmor(client);
			FormatEx(Message, sizeof(Message), "%T", "HudInfo", client, Gethealth, GetArmor, Getinvis, GetLvl, GetCredit, GetXp, GetReqXp);
			if(g_CoreConfigCache[CoreConfig_showhud]==2 && GetGame()!=1)
			{
			new Handle:hBuffer = StartMessageOne("KeyHintText", client);
			BfWriteByte(hBuffer, 1);
			BfWriteString(hBuffer, Message);
			EndMessage();
			}
			else if(g_CoreConfigCache[CoreConfig_showhud]==1)
			{
				CGOPrintHintText(client, Message);
			}
		}
		else if (!IsPlayerAlive(client) && Re_killtimer == 0)
		{
			new i = 0;
			i = GetEntPropEnt(client, Prop_Send, "m_hObserverTarget");
			if (i > 0)
			{
				decl String:Message[1024];
				new Getinvis = GetClientAlpha(i);
				new GetLvl = GetLevel(i);
				new GetCredit = GetCredits(i);
				new GetXp = GetXP(i);
				new GetReqXp = GetReqXP(i);
				new Gethealth = GetClientHealth(i);
				new GetArmor = GetClientArmor(i);
				FormatEx(Message, sizeof(Message), "%T", "HudInfoSpec", client, i, Gethealth, GetArmor, Getinvis, GetLvl, GetCredit, GetXp, GetReqXp);
				if(g_CoreConfigCache[CoreConfig_showhud]==2)
				{
					new Handle:hBuffer = StartMessageOne("KeyHintText", client);
					BfWriteByte(hBuffer, 1);
					BfWriteString(hBuffer, Message);
					EndMessage();
				}
				else if(g_CoreConfigCache[CoreConfig_showhud]==1)
				{
					CGOPrintHintText(client, Message);
				}
			}
		}
		else
		{
			return Plugin_Stop;
		}
	}
	else
	{
		return Plugin_Stop;
	}
	return Plugin_Continue;
}

/**
 * Plugin is ending.
 */
public Core_OnPluginEnd()
{
    new client,hurtxp;
    new size=GetArraySize(g_hVecPlayers);

    for(new x=size-1;x>=0;x--)
    {
        client=GetArrayCell(GetArrayCell(g_hVecPlayers,x),VECTOR_PLAYERS_CLIENT);
        if(IsValidPlayer(client))
        {
            // save clients hurt xp
            hurtxp=GetHurtXP(client);
            if(hurtxp>0)
            {
                SetXP(client,hurtxp,true);
                SetHurtXP(client,0);
            }
            SavePlayerData(client);
            RemoveFromArray(g_hVecPlayers,x);
        }
    }
    pluginisending = true;
}

/**
 * The module that hooked this event callback has been disabled.
 */
public Core_OnMyModuleDisable()
{
    if(pluginisending==false)
        LogMgr_Print(g_moduleCore, LogType_Fatal_Plugin, "OnMyModuleDisable", "PLUGIN STOPPED - Reason: core module got disabled - PLUGIN STOPPED");
}

/**
 * The map has started.
 */
public Core_OnMapStart()
{
    #if defined CONFIG_MANAGER
        ConfigMgr_CacheKv(g_moduleCore, CM_CONFIGINDEX_FIRST, "CoreModule_ConfigCache");
    #endif
	
	//CreateTimer(g_CoreConfigCache[CoreConfig_auto_save], AutoSave_Timer, _, TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
	
    if(g_hSQL!=INVALID_HANDLE)
        for(new client=1;client<=MaxClients;client++)
            if(IsClientAuthorized(client))
                if(GetPlayerVectorPosition(client)==-1)
                {
                    InitPlayer(client);
                    LoadPlayerData(client);
                    SetHurtXP(client,0);
                }
}

/**
 * All convars are set, cvar-dependent code should use this.
 */
public Core_OnConfigsExecuted()
{
    // This should fix the a2s_rules bug on linux srcds
    #if defined PROJECT_PUBLIC_CVAR
        decl String:publiccvarvalue[32];
        strcopy(publiccvarvalue, sizeof(publiccvarvalue), PROJECT_VERSION);
        SetConVarString(FindConVar(PROJECT_PUBLIC_CVAR), publiccvarvalue);
    #endif

    PrecacheModels();
}

/**
 * Client is disconnecting from the server.
 *
 * @param client    The client index.
 */

public Core_OnClientDisconnect(client)
{
    // save clients hurt xp
    new hurtxp=GetHurtXP(client);
    if(hurtxp>0)
    {
        SetXP(client,hurtxp,true);
        SetHurtXP(client,0);
    }

    new clientpos=SavePlayerData(client);
    if(clientpos!=-1)
        RemoveFromArray(g_hVecPlayers,clientpos);
}
/*
public OnDisconnect(Handle:event, const String:name[], bool:dontBroadcast)
{
	new client=GetClientOfUserId(GetEventInt(event, "userid"));
	if(IsValidPlayer(client))
	{
		decl String:reason[192];
		GetEventString(event, "reason", reason, sizeof(reason));
		if(StrContains(reason, "timed out", false) < 0)
		{
			new hurtxp=GetHurtXP(client);
			if(hurtxp>0)
			{
				SetXP(client,hurtxp,true);
				SetHurtXP(client,0);
			}
				
			new clientpos=SavePlayerData(client);
			if(clientpos!=-1) RemoveFromArray(g_hVecPlayers,clientpos);
		}
	}
}

public Action:AutoSave_Timer(Handle:timer)
{
	SaveAllPlayers();
	return Plugin_Continue;
}

SaveAllPlayers()
{
	for(new i = 1; i <= MaxClients; i++)
	{
		if(IsValidPlayer(i))
		{
			new hurtxp=GetHurtXP(i);
			if(hurtxp>0)
			{
				SetXP(i,hurtxp,true);
				SetHurtXP(i,0);
			}	
			SavePlayerData(i);
		}
	}
}*/

/**
 * Round has started.
 */
public Core_RoundStart()
{
    Re_killtimer = 0;
    if(g_hSQL!=INVALID_HANDLE)
        for(new client=1;client<=MaxClients;client++)
            if(IsClientAuthorized(client))
                if(GetPlayerVectorPosition(client)==-1)
                {
                    InitPlayer(client);
                    LoadPlayerData(client);
                    SetHurtXP(client,0);
                }
}

/**
 * Round has ended.
 *
 * @param winner    The index of the winning team.
 */
public Core_RoundEnd(client, winner)
{
    Re_killtimer = 1;
    new hurtxp;

    // give xp to winning team
    if(g_CoreConfigCache[CoreConfig_exp_objectives])
        TeamWinXP(winner);

    // save clients hurt xp
    for(new i=1;i<=MaxClients;i++)
        if(IsClientAuthorized(i))
        {
            hurtxp=GetHurtXP(i);
            if(hurtxp>0)
            {
                SetXP(i,hurtxp,true);
                SetHurtXP(i,0);
            }
        }

    // save all players data
    if(g_hSQL!=INVALID_HANDLE)
        for(new i=1;i<=MaxClients;i++)
            if(IsClientAuthorized(i))
                if(GetPlayerVectorPosition(i)!=-1)
                    SavePlayerData(i);		
}

/**
 * Client has joined a team.
 *
 * @param client        The client index.
 * @param team          The client's new team.
 * @param oldteam       The team the client is switching from.
 * @param disconnect    True if the client is disconnecting, false if switching to a team.
 */
public Core_PlayerTeam(client, team, oldteam, bool:disconnect)
{
    if(!disconnect)
    {
        if(g_hSQL!=INVALID_HANDLE)
            if(IsClientAuthorized(client))
                if(GetPlayerVectorPosition(client)==-1)
                {
                    InitPlayer(client);
                    LoadPlayerData(client);
                    SetHurtXP(client,0);
                }

        if(g_CoreConfigCache[CoreConfig_advert])
            TransMgr_PrintText(client, MsgFormat_None, MsgType_Chat, INVALID_MODULE, false, "Advert", PROJECT_VERSION);
    }
}

/**
 * Client has spawned.
 *
 * @param client    The client index.
 *
 */
public Core_PlayerSpawn(client)
{
    if(IsValidPlayer(client))
    {
        SetSpawnHealth(client, GetClientHealth(client));
        SetModifiedHealth(client, GetClientHealth(client));
        SetSpawnSpeed(client, GetEntPropFloat(client, Prop_Data, "m_flLaggedMovementValue"));
        SetModifiedSpeed(client, GetEntPropFloat(client, Prop_Data, "m_flLaggedMovementValue"));
        if (!DisableStats[client])
        {
			CreateTimer(1.0, Info_Timer, client, TIMER_REPEAT);
        }
    }
}

/**
 * Client has been damaged.
 *
 * @param victim        The index of the hurt client.
 * @param attacker      The index of the attacking client.
 * @param health        How much health the client has after the damage.
 * @param armor         How much armor the client has after the damage.
 * @param weapon        The weapon classname used to hurt the victim. (No weapon_ prefix)
 * @param dmg_health    The amount of health the victim lost.
 * @param dmg_armor     The amount of armor the victim lost.
 * @param hitgroup      The hitgroup index of the victim that was damaged.
 */

public Core_PlayerHurt(victim, attacker, health, armor, const String:weapon[], dmg_health, dmg_armor, hitgroup)
{
    if(AreValidPlayers(victim, attacker))
        if(AreValidTeams(victim,attacker,g_CoreConfigCache[CoreConfig_freeforall],0))
        {
            new amount;
            if(!StrEqual(weapon,"knife",false))
                amount = RoundToNearest(dmg_health*g_CoreConfigCache[CoreConfig_exp_damage]);
            else
                amount = RoundToNearest(dmg_health*g_CoreConfigCache[CoreConfig_exp_knifedmg]);

            if(amount>0)
				SetHurtXP(attacker,amount);
        }
}

/**
 * Client has been killed.
 *
 * @param victim    The index of the killed client.
 * @param attacker  The killer of the victim.
 * @param weapon    The weapon classname used to kill the victim. (No weapon_ prefix)
 * @param headshot  True if the death was by headshot, false if not.
 */
public Core_PlayerDeath(victim, attacker, const String:weapon[], bool:headshot)
{
    if(IsValidPlayer(victim))
	{
		if(IsValidPlayer(attacker))
		{
			if(AreValidTeams(victim,attacker,g_CoreConfigCache[CoreConfig_freeforall],0))
			{
            new amount=RoundToNearest(PlayerLevelRatio(victim,attacker)*g_CoreConfigCache[CoreConfig_exp_kill]);
            if(headshot)
                amount+=g_CoreConfigCache[CoreConfig_exp_headshot];
				amount+=GetHurtXP(attacker);
            
            if(amount>g_CoreConfigCache[CoreConfig_max_exp_kill] && g_CoreConfigCache[CoreConfig_max_exp_kill] != 0)
                amount=g_CoreConfigCache[CoreConfig_max_exp_kill];
            
            if(amount>0)
            {
				SetXP(attacker,amount,g_CoreConfigCache[CoreConfig_announcements]);
				SetHurtXP(attacker,0);
            }

            // save victims hurt xp
            new hurtxp=GetHurtXP(victim);
            if(hurtxp>0)
            {
				SetXP(victim,hurtxp,true);
				SetHurtXP(victim,0);
            }
        }
	}
	}
}

/**
 * Client has planted the bomb.
 *
 * @param planter   The client who has planted the bomb
 */
public Core_BombPlanted(planter)
{
    if(g_CoreConfigCache[CoreConfig_exp_objectives])
	{
		if(g_CoreConfigCache[CoreConfig_exp_mode]==0)
		{
			SetXP(planter,RoundToNearest(TeamRatio(CSS_TEAM_T)*GetClientFrags(planter)*g_CoreConfigCache[CoreConfig_exp_bombplanted]),g_CoreConfigCache[CoreConfig_announcements]);
		}
		else
		{
			SetXP(planter,RoundToNearest(g_CoreConfigCache[CoreConfig_exp_bombplanted]),g_CoreConfigCache[CoreConfig_announcements]);
		}
	}
}

/**
 * Client has defused the bomb.
 *
 * @param defuser   The client who has defused the bomb
 */
public Core_BombDefused(defuser)
{
    if(g_CoreConfigCache[CoreConfig_exp_objectives])
    {
		if(g_CoreConfigCache[CoreConfig_exp_mode]==0)
			SetXP(defuser,RoundToNearest(TeamRatio(CSS_TEAM_CT)*GetClientFrags(defuser)*g_CoreConfigCache[CoreConfig_exp_bombdefused]),g_CoreConfigCache[CoreConfig_announcements]);
		else
			SetXP(defuser,RoundToNearest(g_CoreConfigCache[CoreConfig_exp_bombdefused]),g_CoreConfigCache[CoreConfig_announcements]);
    }
}

/**
 * The bomb has exploded.
 *
 * @param planter   The client who has planted the bomb
 */
public Core_BombExploded(planter)
{
    if(g_CoreConfigCache[CoreConfig_exp_objectives])
    {
		if(g_CoreConfigCache[CoreConfig_exp_mode]==0)
			SetXP(planter,RoundToNearest(TeamRatio(CSS_TEAM_T)*GetClientFrags(planter)*g_CoreConfigCache[CoreConfig_exp_bombexploded]),g_CoreConfigCache[CoreConfig_announcements]);
		else
			SetXP(planter,RoundToNearest(g_CoreConfigCache[CoreConfig_exp_bombexploded]),g_CoreConfigCache[CoreConfig_announcements]);
    }
}

/**
 * Client has rescued a hostage.
 *
 * @param rescuer   The client who has rescued a hostage
 */
 
public Core_HostageRescued(rescuer)
{
    if(g_CoreConfigCache[CoreConfig_exp_objectives])
	{
		if(g_CoreConfigCache[CoreConfig_exp_mode]==0)
			SetXP(rescuer,RoundToNearest(TeamRatio(CSS_TEAM_CT)*GetClientFrags(rescuer)*g_CoreConfigCache[CoreConfig_exp_hostage]),g_CoreConfigCache[CoreConfig_announcements]);
		else
			SetXP(rescuer,RoundToNearest(g_CoreConfigCache[CoreConfig_exp_hostage]),g_CoreConfigCache[CoreConfig_announcements]);
	}
}


/**
 * All hostages got rescued.
 */
public Core_HostageRescuedAll()
{
}

public Action:UpdateDB(Handle:timer)
{
    if(g_hSQL!=INVALID_HANDLE)
        CheckDBUpdate();

    return Plugin_Stop;
}

public Action:LogMap(Handle:timer)
{
    // Called 1 second after OnPluginStart since srcds does not log the first map loaded. Idea from Stormtrooper's "mapfix.sp" for psychostats
    LogMapLoad();
    
    return Plugin_Stop;
}
