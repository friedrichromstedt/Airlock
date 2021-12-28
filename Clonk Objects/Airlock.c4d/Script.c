#strict 2

local top, bottom, left, right, centre;

local aim;
    local beClosed, beOpenLeft, beOpenRight, beOpen,
        beForcedClosed, beForcedOpenLeft, beForcedOpenRight, beForcedOpen;
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
    } else if (aim == beOpenRight) {
    } else if (aim == beOpen) {
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
}

private func RunClosedBoth() {
    if (aim == beForcedClosed) {
        // nothing to do
    } else if (aim == beForcedOpenLeft) {
        SetAction("ShieldOpenLeft");
    } else if (aim == beForcedOpenRight) {
        SetAction("ShieldOpenRight");
    } else if (aim == beForcedOpen) {
        SetAction("ShieldOpenBoth");
    }
}

private func RunClosedLeft() {
    if (aim == beForcedClosed) {
        SetAction("ShieldCloseRight");
    } else if (aim == beForcedOpenLeft) {
        SetAction("ShieldCloseRight");
    } else if (aim == beForcedOpenRight) {
        // nothing to do
    } else if (aim == beForcedOpen) {
        SetAction("ShieldOpenLeftII");
    }
}

private func RunClosedRight() {
    if (aim == beForcedClosed) {
        SetAction("ShieldCloseLeft");
    } else if (aim == beForcedOpenLeft) {
        // nothing to do
    } else if (aim == beForcedOpenRight) {
        SetAction("ShieldCloseLeft");
    } else if (aim == beForcedOpen) {
        SetAction("ShieldOpenRightII");
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
    top = -10;
    bottom = 11;
    left = -80;
    right = 80;
    centre = 0;

    beClosed = 1;
    beOpenLeft = 2;
    beOpenRight = 3;
    beOpen = 4;
    doTransfer = 5;
    doDryPumpingLeft = 6;
    doDryPumpingRight = 7;
    beForcedClosed = 11;
    beForcedOpenLeft = 8;
    beForcedOpenRight = 9;
    beForcedOpen = 10;

    cautious = 1;
    forced = 2;
    transfer = 3;
    drypumping = 4;

    SetAction("IdlingOpenedBoth");
    aim = beForcedOpen;
    mode = forced;
}

protected func ControlLeft(object pCaller) {
    CreateMenu(ALCK, pCaller, 0, 0, "Linke Seite", 0, 1);
    if (mode == forced) {
        AddMenuItem("Links offen", "ControlOpenLeft", ALCK, pCaller);
        AddMenuItem("Links geschlossen", "ControlCloseLeft", ALCK, pCaller);
    }
}

protected func ControlRight(object pCaller) {
    CreateMenu(ALCK, pCaller, 0, 0, "Rechte Seite", 0, 1);
    if (mode == forced) {
        AddMenuItem("Rechts offen", "ControlOpenRight", ALCK, pCaller);
        AddMenuItem("Rechts geschlossen", "ControlCloseRight", ALCK, pCaller);
    }
}

protected func ControlUp(object pCaller) {
    CreateMenu(ALCK, pCaller, 0, 0, "Beide Seiten", 0, 1);
    if (mode == forced) {
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
    if (aim == beForcedClosed) {
        aim = beForcedOpenLeft;
    } else if (aim == beForcedOpenRight) {
        aim = beForcedOpen;
    }
}

protected func ControlCloseLeft() {
    if (aim == beForcedOpenLeft) {
        aim = beForcedClosed;
    } else if (aim == beForcedOpen) {
        aim = beForcedOpenRight;
    }
}

protected func ControlOpenRight() {
    if (aim == beForcedClosed) {
        aim = beForcedOpenRight;
    } else if (aim == beForcedOpenLeft) {
        aim = beForcedOpen;
    }
}

protected func ControlCloseRight() {
    if (aim == beForcedOpenRight) {
        aim = beForcedClosed;
    } else if (aim == beForcedOpen) {
        aim = beForcedOpenLeft;
    }
}

protected func ControlOpenBoth() {
    if (mode == forced) { aim = beForcedOpen; }
}

protected func ControlCloseBoth() {
    if (mode == forced) { aim = beForcedClosed; }
}

protected func ControlModeCautious() {
    if (mode == forced) {
        if (aim == byForcedClosed) { aim = beClosed; }
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
