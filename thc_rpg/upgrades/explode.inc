/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          explode.inc
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
new Module:g_moduleexplode;

new g_explodeUpgradeConfigCache[UpgradeConfigs];
//new HaloSprite;
new BeamSprite;
new g_iExplosionModel;

/**
 * Register this module.
 */
explode_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "explode");
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "explode");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "explode");

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
    g_moduleexplode = ModuleMgr_Register(moduledata);

    // Now register the events we're going to use.
    #if defined EVENT_MANAGER
        EventMgr_RegisterEvent(g_moduleexplode, "Event_OnEventsRegister", "explode_OnEventsRegister");
    #endif
    
    // Register config file(s) that this module will use.
    #if defined CONFIG_MANAGER
        ConfigMgr_Register(g_moduleexplode, "explode_OnConfigReload", "configs/thc_rpg/explode.txt");
    #endif
}

/**
 * Register all events here.
 */
public explode_OnEventsRegister()
{
    // Register all the events needed for this module.
    EventMgr_RegisterEvent(g_moduleexplode, "Event_OnMapStart",                   "explode_OnMapStart");
	#if defined PROJECT_GAME_CSS
		EventMgr_RegisterEvent(g_moduleexplode, "Event_PlayerDeath",              "explode_PlayerDeath");
	#endif
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public explode_OnEventsReady()
{
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public explode_OnConfigReload(configindex)
{
    #if defined LOG_MANAGER
        LogMgr_Print(g_moduleexplode, LogType_Normal, "explodeConfigReload", "Reloaded speed module's config (index %d)", configindex);
    #endif

    ConfigMgr_CacheKv(g_moduleexplode, CM_CONFIGINDEX_FIRST, "explodeModule_ConfigCache");
}

/**
 * Read config values
 */
public KvCache:explodeModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
    // Read Upgrade config
    if(StrEqual(sectionname, "explode", false))
    {
        g_explodeUpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
        g_explodeUpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
        g_explodeUpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
        g_explodeUpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
        g_explodeUpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");
        g_explodeUpgradeConfigCache[UpgradeConfig_Percent] = KvGetFloat(kv, "percent");
		g_explodeUpgradeConfigCache[UpgradeConfig_Radius] = KvGetFloat(kv, "radius");
		g_explodeUpgradeConfigCache[UpgradeConfig_Damage] = KvGetFloat(kv, "dmg");
		KvGetString(kv, "sound", g_explodeUpgradeConfigCache[UpgradeConfig_Sound], sizeof(g_explodeUpgradeConfigCache[UpgradeConfig_Sound]));
        if(g_explodeUpgradeConfigCache[UpgradeConfig_Disable]==1)
            ModuleMgr_Disable(g_moduleexplode);

        ModuleMgr_WriteCell(g_moduleexplode, ModuleData_TeamLock, g_explodeUpgradeConfigCache[UpgradeConfig_TeamLock]);
        ModuleMgr_WriteCell(g_moduleexplode, ModuleData_MaxLevel, g_explodeUpgradeConfigCache[UpgradeConfig_MaxLevel]);
        ModuleMgr_WriteCell(g_moduleexplode, ModuleData_Cost, g_explodeUpgradeConfigCache[UpgradeConfig_Cost]);
        ModuleMgr_WriteCell(g_moduleexplode, ModuleData_iCost, g_explodeUpgradeConfigCache[UpgradeConfig_iCost]);
    }
    else
        ModuleMgr_Disable(g_moduleexplode);
}
#endif

/**
 * The map has started.
 */
public explode_OnMapStart()
{
    #if defined CONFIG_MANAGER
        ConfigMgr_CacheKv(g_moduleexplode, CM_CONFIGINDEX_FIRST, "explodeModule_ConfigCache");
    #endif
	g_iExplosionModel = PrecacheModel("materials/sprites/sprite_fire01.vmt");
	BeamSprite = PrecacheModel("materials/sprites/lgtning.vmt");
	//HaloSprite = PrecacheModel("materials/sprites/halo01.vmt");
	//PrecacheSound(g_explodeUpgradeConfigCache[UpgradeConfig_Sound]);
}

/**
 * Client has been damaged.
 * 
 * @param victim        The index of the hurt client.
 * @param attacker      The index of the attacking client.
 * @param weapon        The weapon classname used to hurt the victim. (No weapon_ prefix)
 */
public explode_PlayerDeath(victim, attacker, const String:weapon[], bool:headshot)
{
	if(AreValidPlayers(victim, attacker))
	{
		new level = GetPlayerUpgradeLevel(victim, g_moduleexplode);
		if(level > 0 && GetRandomFloat(0.0, 1.0) <= g_explodeUpgradeConfigCache[UpgradeConfig_Percent]*level)
		{
			Explode(victim,attacker);
		}
	}
}

Explode(victim,attacker)
{
        decl Float:Vec1[3], Float:Vec2[3];
	GetClientAbsOrigin(victim, Vec1);
	//new Float:level = GetPlayerUpgradeLevel(victim, g_moduleexplode) * 100.0;
	new Float:level = GetPlayerUpgradeLevel(victim, g_moduleexplode) * g_explodeUpgradeConfigCache[UpgradeConfig_Radius];
	//new Float:dmg = GetPlayerUpgradeLevel(victim, g_moduleexplode) * 10.0;
	new Float:dmg = GetPlayerUpgradeLevel(victim, g_moduleexplode) * g_explodeUpgradeConfigCache[UpgradeConfig_Damage];
	//new dmg2 = GetPlayerUpgradeLevel(victim, g_moduleexplode) * 10;
	new dmg2 = GetPlayerUpgradeLevel(victim, g_moduleexplode) * RoundToCeil(g_explodeUpgradeConfigCache[UpgradeConfig_Damage]);
        for (new i = 1; i <= MaxClients; i++)
        {
		if (IsClientInGame(i) && IsPlayerAlive(i))
		{
			if (GetClientTeam(victim) != GetClientTeam(i))
 			{
				GetClientAbsOrigin(i, Vec2);
 				if (GetVectorDistance(Vec1, Vec2) <= level)
				{
					if(!StrEqual(g_vampireUpgradeConfigCache[UpgradeConfig_Sound], "off", false))
                    {
						EmitSoundToAll(g_vampireUpgradeConfigCache[UpgradeConfig_Sound], i, _, _, _, 1.0);
					}
					if (dmg >= float(GetClientHealth(i)))
					{
						RPGDamage(i, RoundToCeil(dmg), attacker, 0, "");
					}
					else
					{
						SetEntityHealth(i, GetClientHealth(i) - dmg2);                           					
					}
					ExplodeEffect(victim);
				}
			}
		}
	}
}
 

ExplodeEffect(victim)
{
		new Float:radius = GetPlayerUpgradeLevel(victim, g_moduleexplode) * g_explodeUpgradeConfigCache[UpgradeConfig_Radius];
		decl Float:client_location[3];
		GetClientAbsOrigin(victim, client_location);
		//new Float:radiusScale = radius / 10.0;
		TE_SetupExplosion( client_location, g_iExplosionModel, 1.0, 12, 0, RoundToCeil(radius), RoundToCeil(radius)+50);
		TE_SendToAll();
		
		client_location[2] -= 40.0;
		
		//new beamcolor[4] = { 0, 0, 102, 255 };
		//TE_SetupBeamRingPoint( client_location, 50.0, radius, BeamSprite, HaloSprite, 0, 15, 2.0, 10.0, 0.5, beamcolor, 1, 0 );
		TE_SetupBeamRingPoint(client_location, 50.0, radius, BeamSprite, 0, 0, 0, 1.0, 30.0, 10.0, {155, 155, 155, 155}, 5, 0);
		TE_SendToAll();
}