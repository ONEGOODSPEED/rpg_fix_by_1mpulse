/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          parasite.inc
 *  Type:          Upgrade
 *  Description:   Gives you the ability to move faster when shooting
 *  Author:        SenatoR
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
new Module:g_moduleparasite;
new bool:bDucking[MAXPLAYERS];
new g_parasiteUpgradeConfigCache[UpgradeConfigs];
new SteamSprite;
/**
 * Register this module.
 */
parasite_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "parasite");
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "parasite");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "parasite");

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

    // Send this array of data to the module manager.
    g_moduleparasite = ModuleMgr_Register(moduledata);

    // Now register the events we're going to use.
    #if defined EVENT_MANAGER
        EventMgr_RegisterEvent(g_moduleparasite, "Event_OnEventsRegister", "parasite_OnEventsRegister");
    #endif
    
    // Register config file(s) that this module will use.
    #if defined CONFIG_MANAGER
        ConfigMgr_Register(g_moduleparasite, "parasite_OnConfigReload", "configs/thc_rpg/parasite.txt");
    #endif
}

/**
 * Register all events here.
 */
public parasite_OnEventsRegister()
{
    // Register all the events needed for this module.
    EventMgr_RegisterEvent(g_moduleparasite, "Event_OnMapStart",                   "parasite_OnMapStart");
	#if defined PROJECT_GAME_CSS
		EventMgr_RegisterEvent(g_moduleparasite, "Event_PlayerDeath",              "parasite_PlayerDeath");
	#endif
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public parasite_OnEventsReady()
{
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public parasite_OnConfigReload(configindex)
{
    #if defined LOG_MANAGER
        LogMgr_Print(g_moduleparasite, LogType_Normal, "parasiteConfigReload", "Reloaded speed module's config (index %d)", configindex);
    #endif

    ConfigMgr_CacheKv(g_moduleparasite, CM_CONFIGINDEX_FIRST, "parasiteModule_ConfigCache");
}

/**
 * Read config values
 */
public KvCache:parasiteModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
    // Read Upgrade config
    if(StrEqual(sectionname, "parasite", false))
    {
        g_parasiteUpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
        g_parasiteUpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
        g_parasiteUpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
        g_parasiteUpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
        g_parasiteUpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");
        g_parasiteUpgradeConfigCache[UpgradeConfig_Percent] = KvGetFloat(kv, "chance");
		g_parasiteUpgradeConfigCache[UpgradeConfig_Effects] = KvGetNum(kv, "effects");

        if(g_parasiteUpgradeConfigCache[UpgradeConfig_Disable]==1)
            ModuleMgr_Disable(g_moduleparasite);

        ModuleMgr_WriteCell(g_moduleparasite, ModuleData_TeamLock, g_parasiteUpgradeConfigCache[UpgradeConfig_TeamLock]);
        ModuleMgr_WriteCell(g_moduleparasite, ModuleData_MaxLevel, g_parasiteUpgradeConfigCache[UpgradeConfig_MaxLevel]);
        ModuleMgr_WriteCell(g_moduleparasite, ModuleData_Cost, g_parasiteUpgradeConfigCache[UpgradeConfig_Cost]);
        ModuleMgr_WriteCell(g_moduleparasite, ModuleData_iCost, g_parasiteUpgradeConfigCache[UpgradeConfig_iCost]);
    }
    else
        ModuleMgr_Disable(g_moduleparasite);
}
#endif

/**
 * The map has started.
 */
public parasite_OnMapStart()
{
    #if defined CONFIG_MANAGER
        ConfigMgr_CacheKv(g_moduleparasite, CM_CONFIGINDEX_FIRST, "parasiteModule_ConfigCache");
		SteamSprite = PrecacheModel( "sprites/steam1.vmt" );
    #endif
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
 
 public Action:OnPlayerRunCmd(client, &buttons, &impulse, Float:vel[3], Float:angles[3], &weapon)
{
    
    bDucking[client]=(buttons & IN_DUCK)?true:false;
    return Plugin_Continue;
}

public parasite_PlayerDeath(victim, attacker, const String:weapon[], bool:headshot)
{
	if(AreValidPlayers(victim, attacker))
	{
		new level = GetPlayerUpgradeLevel(attacker, g_moduleparasite);
		if(level > 0)
		{
			new Float:chance = g_parasiteUpgradeConfigCache[UpgradeConfig_Percent] * level;
			if (chance >= GetRandomFloat(0.0, 1.0))
			if(bDucking[attacker])
			{
					new Float:GlobPos[3];
					GetClientAbsOrigin(victim, GlobPos);
					if(g_parasiteUpgradeConfigCache[UpgradeConfig_Effects]==1)
					{
						new Float:EffectPos[3];
						GetClientAbsOrigin(attacker, EffectPos);
						EffectPos[2] += 50;
						TE_SetupGlowSprite(EffectPos, SteamSprite, 1.0, 2.5, 130 );
						TE_SendToAll();
					}
					TeleportEntity(attacker, GlobPos, NULL_VECTOR, NULL_VECTOR);
					TransMgr_PrintText(attacker, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "You are teleported to a place of death of",victim);
			}
		}
	}
}