#strict 2

local top, bottom, left, right;

local aim;
    local doPowerOff;
    local doClose, doOpenLeft, doOpenRight, doOpenBoth,
        doForcedOpenLeft, doForcedOpenRight, doForcedOpenBoth;
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

func Initialize() {
    top = -10;
    bottom = 11;
    left = -80;
    right = 80;

    doPowerOff = 0;
    doClose = 1;
    doOpenLeft = 2;
    doOpenRight = 3;
    doOpenBoth = 4;
    doTransfer = 5;
    doDryPumpLeft = 6;
    doDryPumpRight = 7;
    doForcedOpenLeft = 8;
    doForcedOpenRight = 9;
    doForcedOpenBoth = 10;

    SetAction("Off");
}
