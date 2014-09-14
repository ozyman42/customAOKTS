//Create your custom scripts in this file.
#include "custscript.h"
/***************************************************************************************************/
//This section is for includes that I added to perform all desired customizations that fit my needs.
#include "./model/scen.h" //For scenario editing.
/***************************************************************************************************/

//SCEN is a scenario pointer that only needs to be here if you are creating other functions outside
//of just customScript.
Scenario * SCEN;
//Below is an array of the players' heros. UID stands for Unit Identification.
UID heroes[7];
//Here is an Array of the locations where players' kings will be created.
AOKPT kingLocs[7] = {AOKPT(237, 0), AOKPT(235, 2), AOKPT(237, 2), AOKPT(239, 2), AOKPT(235, 4), AOKPT(237, 4), AOKPT(239, 4)};

//the void customScript is the entry point for all customization.
void customScript(Scenario * scen)
{
	//BEFORE YOU CONTINUE:
	/*
	1. Know that the below implementation is meant to serve as an example for how to edit Triggers.
	2. I got lazy when writing the script and chose not to update the graphics once triggers are updated,
	   so once you run your custom script, if you edit in the way I have, you will need to go to a different
	   tab then come back to the Triggers tab to properly update the Trigger display tree.
	3. There seems to be a weird error when I update the pUnit in an effect or condition. When I update it, it only
	   shows the change once I save the new scenario and reopen it.
	4. My comments both give my editing context and explain the different methods I used, methods that are 
	   already part of the AoKTS, thanks to DiGiT.
	*/
	
	//Store scen so that it may be accessed in other functions.
	SCEN = scen;
	
	//I only want my custom script to be carried out on a specific scenario I made, so I check for the name.
	if(strcmp(scen->origname, "FATTY WAR V1,4.scx"))
	{
		throw std::exception("This custom script was intended only to run on the\n\"FATTY WAR V1,4.scx\" scenario.");
	}
	//In my scenario, players must complete quests that are provided by p8, which is a computer player.
	//For some quests, a player must bring their hero (a shah) to an orange outpost to complete the quest.
	//The problem is, the hero is controlled by p8 until the human player can complete the quest, so it is
	//possible that a player might accidentally send their hero to the orange outpost when the player did not
	//want to complete that quest in the first place, causing them to be forced to complete the quest
	//to regain control of their hero.
	//My solution (implementing a way for the player to opt out of the quest) requires 7 new triggers
	//for each quest, and so instead of adding the rest by hand, I will complete the task programatically.

	//Before I being the implementation, I want to store some important objects.
	//Here, I am now going to store the different players' heroes.
	for(int i = 0; i < 7; ++i)//7 players total. Each has a hero.
	{
		//The second trigger's effects change the names of each of the heroes, so I can copy the units from there.
		heroes[i] = scen->triggers.at(1)->effects.at(i).uids[0];//uids is an array of unit IDs that each effect has.
	}
	
	//Now to begin the solution...
	//There are 17 quests I have to implement my solution for. 16 of them have very similar names,
	//and one has a different name. I am using the different one for testing, under the singleImplement
	//method, and if the code I write updates that quest properly, I will use the same implementation
	//for the other quests.
	singleImplement();
	fullImplement();
	//If you are curious about the results, you may compare the scenarios entitled "FATTY WAR V1,4,2.scx" and 
	//"FATTY WAR V1,5.scx", where the first was the file before I ran the script and the latter was the result.
	//If you look in version 1,4,2, you will notice that the TCSpawn quest is already implemented. I had done 
	//this quest by hand, at which point, I realized doing the rest by hand would be way too much work.
	
	//I hope this code has helped as an example of how one would go about creating their own custom script for
	//editing scenarios. 
	//Sincerely, Alex Leung
}

