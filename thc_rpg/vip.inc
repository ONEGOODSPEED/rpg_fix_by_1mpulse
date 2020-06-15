stock AdminVipMenu(client)
{
	if(CheckAdminAccess(client,sVipFlags))
	{
		decl String:buffer[256];
		new Handle:menu = CreateMenu(AdminVipMenuHandle);
		Format(buffer, sizeof(buffer), "%T", "VIP manager", client);
		SetMenuTitle(menu, buffer);
		Format(buffer, sizeof(buffer), "%T", "admin_give_vip", client);
		AddMenuItem(menu, "givevip", buffer);
		Format(buffer, sizeof(buffer), "%T", "admin_take_vip", client);
		AddMenuItem(menu, "takevip", buffer);
		DisplayMenu(menu, client, 30);
	}
}

public AdminVipMenuHandle(Handle:menu, MenuAction:action, param1, param2)
{
	if (action == MenuAction_Select)
	{
		new String:info[64];
		GetMenuItem(menu, param2, info, sizeof(info));
		if (IsClientInGame(param1))
		{
			if (StrEqual(info, "givevip"))
			{
				DisplayMenu(GiveVip_Menu(param1), param1, 10);
			}
			if (StrEqual(info, "takevip"))
			{
				DisplayMenu(TakeVip_Menu(param1), param1, 10);
			}
		}
	}
	else if (action == MenuAction_End)
	{
		CloseHandle(menu);
	}
}

public GetVip(client)
{
	decl String:file[255], String:Expired[85];
	new Handle:kv = CreateKeyValues("Vip");
	BuildPath(Path_SM, file, sizeof(file), "configs/thc_rpg/vip.txt");
	if (FileToKeyValues(kv, file))
	{
		decl String:TimeEnd[85];
		decl String:thetime[85], String:toks[3][25];
		decl String:SteamId[32];
		decl String:toks2[3][25];
		GetClientAuthString(client, SteamId, 32);
		FixString(SteamId);
		if (KvJumpToKey(kv, SteamId, false))
		{
			FormatTime(thetime, sizeof(thetime), "%d-%m-%Y");
			ExplodeString(thetime, "-", toks, sizeof(toks), sizeof(toks[]));
			KvGetString(kv, "expire", TimeEnd, sizeof(TimeEnd));
			Expired = TimeEnd;
			if (strlen(TimeEnd) > 0)
			{
				ExplodeString(TimeEnd, ".", toks2, sizeof(toks2), sizeof(toks2[]));
				if (StringToInt(toks[2]) > StringToInt(toks2[2]))
				{
					KvDeleteThis(kv);
					KvGoBack(kv);
					KeyValuesToFile(kv, file);
					TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Vip time is expired",thetime);
					CloseHandle(kv);
					return;
				}
				else if (StringToInt(toks[2]) == StringToInt(toks2[2]) && StringToInt(toks[1]) > StringToInt(toks2[1]))
				{
					KvDeleteThis(kv);
					KvGoBack(kv);
					KeyValuesToFile(kv, file);
					TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Vip time is expired",thetime);
					CloseHandle(kv);
					return;
				}
				else if (StringToInt(toks[2]) == StringToInt(toks2[2]) && StringToInt(toks[1]) == StringToInt(toks2[1]) && StringToInt(toks[0]) >= StringToInt(toks2[0]))
				{
					KvDeleteThis(kv);
					KvGoBack(kv);
					KeyValuesToFile(kv, file);
					TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Vip time is expired",thetime);
					CloseHandle(kv);
					return;
				}
			}
			VipXp[client] = KvGetNum(kv, "xpmodifier", 100);
		}
		else if(KvGotoFirstSubKey(kv))
		{
			decl String:section[32];
			do
			{
				if(KvGetSectionName(kv, section, sizeof(section)) && section[0] && strncmp(section, "STEAM", 5, true) != 0)
				{
					for(new i = 0; i < sizeof(section); i++) section[i] = CharToLower(section[i]);
					if(GetUserFlagBits(client) & ReadFlagString(section))
					{
						VipXp[client] = KvGetNum(kv, "xpmodifier", 100);
					}
				}
			}
			while KvGotoNextKey(kv);
		}
	}
	CloseHandle(kv);
}

