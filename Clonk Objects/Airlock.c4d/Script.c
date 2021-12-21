#strict 2

local top, bottom, left, right;

local aim;
    local beClosed, beOpenLeft, beOpenRight, beOpen,
        beForcedClosed, beForcedOpenLeft, beForcedOpenRight, beForcedOpen;
    local doTransfer;
    local doDryPumpingLeft, doDryPumpingRight;

func fillingDegree(int x) {
    if (GBackLiquid(x, top)) {
        return(2);
    } else if (GBackLiquid(x, bottom)) {
        return(1);
    } else {
        return(0);
    }
}

func getMaterial(int x) {
    if (GBackLiquid(x, top)) {
        return(GetMaterial(x, top));
    } else if (GBackLiquid(x, bottom)) {
        return(GetMaterial(x, bottom));
    }
    return(-1);
}

func Run() {
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

func EndCallFunction() {
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

func ShieldOpenBothDone() {
    if (aim == beForcedClosed) {
        SetAction("ShieldCloseBoth");
    } else if (aim == beForcedOpenLeft) {
        SetAction("ShieldCloseRightII");
    } else if (aim == beForcedOpenRight) {
        SetAction("ShieldcloseLeftII");
    } else if (aim == beForcedOpen) {
        SetAction("IdlingOpenBoth");
    }
}

func ShieldCloseBothDone() {
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

func ShieldOpenLeftDone() {
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

func ShieldCloseLeftDone() {
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

func ShieldOpenRightDone() {
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

func ShieldCloseRightDone() {
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

func ShieldOpenLeftIIDone() {
    if (aim == beForcedClosed) {
        SetAction("ShieldCloseBoth");
    } else if (aim == beForcedOpenLeft) {
        SetAction("ShieldCloseRightII");
    } else if (aim == beForcedOpenRight) {
        SetAction("ShieldCloseLeftII");
    } else if (aim == beForcedOpen) {
        SetAction("IdlingOpenBoth");
    }
}

func ShieldCloseLeftIIDone() {
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

func ShieldOpenRightIIDone() {
    if (aim == beForcedClosed) {
        SetAction("ShieldCloseBoth");
    } else if (aim == beForcedOpenLeft) {
        SetAction("ShieldCloseRightII");
    } else if (aim == beForcedOpenRight) {
        SetAction("ShieldCloseLeftII");
    } else if (aim == beForcedOpen) {
        SetAction("IdlingOpenBoth");
    }
}

func ShieldCloseRightIIDone() {
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

func Initialize() {
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

    SetAction("IdlingOpenBoth");
    aim = beForcedOpen;
}