void singleImplement()
{
	//The solution I have is that when a player's hero is controlled by p8, a new king is created at the north corner
	//of the map, and if the player kills their new king, the player regains control of their hero.
	//In all quests, there is a set of two triggers, one that marks when a player's hero arrives at the orange
	//outpost, thus causing p8 to control their hero and initializing the quest, and another that determines when 
	//a player completes the quest, thus returning the control of the player's hero to them and giving the player the
	//quest's reward. I will need to create a third trigger now, a KillKing Trigger that marks when a player's king that is created
	//in the north corner of the map is destroyed, so I can return the player's hero to them.
	//First we want to locate the existing QuestArrive and QuestComplete Triggers (there are 7 of each so all players are covered.)
	//since we will need to reactivate all of the QuestArrive Triggers once the players kills his king, thus opening the
	//quest up to everyone again, and we will also need to deactivate the new KillKing Trigger if the QuestComplete Trigger fires.
	//For this quest, the QuestArrive Trigger is called ArriveUniqueConstr while the QuestComplete
	//Trigger is called BuildForOrange, where Constr stands for construction since this quest requires a player
	//to build structures that are then converted to orange once the required buildings are complete. 
	//In the Trigger List, the BuildForOrange Triggers come first, so, in List, one would see
	//BuildForOrange1
	//BuildForOrange2
	//BuildForOrange3
	//BuildForOrange4
	//BuildForOrange5
	//BuildForOrange6
	//BuildForOrange7
	//ArriveUniqueConstr1
	//ArriveUniqueConstr2
	//ArriveUniqueConstr3
	//ArriveUniqueConstr4
	//ArriveUniqueConstr5
	//ArriveUniqueConstr6
	//ArriveUniqueConstr7.
	//So if we can get the index for BuildForOrange1, we can access the rest of the triggers.
	int trigI = -1;//Initializing the index within the trigger list.
	while(strcmp(SCEN->triggers.at(++trigI)->name, "BuildForOrange1"));//Setting trigI to the index of the BuildForOrange1 Trigger.
	//Now that we have the Trigger indices, we can move forward to create the new KillKing Trigger.
	Trigger * toCopy = new Trigger();//Create Trigger.
	strcpy(toCopy->name, "OrangeConstrKillKing");//Name Trigger.
	toCopy->state = false;//Set the sarting state of the Trigger as Off.
	toCopy->loop = true;//Allow the Trigger to refire infinite times.
	//We cannot use a UnitDestroyed Condition since there is no King unit on the map to start, but since
	//there only will ever be one king that each player has, we can create a condition that checks to see if the
	//player has any kings at all, and if they don't, they must have deleted their king.
	Condition * c = new Condition();//Create Condition.
	c->type = ConditionType::CONDITION_OwnFewerObjects;//Set the Condition Type.
	c->setPlayer(1);//Set the Source Player.
	//Check scen_const.cpp. In the PAIR array named utypes, 4 corresponds to Military from the ObjectTypes list.
	c->utype = 4;//Set ObjectType to Military.
	//Check scen_const.cpp. In the PAIR array named groups, 0x3B corresponds to KING from the ObjectGroup list.
	c->group = 0x3B;//Set ObjectGroup to KING.
	//Check data_aok.xml. One of the "protounit" tags has the id 434, which corresponds to the King unit.
	c->pUnit = new UnitLink(434, (L"King"));//Set the Object to King.
	c->amount = 0;//Set the Quantity to zero, so if there are ever zero kings, the Trigger Fires.
	toCopy->conds.push_back(*c);//Add the condition to the Trigger.
	//For all of the QuestArrive Triggers in the scenario, once a QuestArrive Trigger fires for a particular player,
	//a flag of that player's color is created by the orange outpost where the player's hero has arrived, to mark that
	//the quest is either being completed, or has been completed, by that player. Therefore, if that player decides
	//to delete his king, we will also need to remove the flag. What we will do for this effect is copy the create flag
	//effect from the QuestArrive (ArriveUniqueConstr) Trigger, change its effect type to removeObject, then paste it to
	//this new KillKing Trigger.
	Effect * e = new Effect();//Create Trigger.
	int createFlagEffectIndex = -1;//Initialize the index that will locate the effect within the ArriveUniqueConstr1
								   //Trigger that points to the Unit Flag A.
	//Below we are retreiving the propper value for the index described above. Remember that pUnit refers to the
	//literal object selected from the ObjectList. We add 7 to trigI to get from the BuildForOrange1 Trigger to 
	//the ArriveUniqueConstr1 Trigger.
	while(wcscmp(SCEN->triggers.at(trigI+7)->effects.at(++createFlagEffectIndex).pUnit->name(), L"Flag A"));
	*e = SCEN->triggers.at(trigI+7)->effects.at(createFlagEffectIndex);//Get our copy of the Effect.
	e->type = EffectType::EFFECT_RemoveObject;//Change Effect Type from CreateObject to RemoveObject.
	//In a CreateObject effect, a location is specified, but in a RemoveObject effect, an area is specified. We therefore have to convert
	//the location to an area.
	e->area = AOKRECT(e->location.y, e->location.x, e->location.y, e->location.x);
	toCopy->effects.push_back(*e);//Add the Effect to the Trigger.
	//Now we may create the effect that changes the ownership of the hero from p8 to the original owner.
	e = new Effect();
	e->type = EffectType::EFFECT_ChangeOwnership;
	e->num_sel = 1;//If an effect is going to select any units, the number of selected units must be specified.
	e->uids[0] = heroes[0];//Set the selected to Player 1's Hero.
	e->s_player = 8;//Set Source Player.
	e->t_player = 1;//Set Target Player.
	toCopy->effects.push_back(*e);
	//Now we need to add Effects that re-activate all of the QuestArrive (ArriveUniqueConstr) Triggers.
	for(int i = 0; i < 7; ++i)
	{
		e = new Effect();
		e->type = EffectType::EFFECT_ActivateTrigger;
		e->trig_index = trigI + 7 + i;//Set Trigger index (points to Trigger in Trigger List).
		toCopy->effects.push_back(*e);
	}
	//Now we have to Deactivate the new KillKing Trigger so it won't fire again 
	//until the player brings their hero back to the orange outpost.
	e = new Effect();
	e->type = EffectType::EFFECT_DeactivateTrigger;
	//To Deactivate itself, set the index to the size of the Trigger List, because although there is not yet a Trigger at 
	//that index, there will be shortly.
	e->trig_index = SCEN->triggers.count();
	toCopy->effects.push_back(*e);
	//We also want to deactivate the QuestComplete Trigger since a player should not be allowed to complete the quest if they
	//just chose they no longer want to complete it.
	e = new Effect();
	e->type = EffectType::EFFECT_DeactivateTrigger;
	e->trig_index = trigI;
	toCopy->effects.push_back(*e);
	//Now we want to duplicate the new KillKing Trigger for all players.
	//I'm not sure if I can access the duplicateForAllPlayers function, but I'd prefer not to
	//use that since it is limited. For instance, I only want to duplicate for 7 players.
	//Since I will do a custom duplicate, within the loop I can also add the effect to all of the
	//QuestArrive Triggers that will create the king for each player in the north corner
	//along with some other effects.
	Trigger * trig;
	for(int i = 0; i < 7; ++i)
	{
		//Here we begin the duplicate.
		trig = new Trigger();
		*trig = *toCopy;
		//Append the Player Number to the Trigger name.
		char integer[2];
		sprintf(integer, "%d", i+1);
		strcat(trig->name, integer);
		//Edit player numbers.
		trig->conds.at(0).setPlayer(i+1);//Reset the source player for the OwnFewer condition.
		trig->effects.at(0).s_player = i+1;//Reset the source player for the RemoveObject (Flag A) Effect.
		trig->effects.at(1).t_player = i+1;//Reset the target player gor the ChangeOwnership Effect.
		//Reset Trigger indices.
		trig->effects.at(9).trig_index = SCEN->triggers.count();//Reset the deactivate self Trigger.
		trig->effects.at(10).trig_index = trigI + i;//Reset the deactivate QuestComplete Trigger Trigger.
		//Reset ChangeOwnership unit selection.
		trig->effects.at(1).uids[0] = heroes[i];
		//Now we may add the KillKing Trigger.
		SCEN->insert_trigger(trig, SCEN->triggers.count());//Add the Trigger to the Scenario.
		//Below are some other tasks that need to be done for each player.
		//First we must edit the QuestArrive (ArriveUniqueConstr) Trigger.
		trig = SCEN->triggers.at(trigI+7+i);
		//Originally, I had not intended to have a player be allowed to complete the QuestArrive Trigger more
		//than once, so there was no need to have the Triggers loop. Now there is such a need.
		trig->loop = true;
		e = new Effect();
		e->type = EffectType::EFFECT_ActivateTrigger;
		e->trig_index = SCEN->triggers.count()-1;//Set Trigger index to new KillKing Trigger.
		trig->effects.push_back(*e);
		e = new Effect();
		e->type = EffectType::EFFECT_DeactivateTrigger;
		e->trig_index = trigI+7+i;//Set Trigger index to self.
		trig->effects.push_back(*e);
		e = new Effect();
		e->type = EffectType::EFFECT_CreateObject;
		e->location = kingLocs[i];//Set location where king will spawn.
		e->s_player = i+1;
		e->pUnit = new UnitLink(434, (L"King"));
		trig->effects.push_back(*e);
		//Now we must edit the QuestComplete (BuildForOrange) Trigger.
		//If the player completes the quest, then they get back their hero, so we should
		//deactivate the KillKing trigger then remove all of the kings.
		trig = SCEN->triggers.at(trigI+i);
		e = new Effect();
		e->type = EffectType::EFFECT_DeactivateTrigger;
		e->trig_index = SCEN->triggers.count()-1;
		trig->effects.push_back(*e);
		e = new Effect();
		e->type = EffectType::EFFECT_RemoveObject;
		e->s_player = i+1;
		e->pUnit = new UnitLink(434, (L"King"));
		trig->effects.push_back(*e);
	}
}