stock Handle:GiveVip_Menu(client)
{
	decl String:buffer[256];
	new Handle:menu = CreateMenu(GiveVip_Handle);
	Format(buffer, sizeof(buffer), "%T", "GiveVipTitle", client);
	SetMenuTitle(menu,buffer);
	SetMenuExitButton(menu, true);
	new Handle:kv = CreateKeyValues("Vip");
	for (new i = 1; i <= MaxClients; i++)
	{
		decl String:file[255];
		BuildPath(Path_SM, file, sizeof(file), "configs/thc_rpg/vip.txt");
		decl String:UserID[8], String:SteamId[64];
		if (FileToKeyValues(kv, file))
		{
			if (IsClientInGame(i) && !IsFakeClient(i))
			{
				GetClientAuthString(i, SteamId, sizeof(SteamId));
				if (KvJumpToKey(kv, SteamId, false))
				{
					Format(UserID, sizeof(UserID), "%d", i);
					Format(buffer, sizeof(buffer),"%N [VIP]", i);
					AddMenuItem(menu, UserID, buffer, ITEMDRAW_DISABLED);
				}
				else
				{
					Format(buffer, sizeof(buffer), "%N", i);
					Format(UserID, sizeof(UserID), "%d", i);
					AddMenuItem(menu, UserID, buffer);
				}
			}
		}
	}
	CloseHandle(kv);
	return menu;
}

stock Handle:TakeVip_Menu(client)
{
	decl String: buffer[128];
	new Handle:menu = CreateMenu(TakeVip_Handle);
	Format(buffer, sizeof(buffer), "%T", "TakeVipTitle", client);
	SetMenuTitle(menu,buffer);
	SetMenuExitButton(menu, true);
	new Handle:kv = CreateKeyValues("Vip");
	for (new i = 1; i <= MaxClients; i++)
	{
		decl String:file[255];
		BuildPath(Path_SM, file, sizeof(file), "configs/thc_rpg/vip.txt");
		if (FileToKeyValues(kv, file))
		{
			decl String:UserID[8], String:sName[128], String:SteamId[64];
			if(IsClientInGame(i) && !IsFakeClient(i))
			{
				GetClientAuthString(i, SteamId, sizeof(SteamId));
				if (KvJumpToKey(kv, SteamId, false))
				{
					GetClientName(i, sName, sizeof(sName));
					Format(UserID, sizeof(UserID), "%d", i);
					AddMenuItem(menu, UserID, sName);
				}
				else
				{
					GetClientName(i, sName, sizeof(sName));
					Format(UserID, sizeof(UserID), "%d", i);
					Format(sName, sizeof(sName), "%s [NO VIP]", sName);
					AddMenuItem(menu, UserID, sName, ITEMDRAW_DISABLED);
				}
			}
		}
	}
	CloseHandle(kv);
	return menu;
}

public GiveVip_Handle(Handle:menu, MenuAction:action, client, param2)
{
	if(action == MenuAction_Select)
	{
		decl String:info[8],String:buff[256];
		GetMenuItem(menu, param2, info, sizeof(info));
		new Handle:GiveVip = CreateMenu(GiveVipHandler);
		Format(buff, sizeof(buff), "%T", "Select the experience factor", client);
		SetMenuTitle(GiveVip,buff);
		SetMenuExitButton(GiveVip, true);
		
		decl String:file[255];
		new Handle:kv = CreateKeyValues("Vip");
		BuildPath(Path_SM, file, sizeof(file), "configs/thc_rpg/vip.txt");
		if (FileToKeyValues(kv, file))
		{
			decl String:SteamId[64];
			decl String:buffer[16];
			GetClientAuthString(StringToInt(info), SteamId, sizeof(SteamId));
			if (KvJumpToKey(kv, SteamId, false))
			{
				Format(buff, sizeof(buff), "%T", "Player already have vip");
				AddMenuItem(GiveVip, info, buff, ITEMDRAW_DISABLED);
			}
			else
			{
				Format(buffer, sizeof(buffer), "%d|150", StringToInt(info));
				AddMenuItem(GiveVip, buffer, "x1.5");
				Format(buffer, sizeof(buffer), "%d|200", StringToInt(info));
				AddMenuItem(GiveVip, buffer, "x2");
				Format(buffer, sizeof(buffer), "%d|250", StringToInt(info));
				AddMenuItem(GiveVip, buffer, "x2.5");
				Format(buffer, sizeof(buffer), "%d|300", StringToInt(info));
				AddMenuItem(GiveVip, buffer, "x3");
				Format(buffer, sizeof(buffer), "%d|500", StringToInt(info));
				AddMenuItem(GiveVip, buffer, "x5");
			}
		}
		CloseHandle(kv);
		DisplayMenu(GiveVip, client, 10);
	}
	else if (action == MenuAction_End)
	{
		CloseHandle(menu);
	}
}

