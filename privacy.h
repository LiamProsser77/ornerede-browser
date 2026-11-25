#ifndef ORNEREDE_PRIVACY_H
#define ORNEREDE_PRIVACY_H

class Privacy {
public:
    static bool trackingEnabled();
    static bool analyticsEnabled();
    static bool advertisingEnabled();
    static bool historyEnabled();

    static bool privateMode();
};

#endif
