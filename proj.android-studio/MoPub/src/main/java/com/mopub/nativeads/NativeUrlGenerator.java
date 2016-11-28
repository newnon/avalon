package com.mopub.nativeads;

import android.content.Context;
import android.location.Location;
import android.os.Build;

import com.mopub.common.AdUrlGenerator;
import com.mopub.common.GpsHelper;
import com.mopub.common.LocationService;
import com.mopub.common.util.DeviceUtils;
import com.mopub.common.MoPub;

class NativeUrlGenerator extends AdUrlGenerator {
    private static int sLocationPrecision = 6;
    private static LocationService.LocationAwareness sLocationAwareness
            = LocationService.LocationAwareness.NORMAL;

    NativeUrlGenerator(Context context) {
        super(context);
    }

    @Override
    public NativeUrlGenerator withAdUnitId(final String adUnitId) {
        mAdUnitId = adUnitId;
        return this;
    }

    NativeUrlGenerator withRequest(final RequestParameters requestParameters) {
        if (requestParameters != null) {
            mKeywords = requestParameters.getKeywords();
            mLocation = requestParameters.getLocation();
        }
        return this;
    }

    @Override
    public String generateUrlString(final String serverHostname) {
        initUrlString(serverHostname, Constants.NATIVE_HANDLER);

        setAdUnitId(mAdUnitId);

        setSdkVersion(MoPub.SDK_VERSION);

        setDeviceInfo(Build.MANUFACTURER, Build.MODEL, Build.PRODUCT);

        setUdid(getUdidFromContext(mContext));

        setDoNotTrack(GpsHelper.isLimitAdTrackingEnabled(mContext));

        setKeywords(mKeywords);

        Location location = mLocation;
        if (location == null) {
            location = LocationService.getLastKnownLocation(mContext,
                                                            sLocationPrecision,
                                                            sLocationAwareness);
        }

        setLocation(location);

        setTimezone(getTimeZoneOffsetString());

        setOrientation(mContext.getResources().getConfiguration().orientation);

        setDensity(mContext.getResources().getDisplayMetrics().density);

        String networkOperator = getNetworkOperator();
        setMccCode(networkOperator);
        setMncCode(networkOperator);

        setIsoCountryCode(mTelephonyManager.getNetworkCountryIso());
        setCarrierName(mTelephonyManager.getNetworkOperatorName());

        setNetworkType(getActiveNetworkType());

        setAppVersion(getAppVersionFromContext(mContext));

        setTwitterAppInstalledFlag();

        return getFinalUrlString();
    }

    @Override
    protected void setSdkVersion(String sdkVersion) {
        addParam("nsv", sdkVersion);
    }
}
