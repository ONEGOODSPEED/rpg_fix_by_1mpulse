/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          inhumanity.inc
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
new Module:g_moduleinhumanity;

new g_inhumanityUpgradeConfigCache[UpgradeConfigs];

/**
 * Register this module.
 */
inhumanity_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "inhumanity");
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "inhumanity");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "inhumanity");

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
    g_moduleinhumanity = ModuleMgr_Register(moduledata);

    // Now register the events we're going to use.
    #if defined EVENT_MANAGER
        EventMgr_RegisterEvent(g_moduleinhumanity, "Event_OnEventsRegister", "inhumanity_OnEventsRegister");
    #endif
    
    // Register config file(s) that this module will use.
    #if defined CONFIG_MANAGER
        ConfigMgr_Register(g_moduleinhumanity, "inhumanity_OnConfigReload", "configs/thc_rpg/inhumanity.txt");
    #endif
}

/**
 * Register all events here.
 */
public inhumanity_OnEventsRegister()
{
    // Register all the events needed for this module.
    EventMgr_RegisterEvent(g_moduleinhumanity, "Event_OnMapStart",                   "inhumanity_OnMapStart");
	#if defined PROJECT_GAME_CSS
		EventMgr_RegisterEvent(g_moduleinhumanity, "Event_PlayerDeath",              "inhumanity_PlayerDeath");
	#endif
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public inhumanity_OnEventsReady()
{
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public inhumanity_OnConfigReload(configindex)
{
    #if defined LOG_MANAGER
        LogMgr_Print(g_moduleinhumanity, LogType_Normal, "inhumanityConfigReload", "Reloaded speed module's config (index %d)", configindex);
    #endif

    ConfigMgr_CacheKv(g_moduleinhumanity, CM_CONFIGINDEX_FIRST, "inhumanityModule_ConfigCache");
}

/**
 * Read config values
 */
public KvCache:inhumanityModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
    // Read Upgrade config
    if(StrEqual(sectionname, "inhumanity", false))
    {
        g_inhumanityUpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
        g_inhumanityUpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
        g_inhumanityUpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
        g_inhumanityUpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
        g_inhumanityUpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");
        g_inhumanityUpgradeConfigCache[UpgradeConfig_Percent] = KvGetFloat(kv, "chance");
        g_inhumanityUpgradeConfigCache[UpgradeConfig_Radius] = KvGetFloat(kv, "radius");
        g_inhumanityUpgradeConfigCache[UpgradeConfig_Amount] = KvGetFloat(kv, "amount");
        if(g_inhumanityUpgradeConfigCache[UpgradeConfig_Disable]==1)
            ModuleMgr_Disable(g_moduleinhumanity);

        ModuleMgr_WriteCell(g_moduleinhumanity, ModuleData_TeamLock, g_inhumanityUpgradeConfigCache[UpgradeConfig_TeamLock]);
        ModuleMgr_WriteCell(g_moduleinhumanity, ModuleData_MaxLevel, g_inhumanityUpgradeConfigCache[UpgradeConfig_MaxLevel]);
        ModuleMgr_WriteCell(g_moduleinhumanity, ModuleData_Cost, g_inhumanityUpgradeConfigCache[UpgradeConfig_Cost]);
        ModuleMgr_WriteCell(g_moduleinhumanity, ModuleData_iCost, g_inhumanityUpgradeConfigCache[UpgradeConfig_iCost]);
    }
    else
        ModuleMgr_Disable(g_moduleinhumanity);
}
#endif

/**
 * The map has started.
 */
public inhumanity_OnMapStart()
{
    #if defined CONFIG_MANAGER
        ConfigMgr_CacheKv(g_moduleinhumanity, CM_CONFIGINDEX_FIRST, "inhumanityModule_ConfigCache");
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
 

public inhumanity_PlayerDeath(victim, attacker, const String:weapon[], bool:headshot)
{
    if(victim>0)
	{
		new Float:deathvec[3],Float:gainhpvec[3];
		for (new i = 1; i <= MaxClients; i++)
		{
			if(IsValidPlayer(i))
			{
				new level = GetPlayerUpgradeLevel(i, g_moduleinhumanity);
				if(level > 0)
				{
					new Float:chance = g_inhumanityUpgradeConfigCache[UpgradeConfig_Percent] * level;
					new Float:InhumanRange= g_inhumanityUpgradeConfigCache[UpgradeConfig_Radius] * level;
					new amount = RoundToNearest(g_inhumanityUpgradeConfigCache[UpgradeConfig_Amount]*level);
					if (chance >= GetRandomFloat(0.0, 1.0))
					{
						GetClientAbsOrigin(victim,deathvec);
						GetClientAbsOrigin(i,gainhpvec);
						if(GetVectorDistance(deathvec,gainhpvec)<=InhumanRange)
						{
							if(amount>GetModifiedHealth(i))
							{
								amount=GetModifiedHealth(i);
							}
							SetEntityHealth(i,amount);
							TransMgr_PrintText(i, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "You got for the death of",amount,i);
						}
					}
				}
			}
		}
	}
}