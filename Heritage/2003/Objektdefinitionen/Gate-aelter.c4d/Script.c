/*
Luftschleuse ALCK

Locals:

0 - Bewegungsmodus: 0 - wait
                    1 - von rechts nach links
                    2 - von links nach rechts
1 - Bewegungsstatus: 0 - none
                     1 - Eintritt
                     2 - Austritt
2 - Flüssigkeit rechts: 0 - keine
                        1 - welche
3 - Flüssigkeit links: 0 - keine
                       1 - welche

Actiondiagramm:

wait
|---------------|---------------|---------------|
<right2left>	<left2right>	<room2right>	<room2left>
|		|		|		|
*GotoPump	*GotoPump	*GotoPump	*GotoPump
|-|		|-|		|-|		|-|
| PumpLeftOut	| PumpRightOut	| PumpLeftOut	| PumpRightOut
| [einziger	|-|		|-|		|-|
| Fall, wo	|		|		|
| pumpen ge-	|		|		|
| brauch wird]	|		|		|
|-|		|		|		|
*GotoStopPump	*GotoStopPump	*GotoStopPump	*GotoStopPump
OpenRight	OpenLeft	OpenRight	OpenLeft
|		|		|		|
*GotoCloseRight	*GotoLoseLeft	*GotoCloseRight	*GotoCloseLeft
ShowEntryRight	ShowEntryLeft	ShowLeaveRight	ShowLeaveLeft
|		|		|		|
<room2right>	<room2left>	wait		wait

*/

Construction:
	Message("HALLOBALH");
	return(1);

Initialize:
	SetSolidMask(99,0,98,41);
	DigFreeRect(Sub(GetX(),55),Sub(GetY(),21),110,41);
	SetAction("Wait");
	SetSolidMask(0,100,98,41);
	SetLocal(0,0);
	SetLocal(1,0);
	return();

/* Checkfunktionen */
CheckLeft:
	return( FindObject(0, -69, -30, 25, 60, OCF_CrewMember()) );
CheckRight:
	return( FindObject(0,  39, -30, 25, 60, OCF_CrewMember()) );
CheckRoom:
	return( FindObject(0, -44, -20, 88, 41, OCF_CrewMember()) );
CheckRoomLeft:
	return( FindObject(0, -44, -20, 44, 41, OCF_CrewMember()) );
CheckRoomRight:
	return( FindObject(0,   0, -20, 44, 41, OCF_CrewMember()) );

LiquidIsRight:
	Message("Liquid is right");
	SetLocal(2,1);
	SetLocal(3,0);
	return();
LiquidIsLeft:
	Message("Liquid is left");
	SetLocal(2,0);
	SetLocal(3,1);
	return();
NoLiquid:
	SetLocal(2,0);
	SetLocal(3,0);
	return();
CheckLiquid:
	
	//level 3
	if(GBackLiquid(55,-15))
		return(LiquidIsRight());
	if(GBackLiquid(-55,-15))
		return(LiquidIsLeft());
	//level 2
	if(GBackLiquid(55,0))
		return(LiquidIsRight());
	if(GBackLiquid(-55,0))
		return(LiquidIsLeft());
	//level 1
	if(GBackLiquid(55,15))
		return(LiquidIsRight());
	if(GBackLiquid(-55,15))
		return(LiquidIsLeft());
	return(NoLiquid());

RoomIsEmpty: // kein wasser mehr drin
	return(Not(GBackLiquid(0,19)));
RoomIsFilled: // geht kein wasser mehr rein
	return(GBackLiquid(0,-15));

RoomMaterial: // material zum wegpumpen
	return(GetMaterial(0,19));
RightMaterial:
	if(GBackLiquid(55,-15))
		return(GetMaterial(55,-15));
	if(GBackLiquid(55,0))
		return(GetMaterial(55,0));
	if(GBackLiquid(55,15))
		return(GetMaterial(55,15));
	return();
LeftMaterial:
	if(GBackLiquid(-55,-15))
		return(GetMaterial(-55,-15));
	if(GBackLiquid(-55,0))
		return(GetMaterial(-55,0));
	if(GBackLiquid(-55,15))
		return(GetMaterial(-55,15));
	return();

/* --waiting-- */
EndWaitingRight2Left:
	SetLocal(0,1);
	SetLocal(1,1);
	return(GotoPump());
EndWaitingLeft2Right:
	SetLocal(0,2);
	SetLocal(1,1);
	return(GotoPump());
EndWaitingRoom2Left:
	SetLocal(0,1);	
	SetLocal(1,2);
	return(GotoPump());
EndWaitingRoom2Right:
	SetLocal(0,2);
	SetLocal(1,2);
	return(GotoPump());
