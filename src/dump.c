/*  Dump command...This command creates a text file with the stats of every  *
 *  mob, or object in the mud, depending on the argument given.              *
 *  Obviously, this will tend to create HUGE files, so it is recommended     *
 *  that it be only given to VERY high level imms, and preferably those      *
 *  with shell access, so that they may clean it out, when they are done     *
 *  with it.
 */


#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include "mud.h"

void do_dump(CHAR_DATA *ch, const char *argument)
  {
    MOB_INDEX_DATA   *mob;
    OBJ_INDEX_DATA   *obj;
    AFFECT_DATA      *paf;

    long             counter;
    char             arg[MAX_INPUT_LENGTH];


    argument = one_argument(argument, arg);

	if(IS_NPC(ch))
	  return;

	if(get_trust(ch) < LEVEL_SUPREME)

	  {
	    send_to_char("Sorry, only the Implementor may use this command!\n\r", ch);
	    return;
	  }

	if(!str_cmp(arg, "mobs") )
	  {
	    FILE  *fp = fopen(MOBDUMP_FILE, "w");
	    send_to_char("Writing to file mobdata.dump ...\n\r", ch);

	    for(counter=0; counter <= 32600; counter++)
	      {
		if ( (mob = get_mob_index(counter) ) != NULL )
		  {
		    fprintf(fp, "VNUM:  %d\n", mob->vnum);
		    fprintf(fp, "S_DESC:  %s\n", mob->short_descr);
		    fprintf(fp, "LEVEL:  %d\n", mob->level);
		    // fprintf(fp, "HITROLL:  %d\n", GET_HITROLL(mob) );
		    // fprintf(fp, "DAMROLL:  %d\n", GET_DAMROLL(mob) );
		    fprintf(fp, "HITDIE:  %dd%d+%d\n", mob->hitnodice, mob->hitsizedice, mob->hitplus );
		    fprintf(fp, "DAMDIE:  %dd%d+%d\n", mob->damnodice, mob->damsizedice, mob->damplus );
		    fprintf(fp, "ACT FLAGS:  %s\n", ext_flag_string(&mob->act, act_flags) );
		    fprintf(fp, "AFFECTED_BY:  %s\n", affect_bit_name(&mob->affected_by) );
		    fprintf(fp, "RESISTS:  %s\n", flag_string(mob->resistant, ris_flags) );
		    fprintf(fp, "SUSCEPTS:  %s\n", flag_string(mob->susceptible, ris_flags) );
		    fprintf(fp, "IMMUNE:  %s\n", flag_string(mob->immune, ris_flags) );
		    fprintf(fp, "ATTACKS:  %s\n", ext_flag_string(&mob->attacks, attack_flags) );
		    fprintf(fp, "DEFENSES:  %s\n\n\n", ext_flag_string(&mob->defenses, defense_flags) );
		  }
		}

	      fclose (fp);
	      send_to_char("Done.\n\r", ch);
	      return;
	    }

	if(!str_cmp(arg, "objects") )
	  {
	    FILE *fp = fopen(OBJDUMP_FILE, "w");
	    send_to_char("Writing objects to file objdata.dump ...\n\r", ch);

	    for(counter=0; counter <= 32600; counter++)
	      {
		if ( (obj = get_obj_index(counter) ) != NULL )
		  {
		    fprintf(fp, "VNUM:  %d\n", obj->vnum);
		    fprintf(fp, "KEYWORDS:  %s\n", obj->name);
		    fprintf(fp, "TYPE:  %s\n", o_types[obj->item_type]);
	//		if ( obj->item_type == ITEM_WEAPON )
	//		 {
	//		  fprintf( fp, "WEAPON CLASS: %s\n\r", get_weapon_type( obj ) );
	//		 }
		    fprintf(fp, "SHORT DESC:  %s\n", obj->short_descr);
		    fprintf(fp, "WEARFLAGS:  %s\n", flag_string(obj->wear_flags, w_flags) );
		    fprintf(fp, "FLAGS:  %s\n", ext_flag_string(&obj->extra_flags, o_flags) );
                    fprintf(fp, "WEIGHT: %d\n", obj->weight); 
		    fprintf(fp, "AFFECTS:\n");

                        for ( paf = obj->first_affect; paf; paf = paf->next )
                          fprintf(fp, "Affects %s by %d.\n",
                          affect_loc_name( paf->location ), paf->modifier );

			    if(obj->layers > 0)
			      fprintf(fp, "Layerable - Wear layer: %d\n", obj->layers);
		    fprintf(fp, "INDEX VALUES: v0: %d v1: %d v2: %d v3: %d v4: %d v5: %d\n\n\n",
			obj->value[0], obj->value[1], obj->value[2],
			obj->value[3], obj->value[4], obj->value[5] );
		  }
		}

	      fclose (fp);
	      send_to_char( "Done.\n\r", ch);
	      return;
	    }

	send_to_char( "Syntax: dump <mobs/objects>\n\r", ch );
	return;

  }