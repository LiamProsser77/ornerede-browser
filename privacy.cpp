#include "privacy.h"

bool Privacy::trackingEnabled() {
    return false;
}

bool Privacy::analyticsEnabled() {
    return false;
}

bool Privacy::advertisingEnabled() {
    return false;
}

bool Privacy::historyEnabled() {
    return false;
}

bool Privacy::privateMode() {
    return true;
}