ThereIsAClonkInside:
	Message("bitte manuell steuern ; %d/1000 frames +",this(),GetActTime());
	if(GreaterThan( GetActTime(),1000 ))
		return(ControlRight());
	return();
Waiting:
	// nach Crewmembers außerhalb gucken
	if( CheckLeft() ) // links daneben
		return(EndWaitingLeft2Right());
	if( CheckRight() ) // rechts daneben
		return(EndWaitingRight2Left());
	if( CheckRoom() ) // innerhalb
		return(ThereIsAClonkInside());
	//nichts zu tun
	return();

/* manuelle steuerung */
/* --control left-- */
ControlLeft:
	if( SEqual(GetAction(),"Wait") ) // macht grad nichts
		return(EndWaitingRoom2Left());
	// beschäftigt, -> Abbruch
	Message("Ohhh mach keinen Stress, bin gleich fertig...",this());
	SetLocal(0,0); // nach der aktuellen Aktion geht er zu 'Waiting'
	SetLocal(1,0);
	return();
ControlRight:
	if( SEqual(GetAction(),"Wait") )
		return(EndWaitingRoom2Right());
	Message("Ohhh mach keinen Stress, bin gleich fertig...",this());
	SetLocal(0,0);
	SetLocal(1,0);
	return();
ControlDownSingle:
	Message("stopping...",this());
	SetLocal(0,0);
	SetLocal(1,0);
	return();

/* --opened-- */
RightOpened:
	return(SetSolidMask(196,100,98,41));
LeftOpened:
	return(SetSolidMask(98,100,98,41));
/* --closed-- */
RightClosed:
	return(SetSolidMask(0,100,98,41));
LeftClosed:
	return(SetSolidMask(0,100,98,41));

/* --right/left opened-- */
GotoCloseRight:
	if(And( Equal(Local(0),1),Equal(Local(1),1) )) // eintritt von rechts
		return(SetAction("ShowEntryRight"));
	if(And( Equal(Local(0),2),Equal(Local(1),2) )) // austritt nach rechts
		return(SetAction("ShowLeaveRight"));
	return(SetAction("Wait"));
GotoCloseLeft:
	Message("Local(0)=%d Local(1)=%d",0,Local(0),Local(1));
	if(And( Equal(Local(0),1),Equal(Local(1),2) )) // austritt nach links
		return(SetAction("ShowLeaveLeft"));
	if(And( Equal(Local(0),2),Equal(Local(1),1) )) // eintritt von links
		return(SetAction("ShowEntryLeft"));
	return(SetAction("Wait"));

/* --trying to close right-- */
CheckCloseRightE:
	if(And( Equal(Local(0),0) , Equal(Local(1),0) )) // interrupted
		return(SetAction("CloseRight"));
	// sind noch Clonks draußen?
	if(And( CheckRight(),GreaterThan( GetActTime(),100 ) ))
			return(Message("Eingang freimachen / manuell",this()));
	if( CheckRight() )
		return();
	SetLocal(1,2); // ausgang, Local(0)=right2left
	return(SetAction("CloseRight"));
CheckCloseRightL:
	if(And( Equal(Local(0),0) , Equal(Local(1),0) )) // interrupted
		return(SetAction("CloseRight"));
	if(And( CheckRoom(),GreaterThan( GetActTime(),100 ) ))
		return(Message("Schleuse verlassen / manuell",this()));
	if( CheckRoom() )
		return();
	SetLocal(0,0);
	SetLocal(1,0);
	return(SetAction("CloseRight"));

/* --trying to close left-- */
CheckCloseLeftE:
	if(And( Equal(Local(0),0) , Equal(Local(1),0) )) // interrupted
		return(SetAction("CloseLeft"));
	if(And( CheckLeft(),GreaterThan( GetActTime(),100 ) ))
		return(Message("Eingang freimachen / manuell",this()));
	if( CheckLeft() )
		return();
	SetLocal(1,2);
	return(SetAction("CloseLeft"));
CheckCloseLeftL:
	if(And( Equal(Local(0),0) , Equal(Local(1),0) )) // interrupted
		return(SetAction("CloseLeft"));
	if(And( CheckRoom(),GreaterThan( GetActTime(),100 ) ))
		return(Message("Schleuse verlassen / manuell",this()));
	if( CheckRoom() )
		return();
	SetLocal(0,0);
	SetLocal(1,0);
	return(SetAction("CloseLeft"));

/* --go to pump-- */
GotoPumpRight2Left: // -> von rechts außerhalb nach room
	if(Equal( Local(2),1 )) // die Flüssigkeit ist rechts -> pumpen unsinnig
		return(GotoStopPump());
	if(Equal( Local(2),0 )) // ist rechts nicht -> überschwemmen vermeiden
		return(SetAction("PumpLeftOut"));
	return(); // tritt nie ein
