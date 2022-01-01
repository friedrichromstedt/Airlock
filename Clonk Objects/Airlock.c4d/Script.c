#strict 2

local top, bottom, left, right, centre, height;

local centreROIx, centreROIwidth;
local leftROIx, leftROIwidth;
local rightROIx, rightROIwidth;

local aim;
    local beClosed, beOpenLeft, beOpenRight, beOpen,
        beForcedClosed, beForcedOpenLeft, beForcedOpenRight, beForcedOpen;
    local waitForTransfer, doTransferRight, doTransferLeft;
    local doDryPumpingLeft, doDryPumpingRight;

local mode;
    local cautious, forced, transfer, drypumping;

private func fillingDegree(int x) {
    if (GBackLiquid(x, top)) {
        return(2);
    } else if (GBackLiquid(x, bottom)) {
        return(1);
    } else {
        return(0);
    }
}

private func getMaterial(int x) {
    if (GBackLiquid(x, top)) {
        return(GetMaterial(x, top));
    } else if (GBackLiquid(x, bottom)) {
        return(GetMaterial(x, bottom));
    }
    return(-1);
}

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
    if (fillingDegree(left) > 0 && fillingDegree(right) > 0) {
        SetAction(action);
    } else if (fillingDegree(left) == 0 && fillingDegree(right) == 0 &&
               fillingDegree(centre) == 0) {
        SetAction(action);
    } else {
        Message("Kann nicht beide Seiten zugleich oeffnen");
        aim = alt_aim;
    }
}

protected func Run() {
    var current_action = GetAction();
    if (current_action == "IdlingOpenedBoth") { RunOpenedBoth(); }
    else if (current_action == "IdlingClosedBoth") { RunClosedBoth(); }
    else if (current_action == "IdlingClosedLeft") { RunClosedLeft(); }
    else if (current_action == "IdlingClosedRight") { RunClosedRight(); }
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
        if (CountCentre() == 0) {
            if (CountRight() > 0 && CountLeft() == 0) {
                aim = doTransferRight;
            } else if (CountLeft() > 0 && CountRight() == 0) {
                aim = doTransferLeft;
            } else {
                SetAction("ShieldCloseBoth");
            }
        }
    } else if (aim == doTransferRight) {
        if (CountCentre() > 0) {
            SetAction("ShieldCloseLeftII");
        } else {
            SetAction("ShieldCloseBoth");
        }
    } else if (aim == doTransferLeft) {
        if (CountCentre() > 0) {
            SetAction("ShieldCloseRightII");
        } else {
            SetAction("ShieldCloseBoth");
        }
    }
}

private func AttemptShieldOpenLeft() {
    if (fillingDegree(centre) == 0 || fillingDegree(left) > 0) {
        SetAction("ShieldOpenLeft");
    } else {
        // We have liquid inside and a dry left side.
        if (fillingDegree(right) > 0) {
            SetAction("PumpRightOut");
        } else {
            Message("Beide Seiten sind trocken");
            aim = beClosed;
        }
    }
}

