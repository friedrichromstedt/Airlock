#strict 2

local top, bottom, left, right, centre, height;

local centreROIx, centreROIwidth;
local leftROIx, leftROIwidth;
local rightROIx, rightROIwidth;

local maxFloorXOffset;

local aim;
    local beClosed, beOpenLeft, beOpenRight, beOpen,
        beForcedClosed, beForcedOpenLeft, beForcedOpenRight, beForcedOpen;
    local waitForTransfer,
        enterFromLeft, exitToRight, enterFromRight, exitToLeft;
    local doDryPumpingLeft, doDryPumpingRight;

local mode;
    local cautious, forced, transfer, drypumping;

local actionAfterWaiting;

private func Suspend(string resume_action) {
    actionAfterWaiting = resume_action;
    SetAction("Waiting");
    Log("Waiting");
}

protected func Resume() {
    SetAction(actionAfterWaiting);
}

private func FillingDegree(int x) {
    if (GBackLiquid(x, top)) {
        return(3);
    } else if (GBackLiquid(x, bottom - 1)) {
        return(2);
    } else if (GBackLiquid(x, bottom)) {
        return(1);
    } else {
        return(0);
    }
}

private func FirstWetOffset() {
    for (var xoffset = 0; xoffset <= maxFloorXOffset; ++xoffset) {
        if (FillingDegree(xoffset) > 0 || FillingDegree(-xoffset) > 0) {
            return(xoffset);
        }
    }
    return(-1);
}

/*
private func getMaterial(int x) {
    if (GBackLiquid(x, top)) {
        return(GetMaterial(x, top));
    } else if (GBackLiquid(x, bottom)) {
        return(GetMaterial(x, bottom));
    }
    return(-1);
}
*/

private func CountLeft() {
    return ObjectCount2(
        Find_InRect(leftROIx, top, leftROIwidth, height),
        Find_OCF(OCF_CrewMember),
    );
}
private func CountCentre() {
    return ObjectCount2(
        Find_InRect(centreROIx, top, centreROIwidth, height),
        Find_OCF(OCF_CrewMember),
    );
}
private func CountRight() {
    return ObjectCount2(
        Find_InRect(rightROIx, top, rightROIwidth, height),
        Find_OCF(OCF_CrewMember),
    );
}

private func RequestOpenBoth(string action, int alt_aim) {
    if (FillingDegree(left) > 0 && FillingDegree(right) > 0) {
        SetAction(action);
    } else if (FillingDegree(left) == 0 && FillingDegree(right) == 0 &&
               FillingDegree(centre) == 0) {
        SetAction(action);
    } else {
        Message("Kann nicht beide Seiten zugleich oeffnen", this);
        aim = alt_aim;
    }
}

// local lastFillingDegree;

protected func Run() {
    var current_action = GetAction();

    if (current_action == "IdlingOpenedBoth") { RunOpenedBoth(); }
    else if (current_action == "IdlingClosedBoth") { RunClosedBoth(); }
    else if (current_action == "IdlingClosedLeft") { RunClosedLeft(); }
    else if (current_action == "IdlingClosedRight") { RunClosedRight(); }

    /*
    var filling_degree = FillingDegree(centre);
    if (filling_degree != lastFillingDegree) {
        Log("%d: FillingDegree(centre) now %d (%s)",
            FrameCounter(), filling_degree, current_action);
    }
    lastFillingDegree = filling_degree;
    */
}