GotoPumpLeft2Right: // -> links2room
	if(Equal( Local(3),1 )) // Flüssigkeit ist links da -> pumpen unsinnig
		return(GotoStopPump());
	if(Equal( Local(3),0 )) // ist links nicht da -> überschwemmung vermeiden
		return(SetAction("PumpRightOut"));
	return(); // tritt nie ein
GotoPumpRoom2Left:
	if(Equal( Local(3),1 )) // links ist flüss -> pumpen unsinnig
		return(GotoStopPump());
	if(Equal( Local(3),0 )) // links ist !flüss -> überschwemmen vermeiden
		return(SetAction("PumpRightOut"));
	return(); // tritt nie ein
GotoPumpRoom2Right:
	if(Equal( Local(2),1 )) // rechts ist flüss -> pumpen unsinnig
		return(GotoStopPump());
	if(Equal( Local(2),0 )) // rechts ist !flüss -> überschwemmen vermeiden
		return(SetAction("PumpLeftOut"));
	return(); // tritt nie ein
GotoPump: // aufgerufen, wenn igendetwas eingeleitet wird: right2left, left2right, room2right room2left
	CheckLiquid();
	if(And( Equal(Local(0),0) , Equal(Local(1),0) ))
		return(SetAction("Wait"));
	if(And( Equal( Local(0),1 ) , Equal( Local(1),1 ) )) // rechts2links
		return(GotoPumpRight2Left());
	if(And( Equal( Local(0),2 ) , Equal( Local(1),1 ) )) // links2rechts
		return(GotoPumpLeft2Right());
	if(And( Equal( Local(0),1 ) , Equal( Local(1),2 ) )) // room2left
		return(GotoPumpRoom2Left());
	if(And( Equal( Local(0),2 ) , Equal( Local(1),2 ) )) // room2right
		return(GotoPumpRoom2Right());
	return(SetAction("Wait"));

GotoStopPump: // aufgerufen, wenn pumpen beendet, fortführung aktuelle aufgabe
	if(And( Equal(Local(0),0) , Equal(Local(1),0) ))
		return(SetAction("Wait"));
	if(And( Equal( Local(0),1 ) , Equal( Local(1),1 ) )) // right2left
		return(SetAction("OpenRight"));
	if(And( Equal( Local(0),2 ) , Equal( Local(1),1 ) )) // left2right
		return(SetAction("OpenLeft"));
	if(And( Equal( Local(0),1 ) , Equal( Local(1),2 ) )) // room2left
		return(SetAction("OpenLeft"));
	if(And( Equal( Local(0),2 ) , Equal( Local(1),2 ) )) // room2right
		return(SetAction("OpenRight"));
	return(SetAction("Wait"));

/* --pumping-- */
PumpRightOut: // nur wenn Local(0)=left2right Local(1)=0 [siehe GotoPumpXXX]
	if(And( Equal(Local(0),0) , Equal(Local(1),0) ))
		return(SetAction("Wait"));
	if(RoomIsEmpty()) // alles weggepumpt
		return(GotoStopPump());
	//noch was wegpumpen
	SetVar(0,RoomMaterial());
	ExtractMaterialAmount(0,19,Var(0),1);
	InsertMaterial(Var(0),50,-16); // casted ein PXC -> evtl wiederholen falls >1 extracted
	ExtractMaterialAmount(0,19,Var(0),1);
	InsertMaterial(Var(0),50,-16); // casted ein PXC -> evtl wiederholen falls >1 extracted
	ExtractMaterialAmount(0,19,Var(0),1);
	InsertMaterial(Var(0),50,-16); // casted ein PXC -> evtl wiederholen falls >1 extracted
	ExtractMaterialAmount(0,19,Var(0),1);
	InsertMaterial(Var(0),50,-16); // casted ein PXC -> evtl wiederholen falls >1 extracted
	ExtractMaterialAmount(0,19,Var(0),1);
	InsertMaterial(Var(0),50,-16); // casted ein PXC -> evtl wiederholen falls >1 extracted
	ExtractMaterialAmount(0,19,Var(0),1);
	InsertMaterial(Var(0),50,-16); // casted ein PXC -> evtl wiederholen falls >1 extracted
	ExtractMaterialAmount(0,19,Var(0),1);
	InsertMaterial(Var(0),50,-16); // casted ein PXC -> evtl wiederholen falls >1 extracted
	ExtractMaterialAmount(0,19,Var(0),1);
	InsertMaterial(Var(0),50,-16); // casted ein PXC -> evtl wiederholen falls >1 extracted
	ExtractMaterialAmount(0,19,Var(0),1);
	InsertMaterial(Var(0),50,-16); // casted ein PXC -> evtl wiederholen falls >1 extracted
	ExtractMaterialAmount(0,19,Var(0),1);
	InsertMaterial(Var(0),50,-16); // casted ein PXC -> evtl wiederholen falls >1 extracted
	return();
