#strict 2

local top, bottom, left, right, centre, height;

local centreROIx, centreROIwidth;
local leftROIx, leftROIwidth;
local rightROIx, rightROIwidth;

local maxFloorXOffset;

local aim;
    local beClosed, beOpenLeft, beOpenRight, beOpen,
        beForcedClosed, beForcedOpenLeft, beForcedOpenRight, beForcedOpen;
    local waitForTransfer, doTransferRight, doTransferLeft;
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

    if (aim == waitForTransfer) {
        // In OpenedBoth state, we wait for the clonk/s which built the
        // AirLock to exit on one of the two sides.
        if (CountCentre() == 0) {
            if (CountRight() > 0 && CountLeft() == 0) {
                // Let the clonks exit to the right.
                aim = doTransferRight;
            } else if (CountLeft() > 0 && CountRight() == 0) {
                // Let the clonks exit to the left.
                aim = doTransferLeft;
            } else {
                SetAction("ShieldCloseBoth");
            }
        }
    } else if (aim == doTransferRight) {
        if (CountCentre() > 0) {
            // Aim set manually.
            SetAction("ShieldCloseLeftII");
        } else {
            // The builder/s exited.
            SetAction("ShieldCloseBoth");
        }
    } else if (aim == doTransferLeft) {
        if (CountCentre() > 0) {
            // Aim set manually.
            SetAction("ShieldCloseRightII");
        } else {
            // The builder/s exited.
            SetAction("ShieldCloseBoth");
        }
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
            aim = doTransferRight;
        } else if (CountRight() > 0 && CountLeft() == 0) {
            aim = doTransferLeft;
        }
    } else if (aim == doTransferRight) {
        if (CountLeft() > 0) {
            AttemptShieldOpenLeft(waitForTransfer);
        } else if (CountCentre() > 0) {
            /*
            if (FillingDegree(centre) == 0) {
                Suspend("ShieldOpenRight");
            } else {
                AttemptShieldOpenRight();
            }
            */
            AttemptShieldOpenRight(waitForTransfer);
        } else if (CountRight() == 0) {
            // Finish doTransferRight
            aim = waitForTransfer;
        }
    } else if (aim == doTransferLeft) {
        if (CountRight() > 0) {
            AttemptShieldOpenRight(waitForTransfer);
        } else if (CountCentre() > 0) {
            AttemptShieldOpenLeft(waitForTransfer);
        } else if (CountLeft() == 0) {
            // finish doTransferLeft
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
            aim = doTransferLeft;
        } else if (CountLeft() > 0 && CountRight() == 0) {
            SetAction("ShieldCloseRight"); // first step
        }
    } else if (aim == doTransferLeft) {
        if (CountRight() > 0) {
            SetAction("WalkLeftOpenRight");
        } else if (CountCentre() > 0) {
            SetAction("ShieldCloseRight");
        } else {
            aim = waitForTransfer;
        }
    } else if (aim == doTransferRight) {
        if (CountCentre() > 0) {
            SetAction("WalkRightOpenRight");
        } else if (CountRight() > 0) {
            SetAction("ShieldCloseRight");
        } else {
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
            aim = doTransferRight;
        } else if (CountRight() > 0 && CountLeft() == 0) {
            SetAction("ShieldCloseLeft"); // first step
        }
    } else if (aim == doTransferRight) {
        if (CountLeft() > 0) {
            SetAction("WalkRightOpenLeft");
        } else if (CountCentre() > 0) {
            SetAction("ShieldCloseLeft");
        } else {
            aim = waitForTransfer;
        }
    } else if (aim == doTransferLeft) {
        if (CountCentre() > 0) {
            SetAction("WalkLeftOpenLeft");
        } else if (CountLeft() > 0) {
            SetAction("ShieldCloseLeft");
        } else {
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
    var pumpedVolume;

    if (aim == beClosed) {
        SetAction("IdlingClosedBoth");
    } else if (aim == beOpenLeft) {
        SetAction("IdlingClosedBoth");
    } else if (aim == beOpenRight) {
        pumpedVolume = PumpOut(10, left);
        if (pumpedVolume == 0) {
            SetAction("IdlingClosedBoth");
        }
    } else if (aim == beOpen) {
        RequestOpenBoth("ShieldOpenBoth", beClosed);
    }

    // In transfer mode, PumpLeftOut is always done to be able to open the
    // right shield.
    if (aim == doTransferLeft || aim == doTransferRight) {
        pumpedVolume = PumpOut(10, left);
        if (pumpedVolume == 0) {
            SetAction("IdlingClosedBoth");
        }
    } else if (aim == waitForTransfer) {
        // In case the aim has been modified while pumping was in effect.
        SetAction("IdlingClosedBoth");
    }

    if (mode == forced) {
        SetAction("IdlingClosedBoth");
    }
}

protected func RunPumpRightOut() {
    var pumpedVolume;
    if (aim == beClosed) {
        SetAction("IdlingClosedBoth");
    } else if (aim == beOpenLeft) {
        pumpedVolume = PumpOut(10, right);
        if (pumpedVolume == 0) {
            SetAction("IdlingClosedBoth");
        }
    } else if (aim == beOpen) {
        RequestOpenBoth("ShieldOpenBoth", beClosed);
    }

    if (aim == doTransferLeft || aim == doTransferRight) {
        pumpedVolume = PumpOut(10, right);
        if (pumpedVolume == 0) {
            SetAction("IdlingClosedBoth");
        }
    } else if (aim == waitForTransfer) {
        SetAction("IdlingClosedBoth");
    }

    if (mode == forced) {
        SetAction("IdlingClosedBoth");
    }
}

protected func RunPumpLeftIn() { }
protected func RunPumpRightIn() { }

protected func RunWalkRightOpenLeft() {
    if (CountLeft() == 0) { SetAction("IdlingClosedRight"); }
    if (aim != doTransferRight) { SetAction("IdlingClosedRight"); }
}
protected func RunWalkRightOpenRight() {
    if (CountCentre() == 0) { SetAction("IdlingClosedLeft"); }
    if (aim != doTransferRight) { SetAction("IdlingClosedLeft"); }
}
protected func RunWalkLeftOpenLeft() {
    if (CountCentre() == 0 ) { SetAction("IdlingClosedRight"); }
    if (aim != doTransferLeft) { SetAction("IdlingClosedRight"); }
}
protected func RunWalkLeftOpenRight() {
    if (CountRight() == 0) { SetAction("IdlingClosedLeft"); }
    if (aim != doTransferLeft) { SetAction("IdlingClosedLeft"); }
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

    doTransferRight = 14;
    doTransferLeft = 12;
    waitForTransfer = 13;

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
        aim = doTransferLeft;
    }
}

protected func ControlRight(object pCaller) {
    if (mode == forced || mode == cautious) {
        CreateMenu(ALCK, pCaller, 0, 0, "Rechte Seite", 0, 1);
        AddMenuItem("Rechts offen", "ControlOpenRight", ALCK, pCaller);
        AddMenuItem("Rechts geschlossen", "ControlCloseRight", ALCK, pCaller);
    } else if (mode == transfer) {
        aim = doTransferRight;
    }
}

protected func ControlUp(object pCaller) {
    if (mode == forced || mode == cautious) {
        CreateMenu(ALCK, pCaller, 0, 0, "Beide Seiten", 0, 1);
        AddMenuItem("Beide Seiten offen", "ControlOpenBoth", ALCK, pCaller);
        AddMenuItem("Beide Seiten geschlossen", "ControlCloseBoth", ALCK, pCaller);
    } else if (mode == transfer) {
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
