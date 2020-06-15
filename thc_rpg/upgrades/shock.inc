/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:		  shock.inc
 *  Type:		  Upgrade
 *  Description:   Gives you the ability to move faster when shooting
 *  Author:		ArsiRC
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
new Module:g_moduleshock;

new g_shockUpgradeConfigCache[UpgradeConfigs];

/**
 * Register this module.
 */
shock_Register()
{
	// Define all the module's data as layed out by enum ModuleData in project.inc.
	new moduledata[ModuleData];
	
	moduledata[ModuleData_Disabled] = false;
	moduledata[ModuleData_Hidden] = false;
	strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "shock");
	strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "shock");
	strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "shock");

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
	g_moduleshock = ModuleMgr_Register(moduledata);

	// Now register the events we're going to use.
	#if defined EVENT_MANAGER
		EventMgr_RegisterEvent(g_moduleshock, "Event_OnEventsRegister", "shock_OnEventsRegister");
	#endif
	
	// Register config file(s) that this module will use.
	#if defined CONFIG_MANAGER
		ConfigMgr_Register(g_moduleshock, "shock_OnConfigReload", "configs/thc_rpg/shock.txt");
	#endif
}

/**
 * Register all events here.
 */
public shock_OnEventsRegister()
{
	// Register all the events needed for this module.
	EventMgr_RegisterEvent(g_moduleshock, "Event_OnMapStart",				   "shock_OnMapStart");
	#if defined PROJECT_GAME_CSS
		EventMgr_RegisterEvent(g_moduleshock, "Event_PlayerHurt",			  "shock_PlayerHurt");
	#endif
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public shock_OnEventsReady()
{
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public shock_OnConfigReload(configindex)
{
	#if defined LOG_MANAGER
		LogMgr_Print(g_moduleshock, LogType_Normal, "shockConfigReload", "Reloaded speed module's config (index %d)", configindex);
	#endif

	ConfigMgr_CacheKv(g_moduleshock, CM_CONFIGINDEX_FIRST, "shockModule_ConfigCache");
}

/**
 * Read config values
 */
public KvCache:shockModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
	// Read Upgrade config
	if(StrEqual(sectionname, "shock", false))
	{
		g_shockUpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
		g_shockUpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
		g_shockUpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
		g_shockUpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
		g_shockUpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");
		g_shockUpgradeConfigCache[UpgradeConfig_Percent] = KvGetFloat(kv, "percent");
		g_shockUpgradeConfigCache[UpgradeConfig_Damage] = KvGetFloat(kv, "damage");

		if(g_shockUpgradeConfigCache[UpgradeConfig_Disable]==1)
			ModuleMgr_Disable(g_moduleshock);

		ModuleMgr_WriteCell(g_moduleshock, ModuleData_TeamLock, g_shockUpgradeConfigCache[UpgradeConfig_TeamLock]);
		ModuleMgr_WriteCell(g_moduleshock, ModuleData_MaxLevel, g_shockUpgradeConfigCache[UpgradeConfig_MaxLevel]);
		ModuleMgr_WriteCell(g_moduleshock, ModuleData_Cost, g_shockUpgradeConfigCache[UpgradeConfig_Cost]);
		ModuleMgr_WriteCell(g_moduleshock, ModuleData_iCost, g_shockUpgradeConfigCache[UpgradeConfig_iCost]);
	}
	else
		ModuleMgr_Disable(g_moduleshock);
}
#endif

/**
 * The map has started.
 */
public shock_OnMapStart()
{
	#if defined CONFIG_MANAGER
		ConfigMgr_CacheKv(g_moduleshock, CM_CONFIGINDEX_FIRST, "shockModule_ConfigCache");
	#endif
}

/**
 * Client has been damaged.
 * 
 * @param victim		The index of the hurt client.
 * @param attacker	  The index of the attacking client.
 * @param health		How much health the client has after the damage.
 * @param armor		 How much armor the client has after the damage.
 * @param weapon		The weapon classname used to hurt the victim. (No weapon_ prefix)
 * @param dmg_health	The amount of health the victim lost.
 * @param dmg_armor	 The amount of armor the victim lost.
 * @param hitgroup	  The hitgroup index of the victim that was damaged.
 */
public shock_PlayerHurt(victim, attacker, health, armor, const String:weapon[], dmg_health, dmg_armor, hitgroup)
{
	if(AreValidPlayers(victim, attacker))
	{
		new level = GetPlayerUpgradeLevel(victim, g_moduleshock);
		if(level > 0 && hitgroup == 2) // hitgroup == 1 
		{
			if(GetRandomFloat(0.0, 1.0) <= (g_shockUpgradeConfigCache[UpgradeConfig_Percent]*level))
			{
				decl Float:pos[3], Float:i_pos[3];
				GetClientAbsOrigin(victim, pos);
				GetClientAbsOrigin(attacker, i_pos);
				new Float:distance = GetVectorDistance(pos, i_pos);
				new ent = CreateEntityByName("point_tesla");
				if(ent != -1)
				{
					DispatchKeyValue(ent, "m_bOn", "1");
					DispatchKeyValue(ent, "beamcount_max", "50");
					DispatchKeyValue(ent, "beamcount_min", "15");
					DispatchKeyValue(ent, "interval_max", "0.5");
					DispatchKeyValue(ent, "thick_min", "2");
					DispatchKeyValue(ent, "interval_min", "0");
					DispatchKeyValue(ent, "thick_max", "6");
					DispatchKeyValue(ent, "lifetime_max", "0.3");
					DispatchKeyValue(ent, "texture", "sprites/physbeam.spr");
					DispatchKeyValue(ent, "lifetime_min", "0.2");
					DispatchKeyValue(ent, "m_SoundName", "DoSpark");
					decl String:radius[8];
					FloatToString(distance, radius, sizeof(radius));
					DispatchKeyValue(ent, "m_flRadius", radius);
					if(GetClientTeam(victim)==2)
						DispatchKeyValue(ent, "m_Color", "255 0 0");
					else if(GetClientTeam(victim)==3)
						DispatchKeyValue(ent, "m_Color", "0 0 255");
					DispatchSpawn(ent);
					TeleportEntity(ent, pos, NULL_VECTOR, NULL_VECTOR);
					ActivateEntity(ent);
					CreateTimer(7.5, Timer_DeleteEffect, ent, TIMER_FLAG_NO_MAPCHANGE);
					for(new i = 1; i <= MaxClients; i++)
						if(victim != i && IsValidPlayer(i, true))
						{
							GetClientAbsOrigin(i, i_pos);
							if(GetVectorDistance(pos, i_pos) <= distance)
								if(GetClientTeam(victim) != GetClientTeam(i))
									RPGDamage(i, GetRandomInt(50, RoundToNearest(g_shockUpgradeConfigCache[UpgradeConfig_Damage])*level), victim, 256, "weapon_shock");
						}
				}
			}
		}
	}
}

public Action:Timer_DeleteEffect(Handle:timer, any:ent)
{
	if(IsValidEntity(ent))
		AcceptEntityInput(ent, "Kill");
}