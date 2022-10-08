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
 * @noreturn
 */
native bool IS_IncrementStattrak(int clientAccount, int victimAccount, int weaponEntity, int count);

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