private func RunOpenedBoth() {
    if (aim == beClosed) {
        SetAction("ShieldCloseBoth");
    } else if (aim == beOpenLeft) {
        SetAction("ShieldCloseRightII");
    } else if (aim == beOpenRight) {
        SetAction("ShieldCloseLeftII");
    } else if (aim == beOpen) {
        // nothing to do
    }

    if (aim == beForcedClosed) {
        SetAction("ShieldCloseBoth");
    } else if (aim == beForcedOpenLeft) {
        SetAction("ShieldCloseRightII");
    } else if (aim == beForcedOpenRight) {
        SetAction("ShieldCloseLeftII");
    } else if (aim == beForcedOpen) {
        // nothing to do
    }

    // in transfer mode, we need to leave the OpenedBoth state.
    if (aim == waitForTransfer) {
        // In OpenedBoth state, we wait for the clonk/s which built the
        // AirLock to exit on one of the two sides.
        if (CountCentre() == 0) {
            if (CountRight() > 0 && CountLeft() == 0) {
                // Let the clonks exit to the right.
                aim = exitToRight;
            } else if (CountLeft() > 0 && CountRight() == 0) {
                // Let the clonks exit to the left.
                aim = exitToLeft;
            } else {
                SetAction("ShieldCloseBoth");
            }
        }
    } else if (aim == enterFromLeft) {
        // When aiming at entering from the left, the right shield can be
        // closed.
        SetAction("ShieldCloseRightII");
    } else if (aim == exitToRight) {
        // When aiming at exiting to the right, the left shield can be
        // closed.
        SetAction("ShieldCloseLeftII");
    } else if (aim == enterFromRight) {
        SetAction("ShieldCloseLeftII");
    } else if (aim == exitToLeft) {
        SetAction("ShieldCloseRightII");
    }
}

private func AttemptShieldOpenLeft(int impossibleAim) {
    if (FillingDegree(centre) == 0 || FillingDegree(left) > 0) {
        SetAction("ShieldOpenLeft");
    } else {
        // We have liquid inside and a dry left side.
        if (FillingDegree(right) > 0) {
            SetAction("PumpRightOut");
        } else {
            Message("Beide Seiten sind trocken");
            aim = impossibleAim;
        }
    }
}

private func AttemptShieldOpenRight(int impossibleAim) {
    if (FillingDegree(centre) == 0 || FillingDegree(right) > 0) {
        SetAction("ShieldOpenRight");
    } else {
        // Log("FillingDegree(centre) = %d", FillingDegree(centre));
        if (FillingDegree(left) > 0) {
            SetAction("PumpLeftOut");
        } else {
            Message("Beide Seiten sind trocken");
            aim = impossibleAim;
        }
    }
}

private func RunClosedBoth() {
    if (aim == beClosed) {
        // nothing to do
    } else if (aim == beOpenLeft) {
        AttemptShieldOpenLeft();
    } else if (aim == beOpenRight) {
        AttemptShieldOpenRight();
    } else if (aim == beOpen) {
        RequestOpenBoth("ShieldOpenBoth", beClosed);
    }

    if (aim == beForcedClosed) {
        // nothing to do
    } else if (aim == beForcedOpenLeft) {
        SetAction("ShieldOpenLeft");
    } else if (aim == beForcedOpenRight) {
        SetAction("ShieldOpenRight");
    } else if (aim == beForcedOpen) {
        SetAction("ShieldOpenBoth");
    }

    if (aim == waitForTransfer) {
        if (CountLeft() > 0 && CountRight() == 0) {
            aim = enterFromLeft;
        } else if (CountRight() > 0 && CountLeft() == 0) {
            aim = enterFromRight;
        }
    } else if (aim == enterFromLeft) {
        if (CountLeft() > 0) {
            AttempShieldOpenLeft(waitForTransfer);
        } else {
            // There's nobody to enter from the left.
            aim = waitForTransfer;
        }
    } else if (aim == exitToRight) {
        if (CountCentre() > 0) {
            AttemptShieldOpenRight(waitForTransfer);
        } else if (CountRight() == 0) {
            // The passenger left the airlock's region.
            aim = waitForTransfer;
        }
        // Otherwise (CountCentre() == 0 && CountRight() > 0) stay in the
        // present state.
    } else if (aim == enterFromRight) {
        if (CountRight() > 0) {
            AttemptShieldOpenRight(waitForTransfer);
        } else {
            aim = waitForTransfer;
        }
    } else if (aim == exitToLeft) {
        if (CountCentre() > 0) {
            AttemptShieldOpenLeft(waitForTransfer);
        } else if (CountLeft() == 0) {
            aim = waitForTransfer;
        }
    }
}