void fullImplement()
{
	//Since the implentation with the first quest worked, I will repeat it for the rest of the quests.
	//Below is a list of quests that need to be updated. Unlike the previous quest, these all have the
	//same naming style. First there is the name, then, if it is a QuestArrive Trigger, the string
	//"Arrive" is added on to it, while if it is a QuestComplete Trigger, the string "ReqMet" is added
	//to it. So, for example, the Hussar quest looks like this in the Trigger List:
	//HussarArrive (p1)
	//HussarArrive (p2)
	//HussarArrive (p3)
	//HussarArrive (p4)
	//HussarArrive (p5)
	//HussarArrive (p6)
	//HussarArrive (p7)
	//HussarReqMet (p1)
	//HussarReqMet (p2)
	//HussarReqMet (p3)
	//HussarReqMet (p4)
	//HussarReqMet (p5)
	//HussarReqMet (p6)
	//HussarReqMet (p7)
	char startOfQuest[16][11] = {
		"Hussar", 
		"HandCannon", 
		"Champs", 
		"Res",
		"Wolf",
		"MarketRel",
		"SuperScout",
		"Tarkan",
		"Monastery",
		"HP",
		"TCDirt",
		"TCWater",
		"TCJungle",
		"TCBamboo",
		"TCMeadow",
		"TCSnow"};
	//We will loop through all of the quests. For each one, we will find the index of its QuestArrive Trigger, since the QuestArrive
	//Trigger typically comes first, then we will pass that index, along with the quest name, on to the standardImplement method to 
	//do the rest of the work.
	int trigI = -1;
	for(int i = 0; i < 16; ++i)
	{
		//We need to get the p1 QuestArrive version of the quest in order to get the correct trigI.
		char arriveVer[22];
		sprintf(arriveVer, "%sArrive (p1)", startOfQuest[i]);
		while(strcmp(SCEN->triggers.at(++trigI)->name, arriveVer));
		//if(i==5)
		//{
			//char error[50];
			//wcstombs(error,  SCEN->triggers.at(trigI)->effects.at(2).pUnit->name(), 50);
			//throw std::exception(error);
		//}
		standardImplement(trigI, startOfQuest[i]);
	}
}