private func AttemptShieldOpenRight() {
    if (fillingDegree(centre) == 0 || fillingDegree(right) > 0) {
        SetAction("ShieldOpenRight");
    } else {
        if (fillingDegree(left) > 0) {
            SetAction("PumpLeftOut");
        } else {
            Message("Beide Seiten sind trocken");
            aim = beClosed;
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
            AttemptShieldOpenLeft();
        } else if (CountRight() > 0 && CountLeft() == 0) {
            AttemptShieldOpenRight();
        }
    } else if (aim == doTransferRight) {
        if (CountLeft() > 0) {
            AttemptShieldOpenLeft();
        } else if (CountCentre() > 0) {
            AttemptShieldOpenRight();
        } else if (CountRight() == 0) {
            // Finish doTransferRight
            aim = waitForTransfer;
        }
    } else if (aim == doTransferLeft) {
        if (CountRight() > 0) {
            AttemptShieldOpenRight();
        } else if (CountCentre() > 0) {
            AttemptShieldOpenLeft();
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
        else if (CountRight() > 0) {
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

protected func RunPumpLeftOut() {
    if (aim == beClosed) {
        SetAction("IdlingClosedBoth");
    } else if (aim == beOpenLeft) {
        SetAction("ShieldOpenLeft");
    } else if (aim == beOpenRight) {
        if (fillingDegree(centre) == 0) {
            SetAction("ShieldOpenRight");
        } else {
            for (var i = 0; i < 20; i++) {
                var material = ExtractLiquid(centre, bottom);
                if (material != -1) {
                    InsertMaterial(material, left, top);
                }
            }
        }
    } else if (aim == beOpen) {
        RequestOpenBoth("ShieldOpenBoth", beClosed);
    }

    if (mode == forced) {
        SetAction("IdlingClosedBoth");
    }
}

protected func RunPumpRightOut() {
    if (aim == beClosed) {
        SetAction("IdlingClosedBoth");
    } else if (aim == beOpenLeft) {
        if (fillingDegree(centre) == 0) {
            SetAction("ShieldOpenLeft");
        } else {
            for (var i = 0; i < 20; i++) {
                var material = ExtractLiquid(centre, bottom);
                if (material != -1) {
                    InsertMaterial(material, right, top);
                }
            }
        }
    } else if (aim == beOpen) {
        RequestOpenBoth("ShieldOpenBoth", beClosed);
    }

    if (mode == forced) {
        SetAction("IdlingClosedBoth");
    }
}

protected func RunPumpLeftIn() { }
protected func RunPumpRightIn() { }

protected func RunWalkRightOpenLeft() { }
protected func RunWalkRightOpenRight() { }
protected func RunWalkLeftOpenLeft() { }
protected func RunWalkLeftOpenRight() { }

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
    top = -10;
    bottom = 11;
    left = -80;
    right = 80;
    centre = 0;
    height = 22;

    centreROIx = -78;
    centreROIwidth = 157;
    leftROIx = -227;
    leftROIwidth = 160;
    rightROIx = 67;
    rightROIwidth = 160;

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
    CreateMenu(ALCK, pCaller, 0, 0, "Linke Seite", 0, 1);
    if (mode == forced || mode == cautious) {
        AddMenuItem("Links offen", "ControlOpenLeft", ALCK, pCaller);
        AddMenuItem("Links geschlossen", "ControlCloseLeft", ALCK, pCaller);
    }
}

protected func ControlRight(object pCaller) {
    CreateMenu(ALCK, pCaller, 0, 0, "Rechte Seite", 0, 1);
    if (mode == forced || mode == cautious) {
        AddMenuItem("Rechts offen", "ControlOpenRight", ALCK, pCaller);
        AddMenuItem("Rechts geschlossen", "ControlCloseRight", ALCK, pCaller);
    }
}

protected func ControlUp(object pCaller) {
    CreateMenu(ALCK, pCaller, 0, 0, "Beide Seiten", 0, 1);
    if (mode == forced || mode == cautious) {
        AddMenuItem("Beide Seiten offen", "ControlOpenBoth", ALCK, pCaller);
        AddMenuItem("Beide Seiten geschlossen", "ControlCloseBoth", ALCK, pCaller);
    }
}

protected func ControlDig(object pCaller) {
    CreateMenu(ALCK, pCaller, 0, 0, "Betriebsmodus", 0, 1);
    AddMenuItem("Vorsichtig", "ControlModeCautious", ALCK, pCaller);
    AddMenuItem("Forciert", "ControlModeForced", ALCK, pCaller);
    // AddMenuItem("Transfer", "ControlModeTransfer", ALCK, pCaller);
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
    } else if (mode == drypumping) {
    }
    mode = cautious;
}

protected func ControlModeForced() {
    if (mode == cautious) {
        if (aim == beClosed) { aim = beForcedOpen; }
        else if (aim == beOpenLeft) { aim = beForcedOpenLeft; }
        else if (aim == beOpenRight) { aim = beForcedOpenRight; }
        else if (aim == beOpen) { aim = beForcedOpen; }
    } else if (mode == transfer) {
    } else if (mode == drypumping) {
    }
    mode = forced;
}

protected func SolidMaskClosedBoth() {
    SetSolidMask(0, 52, 173, 26, 0, -2);
}

protected func SolidMaskOpenedBoth() {
    SetSolidMask(0, 130, 173, 26, 0, -2);
}

protected func SolidMaskClosedLeft() {
    SetSolidMask(0, 78, 173, 26, 0, -2);
}

protected func SolidMaskClosedRight() {
    SetSolidMask(0, 104, 173, 26, 0, -2);
}