private func RunClosedLeft() {
    if (aim == beClosed) {
        SetAction("ShieldCloseRight");
    } else if (aim == beOpenLeft) {
        SetAction("ShieldCloseRight"); // the first step
    } else if (aim == beOpenRight) {
        // nothing to do
    } else if (aim == beOpen) {
        RequestOpenBoth("ShieldOpenLeftII", beOpenRight);
    }

    if (aim == beForcedClosed) {
        SetAction("ShieldCloseRight");
    } else if (aim == beForcedOpenLeft) {
        SetAction("ShieldCloseRight");
    } else if (aim == beForcedOpenRight) {
        // nothing to do
    } else if (aim == beForcedOpen) {
        SetAction("ShieldOpenLeftII");
    }

    if (aim == waitForTransfer) {
        if (CountRight() > 0 && CountLeft() == 0) {
            aim = enterFromRight;
        } else if (CountLeft() > 0 && CountRight() == 0) {
            aim = enterFromLeft;
        }
        // Otherwise keep waiting; the user might influence the aim
        // manually too.
    } else if (aim == enterFromLeft) {
        if (CountLeft() > 0) {
            // Passengers are present.
            SetAction("ShieldCloseRight");  // first step
        } else {
            // No passenger present.
            aim = waitForTransfer;
        }
    } else if (aim == exitToRight) {
        if (CountCentre() > 0) {
            // Invite the passengers to leave the airlock.
            SetAction("WalkRightOpenRight");
        } else if (CountRight() == 0) {
            // All passengers left the airlock's region.
            aim = waitForTransfer;
        }
    } else if (aim == enterFromRight) {
        if (CountRight() > 0) {
            // There are passengers, invite them.
            SetAction("WalkLeftOpenRight");
        } else if (CountCentre() > 0) {
            // All passengers entered the airlock.
            aim = exitToLeft;
        } else {
            aim = waitForTransfer;
        }
    } else if (aim == exitToLeft) {
        if (CountCentre() > 0) {
            // There are passengers.
            SetAction("ShieldCloseRight");
        } else {
            // There are no passengers.
            aim = waitForTransfer;
        }
    }
}

private func RunClosedRight() {
    if (aim == beClosed) {
        SetAction("ShieldCloseLeft");
    } else if (aim == beOpenLeft) {
        // nothing to do
    } else if (aim == beOpenRight) {
        SetAction("ShieldCloseLeft"); // the first step
    } else if (aim == beOpen) {
        RequestOpenBoth("ShieldOpenRightII", beOpenLeft);
    }

    if (aim == beForcedClosed) {
        SetAction("ShieldCloseLeft");
    } else if (aim == beForcedOpenLeft) {
        // nothing to do
    } else if (aim == beForcedOpenRight) {
        SetAction("ShieldCloseLeft");
    } else if (aim == beForcedOpen) {
        SetAction("ShieldOpenRightII");
    }

    if (aim == waitForTransfer) {
        if (CountLeft() > 0 && CountRight() == 0) {
            aim = enterFromLeft;
        } else if (CountRight() > 0 && CountLeft() == 0) {
            aim = enterFromRight;
        }
    } else if (aim == enterFromLeft) {
        if (CountLeft() > 0) {
            SetAction("WalkRightOpenLeft");
        } else if (CountCentre() > 0) {
            aim = exitToRight;
        } else {
            aim = waitFortransfer;
        }
    } else if (aim == exitToRight) {
        if (CountCentre() > 0) {
            SetAction("ShieldCloseLeft");
        } else {
            aim = waitForTransfer;
        }
    } else if (aim == enterFromRight) {
        if (CountRight() > 0) {
            SetAction("ShieldCloseLeft");
        } else {
            aim = waitForTransfer;
        }
    } else if (aim == exitToLeft) {
        if (CountCenter() > 0) {
            SetAction("WalkLeftOpenLeft");
        } else if (CountLeft() == 0) {
            aim = waitForTransfer;
        }
    }
}

