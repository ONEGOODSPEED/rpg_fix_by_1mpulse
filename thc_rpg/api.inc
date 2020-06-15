// XP
public Native_SetXP(Handle:plugin, numParams)
{
	SetXP(GetNativeCell(1), GetNativeCell(2),false);
}

public Native_SetReqXP(Handle:plugin, numParams)
{
	SetReqXP(GetNativeCell(1), GetNativeCell(2));
}

public Native_GetXP(Handle:plugin, numParams)
{
	return GetXP(GetNativeCell(1));
}

public Native_GetReqXP(Handle:plugin, numParams)
{
	return GetReqXP(GetNativeCell(1));
}

public Native_SetStaticReqXP(Handle:plugin, numParams)
{
	SetStaticReqXP(GetNativeCell(1), GetNativeCell(2));
}

public Native_SetStaticXP(Handle:plugin, numParams)
{
	SetStaticXP(GetNativeCell(1), GetNativeCell(2));
}

public Native_GetHurtXP(Handle:plugin, numParams)
{
	return GetHurtXP(GetNativeCell(1));
}

public Native_SetHurtXP(Handle:plugin, numParams)
{
	SetHurtXP(GetNativeCell(1), GetNativeCell(2));
}

//Level

public Native_SetLevel(Handle:plugin, numParams)
{
	SetLevel(GetNativeCell(1), GetNativeCell(2),false);
}

public Native_GetLevel(Handle:plugin, numParams)
{
	return GetLevel(GetNativeCell(1));
}

public Native_SetStaticLevel(Handle:plugin, numParams)
{
	SetStaticLevel(GetNativeCell(1), GetNativeCell(2));
}


// Credits

public Native_SetCredits(Handle:plugin, numParams)
{
	SetCredits(GetNativeCell(1), GetNativeCell(2),false);
}

public Native_GetCredits(Handle:plugin, numParams)
{
	return GetCredits(GetNativeCell(1));
}

public Native_SetStaticCredits(Handle:plugin, numParams)
{
	SetStaticCredits(GetNativeCell(1), GetNativeCell(2));
}

// Vip

public Native_GetVip(Handle:plugin, numParams)
{
	if(VipXp[GetNativeCell(1)] > 0)
		return VipXp[GetNativeCell(1)];
	else
		return 0;
}

// databases

public Native_RemovePlayer(Handle:plugin, numParams)
{
	RemovePlayer(GetNativeCell(1));	
}

public Native_ResetPlayer(Handle:plugin, numParams)
{
	ResetPlayer(GetNativeCell(1));
}

public Native_GetPlayerTop10(Handle:plugin, numParams)
{
	new client = GetNativeCell(1);
	return GetPlayerTop10(client);
}

public Native_SavePlayerData(Handle:plugin, numParams)
{
	SavePlayerData(GetNativeCell(1));
}

public Native_LoadPlayerData(Handle:plugin, numParams)
{
	LoadPlayerData(GetNativeCell(1));
}

public Native_InsertPlayer(Handle:plugin, numParams)
{
	InsertPlayer(GetNativeCell(1));
}

//1mpulse
public Native_GetPlayerFreezing(Handle:plugin, numParams)
{
	return GetClientFreezing(GetNativeCell(1));
}