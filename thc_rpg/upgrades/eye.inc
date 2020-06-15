/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          eye.inc
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
new Module:g_moduleeye;
new g_eyeUpgradeConfigCache[UpgradeConfigs];
new bool:WeaponZoomed[MAXPLAYERS+1];
new g_iScope[MAXPLAYERS+1];
new ammo[MAXPLAYERS+1];
new HaloSpriteEye,BeamSpriteEye,GlowSpriteEye,GlowSpriteEye2;

/**
 * Register this module.
 */
eye_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "eye");
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "eye");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "eye");

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
    g_moduleeye = ModuleMgr_Register(moduledata);

    // Now register the events we're going to use.
    #if defined EVENT_MANAGER
        EventMgr_RegisterEvent(g_moduleeye, "Event_OnEventsRegister", "eye_OnEventsRegister");
    #endif
    
    // Register config file(s) that this module will use.
    #if defined CONFIG_MANAGER
        ConfigMgr_Register(g_moduleeye, "eye_OnConfigReload", "configs/thc_rpg/eye.txt");
    #endif
}

/**
 * Register all events here.
 */
public eye_OnEventsRegister()
{
    // Register all the events needed for this module.
    EventMgr_RegisterEvent(g_moduleeye, "Event_OnMapStart",                   "eye_OnMapStart");
	#if defined PROJECT_GAME_CSS
		EventMgr_RegisterEvent(g_moduleeye, "Event_WeaponFire",           "eye_WeaponFire");
		EventMgr_RegisterEvent(g_moduleeye, "Event_WeaponZoom",           "eye_WeaponZoom");
		EventMgr_RegisterEvent(g_moduleeye, "Event_WeaponReload",           "eye_WeaponReload");
	#endif
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public eye_OnEventsReady()
{
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public eye_OnConfigReload(configindex)
{
    #if defined LOG_MANAGER
        LogMgr_Print(g_moduleeye, LogType_Normal, "eyeConfigReload", "Reloaded speed module's config (index %d)", configindex);
    #endif

    ConfigMgr_CacheKv(g_moduleeye, CM_CONFIGINDEX_FIRST, "eyeModule_ConfigCache");
}

/**
 * Read config values
 */
public KvCache:eyeModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
    // Read Upgrade config
    if(StrEqual(sectionname, "eye", false))
    {
        g_eyeUpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
        g_eyeUpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
        g_eyeUpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
        g_eyeUpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
        g_eyeUpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");
        g_eyeUpgradeConfigCache[UpgradeConfig_Sense] = KvGetFloat(kv, "sense");

        if(g_eyeUpgradeConfigCache[UpgradeConfig_Disable]==1)
            ModuleMgr_Disable(g_moduleeye);

        ModuleMgr_WriteCell(g_moduleeye, ModuleData_TeamLock, g_eyeUpgradeConfigCache[UpgradeConfig_TeamLock]);
        ModuleMgr_WriteCell(g_moduleeye, ModuleData_MaxLevel, g_eyeUpgradeConfigCache[UpgradeConfig_MaxLevel]);
        ModuleMgr_WriteCell(g_moduleeye, ModuleData_Cost, g_eyeUpgradeConfigCache[UpgradeConfig_Cost]);
        ModuleMgr_WriteCell(g_moduleeye, ModuleData_iCost, g_eyeUpgradeConfigCache[UpgradeConfig_iCost]);
    }
    else
        ModuleMgr_Disable(g_moduleeye);
}
#endif

/**
 * The map has started.
 */
public eye_OnMapStart()
{
	GlowSpriteEye=PrecacheModel("materials/effects/redflare.vmt");
	GlowSpriteEye2=PrecacheModel("materials/effects/fluttercore.vmt");
	BeamSpriteEye=PrecacheModel("materials/sprites/lgtning.vmt");
	HaloSpriteEye=PrecacheModel("materials/sprites/halo01.vmt");
	CreateTimer( 0.1, CalcSense, _, TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
    #if defined CONFIG_MANAGER
        ConfigMgr_CacheKv(g_moduleeye, CM_CONFIGINDEX_FIRST, "eyeModule_ConfigCache");
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

public Action:CalcSense( Handle:timer, any:userid )
{
	for(new i = 1; i <= MaxClients; i++)
	{
		if(IsValidPlayer(i))
		{
			new level = GetPlayerUpgradeLevel(i, g_moduleeye);
			if(level > 0)
			{
				Sense(i);
			}		
		}
	}
	
}

public Sense(client)
{
		new level = GetPlayerUpgradeLevel(client, g_moduleeye);
		if(level > 0)
		{
		new Float:ElfPos[3];
		new Float:VictimPos[3];
		
		GetClientAbsOrigin( client, ElfPos );
		
		ElfPos[2] += 50.0;
		
		for( new i = 1; i <= MaxClients; i++ )
		{
			if(IsValidPlayer( i, true ) && GetClientTeam( i ))
			{
				GetClientAbsOrigin( i, VictimPos );
				VictimPos[2] += 50.0;
				new Float:sense = g_eyeUpgradeConfigCache[UpgradeConfig_Sense]*level;
				if(GetVectorDistance( ElfPos, VictimPos ) <= sense)
				{
					decl String:weapon[64];
					GetClientWeapon(client, weapon, sizeof(weapon));
					if(StrEqual(weapon, "weapon_awp")||StrEqual(weapon, "weapon_scout")||StrEqual(weapon, "weapon_ssg08"))
					{
						new VictimTeam = GetClientTeam( i );
						if(WeaponZoomed[client] == true)
						{
							if(VictimTeam == 2) // TT
							{
								TE_SetupGlowSprite(VictimPos,GlowSpriteEye,0.1,0.6,80);
								TE_SendToClient(client);
								TE_SetupBeamPoints(ElfPos, VictimPos, BeamSpriteEye, HaloSpriteEye, 0, 8, 0.1, 1.0, 10.0, 10, 10.0, {255,0,0,155}, 70); // czerwony
								TE_SendToClient(client);
							}
							else if(VictimTeam == 3) // CT
							{
								TE_SetupGlowSprite(VictimPos,GlowSpriteEye2,0.1,0.1,150);
								TE_SendToClient(client);
								TE_SetupBeamPoints(ElfPos, VictimPos, BeamSpriteEye, HaloSpriteEye, 0, 8, 0.1, 1.0, 10.0, 10, 10.0, {30,144,255,155}, 70); // niebieski
								TE_SendToClient(client);
							}	
						}
					}
					else
					{
						g_iScope[client] = 0;
						WeaponZoomed[client] = false;						
					}						
				}
			}
			}
		}
}



/* *************************************** OnPlayerZoom *************************************** */
public eye_WeaponZoom(client, const String:weapon[])
{
	g_iScope[client]++;
	if(g_iScope[client] >= 3)
	{
		g_iScope[client] = 0;
	}
	if(StrEqual(weapon, "weapon_awp")||StrEqual(weapon, "weapon_scout"))
	{
		if(g_iScope[client] == 2)
		{
			WeaponZoomed[client] = true;
		}
		else
		{
			WeaponZoomed[client] = false;
		}
	}
	else
	{
		g_iScope[client] = 0;
		WeaponZoomed[client] = false;
	}
}

/* *************************************** OnPlayerReload *************************************** */
public eye_WeaponReload(client, const String:weapon[])
{
	g_iScope[client] = 0;
	WeaponZoomed[client] = false;
	ammo[client] = 10;
}

/* *************************************** OnPlayerFire *************************************** */
public eye_WeaponFire(client, const String:weapon[])
{
	if(StrEqual(weapon, "weapon_awp")||StrEqual(weapon, "weapon_scout"))
	{
		ammo[client]--;
		if(ammo[client] == 0)
		{
			g_iScope[client] = 0;
			WeaponZoomed[client] = false;
			ammo[client] = 10;
		}
	}
}