/*
private func PumpOnePxLeftOut(int x) {
    var material = ExtractLiquid(x, bottom);
    if (material != -1) {
        InsertMaterial(material, left, top);
        return(1);
    } else {
        return(0);
    }
}
*/

private func PumpOut(int amount, int target) {
    // *target* is the x position of the outlet.
    var xoffset = 0, material, pumpedVolume = 0;
    for (var i = 0; i < amount; i++) {
        xoffset = FirstWetOffset();
        if (xoffset != -1) {
            if (FillingDegree(xoffset) > 0) {
                material = ExtractLiquid(xoffset, bottom);
                InsertMaterial(material, target, top);
                pumpedVolume += 1;
            }
            if (FillingDegree(-xoffset) > 0) {
                material = ExtractLiquid(-xoffset, bottom);
                InsertMaterial(material, target, top);
                pumpedVolume += 1;
            }
        }
    }
    return(pumpedVolume);
}

// local stabilisation_countdown;

protected func RunPumpLeftOut() {
    if (aim == beClosed) {
        SetAction("IdlingClosedBoth");
    } else if (aim == beOpenLeft) {
        SetAction("IdlingClosedBoth");
    } else if (aim == beOpenRight) {
        if (PumpOut(10, left) == 0) {
            SetAction("IdlingClosedBoth");
        }
    } else if (aim == beOpen) {
        RequestOpenBoth("ShieldOpenBoth", beClosed);
    }

    if (aim == waitForTransfer) {
        SetAction("IdlingClosedBoth");
    } else if (aim == enterFromLeft) {
        // There's no reason to pump to the left when aiming at entering
        // the airlock from the left.
        SetAction("IdlingClosedBoth");
    } else if (aim == exitToRight) {
        if (PumpOut(10, left) == 0) {
            SetAction("IdlingClosedBoth");
        }
    } else if (aim == enterFromRight) {
        if (PumpOut(10, left) == 0) {
            SetAction("IdlingClosedBoth");
        }
    } else if (aim == exitToLeft) {
        // There's no reason to pump to the left when aiming at leaving the
        // airlock to the left.
        SetAction("IdlingClosedBoth");
    }

    if (mode == forced) {
        SetAction("IdlingClosedBoth");
    }
}

protected func RunPumpRightOut() {
    if (aim == beClosed) {
        SetAction("IdlingClosedBoth");
    } else if (aim == beOpenLeft) {
        if (PumpOut(10, right) == 0) {
            SetAction("IdlingClosedBoth");
        }
    } else if (aim == beOpen) {
        RequestOpenBoth("ShieldOpenBoth", beClosed);
    }

    if (aim == waitForTransfer) {
        SetAction("IdlingClosedBoth");
    } else if (aim == enterFromLeft) {
        if (PumpOut(10, right) == 0) {
            SetAction("IdlingClosedBoth");
        }
    } else if (aim == exitToRight) {
        SetAction("IdlingClosedBoth");
    } else if (aim == enterFromRight) {
        SetAction("IdlingClosedBoth");
    } else if (aim == exitToLeft) {
        if (PumpOut(10, right) == 0) {
            SetAction("IdlingClosedBoth");
        }
    }

    if (mode == forced) {
        SetAction("IdlingClosedBoth");
    }
}

protected func RunPumpLeftIn() { }
protected func RunPumpRightIn() { }

