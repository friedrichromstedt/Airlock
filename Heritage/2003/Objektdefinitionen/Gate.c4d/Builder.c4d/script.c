/*
Locals:
0 - n mal bestätigt

*/

Initialize:
	SetLocal(0,0);
	return();

Warn:
	Message("bestätigen mit noch mal 'aktivate'",this());
	SetLocal(0,Sum(Local(0),1));
	return();

Build:
	Message("BUILDING...");
//	DigFreeRect(Sub(GetX(),55),Sub(GetY(),31),104,41);
//	CreateConstruction(ALCK,0,10);
	CreateObject(ALCK,0,10);
	return(RemoveObject());
	return();

Activate:
	if(LessThan( Local(0),1 ))
		return( Warn() );
	return(Build());