public GiveVipHandler(Handle:menu, MenuAction:action, client, param2)
{
	if(action == MenuAction_Select)
	{
		decl String:info[16], String:toks[2][8];
		GetMenuItem(menu, param2, info, sizeof(info));
		ExplodeString(info, "|", toks, 2, 8);
		if (IsClientInGame(StringToInt(toks[0])))
		{
			decl String:file[255];
			new Handle:kv = CreateKeyValues("Vip");
			BuildPath(Path_SM, file, sizeof(file), "configs/thc_rpg/vip.txt");
			if (FileToKeyValues(kv, file))
			{
				new String:SteamId[64];
				GetClientAuthString(StringToInt(toks[0]), SteamId, sizeof(SteamId));
				if (KvJumpToKey(kv, SteamId, true))
				{
					KvSetNum(kv, "xpmodifier", StringToInt(toks[1]));
					decl String:thetime[85], String:toks2[3][25];
					FormatTime(thetime, sizeof(thetime), "%d-%m-%Y");
					ExplodeString(thetime, "-", toks2, sizeof(toks2), sizeof(toks2[]));
					decl String:expired[85], String:Name[64];
					if (StringToInt(toks2[1]) < 12)
					{
						new Value = StringToInt(toks2[1]) + 1;
						Format(expired, sizeof(expired), "%d.%d.%d", StringToInt(toks2[0]), Value, StringToInt(toks2[2]));
					}
					else
					{
						new Value = StringToInt(toks2[2]) + 1;
						Format(expired, sizeof(expired), "%d.01.%d", StringToInt(toks2[0]), Value);
					}
					GetClientName(StringToInt(toks[0]), Name, sizeof(Name));
					KvSetString(kv, "name", Name);
					KvSetString(kv, "expire", expired);
					KvGoBack(kv);
					KeyValuesToFile(kv, file);
					new percent = StringToInt(toks[1]);
					TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Vip successfully given",StringToInt(toks[0]), percent, expired);
					TransMgr_PrintText(StringToInt(toks[0]), MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Vip successfully given client", client, percent, expired);
					GetVip(StringToInt(toks[0]));
				}
			}
			CloseHandle(kv);
		}
	}
	else if (action == MenuAction_End)
	{
		CloseHandle(menu);
	}
}

public TakeVip_Handle(Handle:menu, MenuAction:action, client, param2)
{
	if(action == MenuAction_Select)
	{
		decl String:info[16];
		GetMenuItem(menu, param2, info, sizeof(info));
		new user = StringToInt(info);
		if (IsClientInGame(user))
		{
			decl String:file[255];
			new Handle:kv = CreateKeyValues("Vip");
			BuildPath(Path_SM, file, sizeof(file), "configs/thc_rpg/vip.txt");
			if (FileToKeyValues(kv, file))
			{
				new String:SteamId[64];
				GetClientAuthString(user, SteamId, sizeof(SteamId));
				if (KvJumpToKey(kv, SteamId, false))
				{
					KvDeleteThis(kv);
					KvGoBack(kv);
					KeyValuesToFile(kv, file);
					TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Vip successfully taken", user);
					TransMgr_PrintText(user, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Vip successfully taken client", client);
					VipXp[user] = 0;
				}
			}
			CloseHandle(kv);
		}
	}
	else if (action == MenuAction_End)
	{
		CloseHandle(menu);
	}
}



public FixString(String:Str[])
{
	StripQuotes(Str);
	TrimString(Str);
}