protected func RunWalkRightOpenLeft() {
    if (aim != enterFromLeft || CountLeft() == 0) {
        SetAction("IdlingClosedRight");
    }
}
protected func RunWalkRightOpenRight() {
    if (aim != exitToRight || CountCentre() == 0) {
        SetAction("IdlingClosedLeft");
    }
}
protected func RunWalkLeftOpenLeft() {
    if (aim != exitToLeft || CountCentre() == 0) {
        SetAction("IdlingClosedRight");
    }
}
protected func RunWalkLeftOpenRight() {
    if (aim != enterFromRight || CountRight() == 0) {
        SetAction("IdlingClosedLeft");
    }
}

/*
protected func EndCallFunction() {
    if (aim == beForcedClosed) {
        SetAction("");
    } else if (aim == beForcedOpenLeft) {
        SetAction("");
    } else if (aim == beForcedOpenRight) {
        SetAction("");
    } else if (aim == beForcedOpen) {
        SetAction("");
    }
}
*/

protected func Initialize() {
    top = -9;
    bottom = 11;
    left = -80;
    right = 80;
    centre = 0;
    height = 22;

    centreROIx = -78;
    centreROIwidth = 157;

    /*
    // Width of outer regions: 160 px.
    leftROIx = -227;
    leftROIwidth = 160;
    rightROIx = 67;
    rightROIwidth = 160;
    */

    // Width of outer regions: 50 px.
    leftROIx = -117;
    leftROIwidth = 50;
    rightROIx = 67;
    rightROIwidth = 50;

    maxFloorXOffset = 68;

    beClosed = 1;
    beOpenLeft = 2;
    beOpenRight = 3;
    beOpen = 4;

    doDryPumpingLeft = 6;
    doDryPumpingRight = 7;

    beForcedClosed = 11;
    beForcedOpenLeft = 8;
    beForcedOpenRight = 9;
    beForcedOpen = 10;

    waitForTransfer = 13;
    enterFromLeft = 15;
    exitToRight = 16;
    enterFromRight = 17;
    exitToLeft = 18;

    cautious = 1;
    forced = 2;
    transfer = 3;
    drypumping = 4;

    SetAction("IdlingOpenedBoth");
    aim = waitForTransfer;
    mode = transfer;
}

protected func ControlLeft(object pCaller) {
    if (mode == forced || mode == cautious) {
        CreateMenu(ALCK, pCaller, 0, 0, "Linke Seite", 0, 1);
        AddMenuItem("Links offen", "ControlOpenLeft", ALCK, pCaller);
        AddMenuItem("Links geschlossen", "ControlCloseLeft", ALCK, pCaller);
    } else if (mode == transfer) {
        if (CountRight() > 0) {
            aim = enterFromRight;
        } else {
            // We can skip the case that *no* passenger is present since
            // someone actually issued this callback.
            aim = exitToLeft;
            // In case of exit via the left shield, it will be attempted to
            // be opened.  When about to leave the airlock's region to the
            // left, nothing will be done.  The latter case is especially
            // useful when the airlock shall not open the left shield,
            // assuming that the passenger requests transfer.
        }
    }
}

protected func ControlRight(object pCaller) {
    if (mode == forced || mode == cautious) {
        CreateMenu(ALCK, pCaller, 0, 0, "Rechte Seite", 0, 1);
        AddMenuItem("Rechts offen", "ControlOpenRight", ALCK, pCaller);
        AddMenuItem("Rechts geschlossen", "ControlCloseRight", ALCK, pCaller);
    } else if (mode == transfer) {
        if (CountLeft() > 0) {
            aim = enterFromLeft;
        } else {
            aim = exitToRight;
        }
    }
}

protected func ControlUp(object pCaller) {
    if (mode == forced || mode == cautious) {
        CreateMenu(ALCK, pCaller, 0, 0, "Beide Seiten", 0, 1);
        AddMenuItem("Beide Seiten offen", "ControlOpenBoth", ALCK, pCaller);
        AddMenuItem("Beide Seiten geschlossen", "ControlCloseBoth", ALCK, pCaller);
    } else if (mode == transfer) {
        // This can be used to cancel all transfer currently in operation.
        aim = waitForTransfer;
    }
}

