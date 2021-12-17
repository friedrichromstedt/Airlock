#strict 2

local top, bottom, left, right;

local aim;
    local bePoweredOff;
    local beClosed, beOpenLeft, beOpenRight, beOpen,
        beForcedOpenLeft, beForcedOpenRight, beForcedOpen;
    local doTransfer;
    local doDryPumpLeft, doDryPumpRight;

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
    if (current_action == "Off") {
        if (aim == beClosed) {
            SetAction("PowerOn");
        } else if (aim == beOpenLeft) {
            SetAction("ShieldCloseRightII");
        } else if (aim == beOpenRight) {
            SetAction("ShieldCloseLeftII");
        }
    } else if (current_action == "Idling") {
        if (aim == bePoweredOff) {
            SetAction("PowerOff");
        } else if (aim == beOpenLeft) {
            SetAction("ShieldOpenLeft");
        } else if (aim == beOpenRigth) {
            SetAction("ShieldOpenRight");
        } else if (aim == beOpen) {
            SetAction("PowerOff");
        }
    } else if (current_action == "IdlingClosedLeft") {
        if (aim == bePoweredOff) {
            SetAction("ShieldOpenLeftII");
        } else if (aim == beClosed) {
            SetAction("ShieldCloseRight");
        } else if (aim == beOpenLeft) {
            SetAction("ShieldCloseRight");
        } else if (aim == beOpen) {
            SetAction("ShieldOpenLeftII");
        }
    } else if (current_action == "IdlingClosedRight") {
        if (aim == bePoweredOff) {
            SetAction("ShieldOpenRightII");
        } else if (aim == beClosed) {
            SetAction("ShieldCloseLeft");
        } else if (aim == beOpenRight) {
            SetAction("ShieldCloseLeft");
        } else if (aim == beOpen) {
            SetAction("ShieldOpenRightII");
        }
    }
}

func PowerOnDone() {
    if (aim == bePoweredOff) {
        SetAction("PowerOff");
    } else if (aim == beClosed) {
        SetAction("Idling");
    } else if (aim == beOpenLeft) {
        SetAction("ShieldOpenLeft");
    } else if (aim == beOpenRight) {
        SetAction("ShieldOpenRight");
    } else if (aim == beOpen) {
        SetAction("PowerOff");
    }
}

func Initialize() {
    top = -10;
    bottom = 11;
    left = -80;
    right = 80;

    bePoweredOff = 0;
    beClosed = 1;
    beOpenLeft = 2;
    beOpenRight = 3;
    beOpen = 4;
    doTransfer = 5;
    doDryPumpLeft = 6;
    doDryPumpRight = 7;
    beForcedOpenLeft = 8;
    beForcedOpenRight = 9;
    beForcedOpen = 10;

    SetAction("Off");
    aim = bePoweredOff;
}
