/*
 * Simple gold, silver & copper SmaugFUSS1.9 Bank
 * Inspired & based in the SMAUG Bank Support of Minas Ravenblood.
 * It needs gold, silver & copper support installed.
 * I use it with the "Gold/Silver/Copper, snippet by The Dark Druid"
 * May be used freely. No credit is need. May contain bugs.
 * Coded by Leia. Thanks. Enjoy!
 */
 
/*  Install:

1.) Edit mud.h and add 3 fields to the pc_data structure called 'gbalance, sbalance & cbalance'.

2.) Edit DB.C and SAVE.C to save/read the "gbalance, sbalance & cbalance" fields.

3.) Cut and Paste this code where you wish it to be.

4.) Add an ACT_BANKER mob flag.

5.) Add do_bank to mud.h.

6.) Create a help file.

7.) Create a command bank with cedit. Recommended level is 2.

8.) Set the banker flag on a mob of your choice, and enjoy your new
    banking system. */

void do_bank( CHAR_DATA *ch, const char *argument )
{
	CHAR_DATA *banker, *vch;
	char arg[MIL], arg2[MIL], arg3[MIL], arg4[MIL], buf[MSL];
	bool Gold = FALSE, Silver = FALSE, Copper = FALSE, Deposit = FALSE, Withdraw = FALSE, Transfer = FALSE, All = FALSE;
	int amount;
 
	for( banker = ch->in_room->first_person; banker; banker = banker->next_in_room )
		if( IS_NPC( banker ) && xIS_SET( banker->act, ACT_BANKER ) )
			break;

	set_char_color( AT_GREEN, ch );
	if( !banker )
	{
		send_to_char( "You can't seem to find a banker.\n\r", ch );
		return;
	}
  
	if( IS_NPC( ch ) )
	{
		send_to_char( "Sorry, we don't do business with mobs.\n\r", ch );
		return;
	}

	if( argument[0] == '\0' )
	{
		do_say( banker, "If you need help, see HELP BANK." );
		return;
	}
	
	argument = one_argument( argument, arg );
	argument = one_argument( argument, arg2 );
	argument = one_argument( argument, arg3 );
	argument = one_argument( argument, arg4 );	
	
	if( !str_cmp( arg, "balance" ) )
	{   
		set_char_color( AT_GREEN, ch );
		ch_printf( ch, "You are carrying %d gold coin%s, %d of silver and %d of copper.\n\r", ch->gold, ( ch->gold == 1 ) ? "" : "s", ch->silver, ch->copper );
		ch_printf( ch, "You also have %d gold coin%s, %d of silver and %d of copper in the bank.", ch->pcdata->gbalance, ( ch->pcdata->gbalance == 1 ) ? "" : "s", ch->pcdata->sbalance, ch->pcdata->cbalance );
		
		return;
	}

	if( !str_cmp( arg, "deposit" ) )
		Deposit = TRUE;
		
	if( !str_cmp( arg, "withdraw" ) )
		Withdraw = TRUE;
		
	if( !str_cmp( arg, "transfer" ) )
		Transfer = TRUE;
		
	if( !Deposit && !Withdraw && !Transfer )
	{
		sprintf( buf, "%s You can deposit, withdraw or transfer.", ch->name );
		do_tell( banker, buf );
		return;
	}
	
	if( !str_cmp( arg2, "all" ) )
		All = TRUE;

	if( ( arg2[0] == '\0' || ( !All && !is_number( arg2 ) ) ) || ( Transfer && arg4[0] == '\0' ) )
	{
		if( Deposit )
			sprintf( buf, "%s How much money do you wish to deposit?", ch->name );
		if( Withdraw )
			sprintf( buf, "%s How much money do you wish to withdraw?", ch->name );
		if( Transfer )
		{
			if( ( arg2[0] == '\0' || ( !All && !is_number( arg2 ) ) ) && arg4[0] == '\0' )
				sprintf( buf, "%s How much money do you wish to send to who?", ch->name );
			if( ( arg2[0] == '\0' || ( !All && !is_number( arg2 ) ) ) && arg4[0] != '\0' )
				sprintf( buf, "%s How much money do you wish to send?", ch->name );
			if( ( All || is_number( arg2 ) ) && arg4[0] != '\0' );
				sprintf( buf, "%s Do you wish to send the money to who?", ch->name );
		}
		do_tell( banker, buf );
		return;
	}

	if( !All && is_number( arg2 ) )
	{
		amount = atoi( arg2 );
		if( amount <= 0 )
		{
			sprintf( buf, "%s Oh I see.. I didn't know I was doing business with a comedian.", ch->name );
			do_tell( banker, buf );
			return;
		}
	}
	
  	if( !str_cmp( arg3, "gold" ) )
		Gold = TRUE;
	
	if( !str_cmp( arg3, "silver" ) )
		Silver = TRUE;
	
	if( !str_cmp( arg3, "copper" ) )
		Copper = TRUE;
	
    if( !Gold && !Silver && !Copper )
	{
		sprintf( buf, "%s What kind of money, gold, silver or copper?", ch->name );
		do_tell( banker, buf );
		return;
	}
	
	if( All )
	{
		if( Deposit )
		{
			if( Gold )
				amount = ch->gold;
			if( Silver )
				amount = ch->silver;
			if( Copper )
				amount = ch->copper;
		}
		
		if( Withdraw || Transfer )
		{
			if( Gold )
				amount = ch->pcdata->gbalance;
			if( Silver )
				amount = ch->pcdata->sbalance;
			if( Copper )
				amount = ch->pcdata->cbalance;
		}
	}
	
	if( Deposit )
	{
		if( ( Gold && amount > ch->gold ) || ( Silver && amount > ch->silver ) || ( Copper && amount > ch->copper ) )
		{
			sprintf( buf, "%s Sorry, but you don't have that much %s to deposit.", ch->name, arg3 );
			do_tell( banker, buf );
			return;
		}
    
		if( Gold )
		{
			ch->gold -= amount;
			ch->pcdata->gbalance += amount;
		}
	
		if( Silver )
		{
			ch->silver -= amount;
			ch->pcdata->sbalance += amount;
		}
	
		if( Copper )
		{
			ch->copper -= amount;
			ch->pcdata->cbalance += amount;
		}
		set_char_color( AT_PLAIN, ch );
		ch_printf( ch, "You deposit %d %s coin%s.\n\r", amount, arg3, ( amount != 1 ) ? "s" : "" );
		sprintf( buf, "$n deposits %d %s coin%s.", amount, arg3, ( amount != 1 ) ? "s" : "" );
		act( AT_PLAIN, buf, ch, NULL, NULL, TO_ROOM );
		return;
	}
  
	if( Withdraw )
	{   
		if( ( Gold && amount > ch->pcdata->gbalance ) || ( Silver && amount > ch->pcdata->sbalance ) || ( Copper && amount > ch->pcdata->cbalance ) )
		{
			sprintf( buf, "%s Sorry, but you don't have that much %s in your account.", ch->name, arg3 );
			do_tell( banker, buf );
			return;
		}
    
		if( Gold )
		{
			ch->pcdata->gbalance -= amount;
			ch->gold += amount;
		}

		if( Silver )
		{
			ch->pcdata->sbalance -= amount;
			ch->silver += amount;
		}
		
		if( Copper )
		{
			ch->pcdata->cbalance -= amount;
			ch->copper += amount;
		}
		set_char_color( AT_PLAIN, ch );
		ch_printf( ch, "You withdraw %d %s coin%s.\n\r", amount, arg3, ( amount != 1 ) ? "s" : "" );
		sprintf( buf, "$n withdraws %d %s coin%s.", amount, arg3, ( amount != 1 ) ? "s" : "" );
		act( AT_PLAIN, buf, ch, NULL, NULL, TO_ROOM );
		return;
	}

	if( Transfer )
	{
		if ( !( vch = get_char_world( ch, arg4 ) ) )
		{
			sprintf( buf, "%s %s could not be located.", ch->name, capitalize( arg4 ) );
			do_tell( banker, buf );
			return;
		}
    
		if( IS_NPC( vch ) )
		{
			sprintf( buf, "%s We don't do business with mobiles...", ch->name );
			do_tell( banker, buf );
			return;
		}
		
		if( ( Gold && amount > ch->pcdata->gbalance ) || ( Silver && amount > ch->pcdata->sbalance ) || ( Copper && amount > ch->pcdata->cbalance ) )    
		{
			sprintf( buf, "%s You are very generous, but you don't have that much %s!", ch->name, arg3 );
			do_tell( banker, buf );
			return;
		}

		if( Gold )
		{
			ch->pcdata->gbalance -= amount;
			vch->pcdata->gbalance += amount;
		}
		
		if( Silver )
		{
			ch->pcdata->sbalance -= amount;
			vch->pcdata->sbalance += amount;
		}
		
		if( Copper )
		{
			ch->pcdata->cbalance -= amount;
			vch->pcdata->cbalance += amount;
		}	
		set_char_color( AT_GREEN, ch );
		ch_printf( ch, "You transfer %d %s coin%s to %s's bank account.\n\r", amount, arg3, ( amount != 1 ) ? "s" : "", capitalize( vch->name ) );
		set_char_color( AT_GREEN, vch );
		ch_printf( vch, "%s just transferred %d %s coin%s to your bank account.\n\r", capitalize( ch->name ), amount, arg3, ( amount != 1 ) ? "s" : "" );
		return;
	}
	
	if( !str_cmp( arg, "help" ) )
	{
		do_help( ch, "bank" );
		return;
	}
	return;
}