PumpRightIn: // nur wenn Local(0)=left2right Local(2)=1 [siehe GotoPumpXXX]
	if(And( Equal(Local(0),0) , Equal(Local(1),0) ))
		return(SetAction("Wait"));
	if(Or( Not( RightMaterial() ) , RoomIsFilled() )) // schleuse gefüllt
		return(GotoStopPump());
	//noch was reinpumpen
	SetVar(0,RightMaterial());
	ExtractMaterialAmount(55,15,Var(0),1);
	InsertMaterial(Var(0),0,19);
	ExtractMaterialAmount(55,15,Var(0),1);
	InsertMaterial(Var(0),0,19);
	ExtractMaterialAmount(55,15,Var(0),1);
	InsertMaterial(Var(0),0,19);
	ExtractMaterialAmount(55,15,Var(0),1);
	InsertMaterial(Var(0),0,19);
	ExtractMaterialAmount(55,19,Var(0),1);
	InsertMaterial(Var(0),0,15);
	ExtractMaterialAmount(55,15,Var(0),1);
	InsertMaterial(Var(0),0,19);
	ExtractMaterialAmount(55,15,Var(0),1);
	InsertMaterial(Var(0),0,19);
	ExtractMaterialAmount(55,15,Var(0),1);
	InsertMaterial(Var(0),0,19);
	ExtractMaterialAmount(55,15,Var(0),1);
	InsertMaterial(Var(0),0,19);
	ExtractMaterialAmount(55,19,Var(0),1);
	InsertMaterial(Var(0),0,15);
	return();
PumpLeftOut: // nur wenn Local(0)=left2right Local(2)=0 [siehe GotoPumpXXX]
	if(And( Equal(Local(0),0) , Equal(Local(1),0) ))
		return(SetAction("Wait"));
	if(RoomIsEmpty()) // alles weggepumpt
		return(GotoStopPump());
	//noch was wegpumpen
	SetVar(0,RoomMaterial());
	ExtractMaterialAmount(0,19,Var(0),1);
	InsertMaterial(Var(0),-50,-16);
	ExtractMaterialAmount(0,19,Var(0),1);
	InsertMaterial(Var(0),-50,-16);
	ExtractMaterialAmount(0,19,Var(0),1);
	InsertMaterial(Var(0),-50,-16);
	ExtractMaterialAmount(0,19,Var(0),1);
	InsertMaterial(Var(0),-50,-16);
	ExtractMaterialAmount(0,19,Var(0),1);
	InsertMaterial(Var(0),-50,-16);
	ExtractMaterialAmount(0,19,Var(0),1);
	InsertMaterial(Var(0),-50,-16);
	ExtractMaterialAmount(0,19,Var(0),1);
	InsertMaterial(Var(0),-50,-16);
	ExtractMaterialAmount(0,19,Var(0),1);
	InsertMaterial(Var(0),-50,-16);
	ExtractMaterialAmount(0,19,Var(0),1);
	InsertMaterial(Var(0),-50,-16);
	ExtractMaterialAmount(0,19,Var(0),1);
	InsertMaterial(Var(0),-50,-16);
	return();
PumpLeftIn: // nur wenn Local(0)=right2left Local(3)=1 [siehe GotoPumpXXX]
	if(And( Equal(Local(0),0) , Equal(Local(1),0) ))
		return(SetAction("Wait"));
	if(Or( Not(LeftMaterial()) , RoomIsFilled() )) // schleuse gefüllt
		return(GotoStopPump());
	//noch was rein
	SetVar(0,LeftMaterial());
	ExtractMaterialAmount(-55,19,Var(0),1);
	InsertMaterial(Var(0),0,19);
	ExtractMaterialAmount(-55,19,Var(0),1);
	InsertMaterial(Var(0),0,19);
	ExtractMaterialAmount(-55,19,Var(0),1);
	InsertMaterial(Var(0),0,19);
	ExtractMaterialAmount(-55,19,Var(0),1);
	InsertMaterial(Var(0),0,19);
	ExtractMaterialAmount(-55,19,Var(0),1);
	InsertMaterial(Var(0),0,19);
	ExtractMaterialAmount(-55,19,Var(0),1);
	InsertMaterial(Var(0),0,19);
	ExtractMaterialAmount(-55,19,Var(0),1);
	InsertMaterial(Var(0),0,19);
	ExtractMaterialAmount(-55,19,Var(0),1);
	InsertMaterial(Var(0),0,19);
	ExtractMaterialAmount(-55,19,Var(0),1);
	InsertMaterial(Var(0),0,19);
	ExtractMaterialAmount(-55,19,Var(0),1);
	InsertMaterial(Var(0),0,19);
	return();