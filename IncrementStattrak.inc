#if defined _incrementstattrak_included
  #endinput
#endif
#define _incrementstattrak_included

/**
 * Increment stattrak of a weapon. GetSteamAccountID function can get the accountid of players by
 * client index.
 *
 * @param clientAccount             Account id of the player.
 * @param victimAccount             Account id of the victim.
 * @param weaponEntity              Entity index of the target weapon.
 * @param count                     Increment count.
 * @return							Increment success or not.
 */
native bool IS_IncrementStattrak(int clientAccount, int victimAccount, int weaponEntity, int count);

/**
 * Decrement stattrak of a weapon. Simply by causing a uint32 overflow. GetSteamAccountID function 
 * can get the accountid of players by client index.
 *
 * @param clientAccount             Account id of the player.
 * @param victimAccount             Account id of the victim.
 * @param weaponEntity              Entity index of the target weapon.
 * @param count                     Decrement count.
 * @return							Decrement success or not.
 */
stock bool IS_DecrementStattrak(int clientAccount, int victimAccount, int weaponEntity, int count)
{
	return IS_IncrementStattrak(clientAccount, victimAccount, weaponEntity, 0x7FFFFFFF) &&
		IS_IncrementStattrak(clientAccount, victimAccount, weaponEntity, 0x7FFFFFFF - count + 2);
}

//Do not edit below this
public Extension __ext_fakequeries = 
{
	name = "IncrementStattrak",
	file = "IncrementStattrak.ext",
#if defined AUTOLOAD_EXTENSIONS
	autoload = 1,
#else
	autoload = 0,
#endif
#if defined REQUIRE_EXTENSIONS
	required = 1,
#else
	required = 0,
#endif
};

#if !defined REQUIRE_EXTENSIONS
public void __ext_fakequeries_SetNTVOptional()
{
    MarkNativeAsOptional("IS_IncrementStattrak");
}
#endif