void standardImplement(int trigI, char * questName)
{
	//The code below is the same code that was extensively commented in the singleImplement method.
	//The only differences are when 7 is added to trigI to get the index of either a QuestArrive
	//Trigger or a QuestComplete Trigger. Since the QuestArrive will come first for a standard quest,
	//the plus sevens to the trigI variable will be swapped with the times when trigI is used and 7 is not
	//added to it. Also, the naming convention for the KillKing Triggers will be slightly different.
	//The end of each new KillKing Trigger will be " (p#)" instead of "#", where # is the calculated
	//player number.
	Trigger * toCopy = new Trigger();
	char killName[19];
	sprintf(killName, "%sKillKing", questName);
	strcpy(toCopy->name, killName);
	toCopy->state = false;
	toCopy->loop = true;
	Condition * c = new Condition();
	c->type = ConditionType::CONDITION_OwnFewerObjects;
	c->setPlayer(1);
	c->utype = 4;//4 = Military
	c->group = 0x3B;//0x3B = KING.
	c->pUnit = new UnitLink(434, (L"King"));
	c->amount = 0;
	toCopy->conds.push_back(*c);
	Effect * e = new Effect();
	int createFlagEffectIndex = -1;
	while(!SCEN->triggers.at(trigI)->effects.at(++createFlagEffectIndex).pUnit || wcscmp(SCEN->triggers.at(trigI)->effects.at(createFlagEffectIndex).pUnit->name(), L"Flag A"));
	*e = SCEN->triggers.at(trigI)->effects.at(createFlagEffectIndex);
	e->type = EffectType::EFFECT_RemoveObject;
	e->area = AOKRECT(e->location.y, e->location.x, e->location.y, e->location.x);
	toCopy->effects.push_back(*e);
	e = new Effect();
	e->type = EffectType::EFFECT_ChangeOwnership;
	e->num_sel = 1;
	e->uids[0] = heroes[0];
	e->s_player = 8;
	e->t_player = 1;
	toCopy->effects.push_back(*e);
	for(int i = 0; i < 7; ++i)
	{
		e = new Effect();
		e->type = EffectType::EFFECT_ActivateTrigger;
		e->trig_index = trigI + i;
		toCopy->effects.push_back(*e);
	}
	e = new Effect();
	e->type = EffectType::EFFECT_DeactivateTrigger;
	e->trig_index = SCEN->triggers.count();
	toCopy->effects.push_back(*e);
	e = new Effect();
	e->type = EffectType::EFFECT_DeactivateTrigger;
	e->trig_index = trigI+7;
	toCopy->effects.push_back(*e);
	Trigger * trig;
	for(int i = 0; i < 7; ++i)
	{
		trig = new Trigger();
		*trig = *toCopy;
		char integer[6];
		sprintf(integer, " (p%d)", i+1);
		strcat(trig->name, integer);
		trig->conds.at(0).setPlayer(i+1);
		trig->effects.at(0).s_player = i+1;
		trig->effects.at(1).t_player = i+1;
		trig->effects.at(9).trig_index = SCEN->triggers.count();
		trig->effects.at(10).trig_index = trigI+7 + i;
		trig->effects.at(1).uids[0] = heroes[i];
		SCEN->insert_trigger(trig, SCEN->triggers.count());
		trig = SCEN->triggers.at(trigI+i);
		trig->loop = true;
		e = new Effect();
		e->type = EffectType::EFFECT_ActivateTrigger;
		e->trig_index = SCEN->triggers.count()-1;
		trig->effects.push_back(*e);
		e = new Effect();
		e->type = EffectType::EFFECT_DeactivateTrigger;
		e->trig_index = trigI+i;
		trig->effects.push_back(*e);
		e = new Effect();
		e->type = EffectType::EFFECT_CreateObject;
		e->location = kingLocs[i];
		e->s_player = i+1;
		e->pUnit = new UnitLink(434, (L"King"));
		trig->effects.push_back(*e);
		trig = SCEN->triggers.at(trigI+7+i);
		e = new Effect();
		e->type = EffectType::EFFECT_DeactivateTrigger;
		e->trig_index = SCEN->triggers.count()-1;
		trig->effects.push_back(*e);
		e = new Effect();
		e->type = EffectType::EFFECT_RemoveObject;
		e->s_player = i+1;
		e->pUnit = new UnitLink(434, (L"King"));
		trig->effects.push_back(*e);
	}
}