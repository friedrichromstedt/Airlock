#strict 2

local top, bottom, left, right;

local aim;
    local beClosed, beOpenLeft, beOpenRight, beOpen,
        beForcedClosed, beForcedOpenLeft, beForcedOpenRight, beForcedOpen;
    local doTransfer;
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
    if (current_action == "IdlingOpenedBoth") {
        if (aim == beForcedClosed) {
            SetAction("ShieldCloseBoth");
        } else if (aim == beForcedOpenLeft) {
            SetAction("ShieldCloseRightII");
        } else if (aim == beForcedOpenRight) {
            SetAction("ShieldCloseLeftII");
        } else if (aim == beForcedOpen) {
            // nothing to do
        }
    } else if (current_action == "IdlingClosedBoth") {
        if (aim == beForcedClosed) {
            // alright
        } else if (aim == beForcedOpenLeft) {
            SetAction("ShieldOpenLeft");
        } else if (aim == beForcedOpenRight) {
            SetAction("ShieldOpenRight");
        } else if (aim == beForcedOpen) {
            SetAction("ShieldOpenBoth");
        }
    } else if (current_action == "IdlingClosedLeft") {
        if (aim == beForcedClosed) {
            SetAction("ShieldCloseRight");
        } else if (aim == beForcedOpenLeft) {
            SetAction("ShieldCloseRight");
        } else if (aim == beForcedOpenRight) {
            // nothing to do
        } else if (aim == beForcedOpen) {
            SetAction("ShieldOpenLeftII");
        }
    } else if (current_action == "IdlingClosedRight") {
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

protected func ShieldOpenBothDone() {
    if (aim == beForcedClosed) {
        SetAction("ShieldCloseBoth");
    } else if (aim == beForcedOpenLeft) {
        SetAction("ShieldCloseRightII");
    } else if (aim == beForcedOpenRight) {
        SetAction("ShieldcloseLeftII");
    } else if (aim == beForcedOpen) {
        SetAction("IdlingOpenedBoth");
    }
}

protected func ShieldCloseBothDone() {
    if (aim == beForcedClosed) {
        SetAction("IdlingClosedBoth");
    } else if (aim == beForcedOpenLeft) {
        SetAction("ShieldOpenLeft");
    } else if (aim == beForcedOpenRight) {
        SetAction("ShieldOpenRight");
    } else if (aim == beForcedOpen) {
        SetAction("ShieldOpenBoth");
    }
}

protected func ShieldOpenLeftDone() {
    if (aim == beForcedClosed) {
        SetAction("ShieldCloseLeft");
    } else if (aim == beForcedOpenLeft) {
        SetAction("IdlingClosedRight");
    } else if (aim == beForcedOpenRight) {
        SetAction("ShieldCloseLeft");
    } else if (aim == beForcedOpen) {
        SetAction("ShieldOpenRightII");
    }
}

protected func ShieldCloseLeftDone() {
    if (aim == beForcedClosed) {
        SetAction("IdlingClosedBoth");
    } else if (aim == beForcedOpenLeft) {
        SetAction("ShieldOpenLeft");
    } else if (aim == beForcedOpenRight) {
        SetAction("ShieldOpenRight");
    } else if (aim == beForcedOpen) {
        SetAction("ShieldOpenBoth");
    }
}

protected func ShieldOpenRightDone() {
    if (aim == beForcedClosed) {
        SetAction("ShieldCloseRight");
    } else if (aim == beForcedOpenLeft) {
        SetAction("ShieldCloseRight");
    } else if (aim == beForcedOpenRight) {
        SetAction("IdlingClosedLeft");
    } else if (aim == beForcedOpen) {
        SetAction("ShieldOpenLeftII");
    }
}

protected func ShieldCloseRightDone() {
    if (aim == beForcedClosed) {
        SetAction("IdlingClosedBoth");
    } else if (aim == beForcedOpenLeft) {
        SetAction("ShieldOpenLeft");
    } else if (aim == beForcedOpenRight) {
        SetAction("ShieldOpenRight");
    } else if (aim == beForcedOpen) {
        SetAction("ShieldOpenBoth");
    }
}

protected func ShieldOpenLeftIIDone() {
    if (aim == beForcedClosed) {
        SetAction("ShieldCloseBoth");
    } else if (aim == beForcedOpenLeft) {
        SetAction("ShieldCloseRightII");
    } else if (aim == beForcedOpenRight) {
        SetAction("ShieldCloseLeftII");
    } else if (aim == beForcedOpen) {
        SetAction("IdlingOpenedBoth");
    }
}

protected func ShieldCloseLeftIIDone() {
    if (aim == beForcedClosed) {
        SetAction("ShieldCloseRight");
    } else if (aim == beForcedOpenLeft) {
        SetAction("ShieldCloseRight");
    } else if (aim == beForcedOpenRight) {
        SetAction("IdlingClosedLeft");
    } else if (aim == beForcedOpen) {
        SetAction("ShieldOpenLeftII");
    }
}

protected func ShieldOpenRightIIDone() {
    if (aim == beForcedClosed) {
        SetAction("ShieldCloseBoth");
    } else if (aim == beForcedOpenLeft) {
        SetAction("ShieldCloseRightII");
    } else if (aim == beForcedOpenRight) {
        SetAction("ShieldCloseLeftII");
    } else if (aim == beForcedOpen) {
        SetAction("IdlingOpenedBoth");
    }
}

protected func ShieldCloseRightIIDone() {
    if (aim == beForcedClosed) {
        SetAction("ShieldCloseLeft");
    } else if (aim == beForcedOpenLeft) {
        SetAction("IdlingClosedRight");
    } else if (aim == beForcedOpenRight) {
        SetAction("ShieldCloseLeft");
    } else if (aim == beForcedOpen) {
        SetAction("ShieldOpenRightII");
    }
}

protected func Initialize() {
    top = -10;
    bottom = 11;
    left = -80;
    right = 80;

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
    CreateMenu(ALCK, pCaller, 0, 0, "Linke Seite");
    if (mode == forced) {
        AddMenuItem("Links offen", "ControlOpenLeft", ALCK, pCaller);
        AddMenuItem("Links geschlossen", "ControlCloseLeft", ALCK, pCaller);
    }
}

protected func ControlRight(object pCaller) {
    CreateMenu(ALCK, pCaller, 0, 0, "Rechte Seite");
    if (mode == forced) {
        AddMenuItem("Rechts offen", "ControlOpenRight", ALCK, pCaller);
        AddMenuItem("Rechts geschlossen", "ControlCloseRight", ALCK, pCaller);
    }
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