protected func ControlDig(object pCaller) {
    CreateMenu(ALCK, pCaller, 0, 0, "Betriebsmodus", 0, 1);
    AddMenuItem("Vorsichtig", "ControlModeCautious", ALCK, pCaller);
    AddMenuItem("Forciert", "ControlModeForced", ALCK, pCaller);
    AddMenuItem("Transfer", "ControlModeTransfer", ALCK, pCaller);
    // AddMenuItem("Trockenpumpen", "ControlModeDrypumping", ALCK, pCaller);
}

protected func ControlOpenLeft() {
    if (aim == beClosed) {
        aim = beOpenLeft;
    } else if (aim == beOpenRight) {
        aim = beOpen;
    }

    if (aim == beForcedClosed) {
        aim = beForcedOpenLeft;
    } else if (aim == beForcedOpenRight) {
        aim = beForcedOpen;
    }
}

protected func ControlCloseLeft() {
    if (aim == beOpenLeft) {
        aim = beClosed;
    } else if (aim == beOpen) {
        aim = beOpenRight;
    }

    if (aim == beForcedOpenLeft) {
        aim = beForcedClosed;
    } else if (aim == beForcedOpen) {
        aim = beForcedOpenRight;
    }
}

protected func ControlOpenRight() {
    if (aim == beClosed) {
        aim = beOpenRight;
    } else if (aim == beOpenLeft) {
        aim = beOpen;
    }

    if (aim == beForcedClosed) {
        aim = beForcedOpenRight;
    } else if (aim == beForcedOpenLeft) {
        aim = beForcedOpen;
    }
}

protected func ControlCloseRight() {
    if (aim == beOpenRight) {
        aim = beClosed;
    } else if (aim == beOpen) {
        aim = beOpenLeft;
    }

    if (aim == beForcedOpenRight) {
        aim = beForcedClosed;
    } else if (aim == beForcedOpen) {
        aim = beForcedOpenLeft;
    }
}

protected func ControlOpenBoth() {
    if (mode == cautious) { aim = beOpen; }
    if (mode == forced) { aim = beForcedOpen; }
}

protected func ControlCloseBoth() {
    if (mode == cautious) { aim = beClosed; }
    if (mode == forced) { aim = beForcedClosed; }
}

protected func ControlModeCautious() {
    if (mode == forced) {
        if (aim == beForcedClosed) { aim = beClosed; }
        else if (aim == beForcedOpenLeft) { aim = beOpenLeft; }
        else if (aim == beForcedOpenRight) { aim = beOpenRight; }
        else if (aim == beForcedOpen) { aim = beOpen; }
    } else if (mode == transfer) {
        aim = beClosed;
    } else if (mode == drypumping) {
    }
    mode = cautious;
}

protected func ControlModeForced() {
    if (mode == cautious) {
        if (aim == beClosed) { aim = beForcedClosed; }
        else if (aim == beOpenLeft) { aim = beForcedOpenLeft; }
        else if (aim == beOpenRight) { aim = beForcedOpenRight; }
        else if (aim == beOpen) { aim = beForcedOpen; }
    } else if (mode == transfer) {
        aim = beForcedClosed;
    } else if (mode == drypumping) {
    }
    mode = forced;
}

protected func ControlModeTransfer() {
    aim = waitForTransfer; // this will maintain the current Action
    mode = transfer;
}

protected func SolidMaskClosedBoth() {
    // Log("both closed");
    SetSolidMask(0, 52, 173, 26, 0, -2);
}

protected func SolidMaskOpenedBoth() {
    // Log("both opened");
    SetSolidMask(0, 130, 173, 26, 0, -2);
}

protected func SolidMaskClosedLeft() {
    // Log("left closed, right open");
    SetSolidMask(0, 78, 173, 26, 0, -2);
}

protected func SolidMaskClosedRight() {
    // Log("left open, right closed");
    SetSolidMask(0, 104, 173, 26, 0, -2);
}
