package com.mopub.common;

import android.content.Context;
import android.location.Location;
import android.location.LocationManager;

import com.mopub.common.util.MoPubLog;

import java.math.BigDecimal;

public class LocationService {
    public static enum LocationAwareness { NORMAL, TRUNCATED, DISABLED };

    /*
     * Returns the last known location of the device using its GPS and network location providers.
     * May be null if:
     * - Location permissions are not requested in the Android manifest file
     * - The location providers don't exist
     * - Location awareness is disabled in the parent MoPubView
     */
    public static Location getLastKnownLocation(final Context context,
                                                final int locationPrecision,
                                                final LocationAwareness locationAwareness) {
        Location result;

        if (locationAwareness == LocationAwareness.DISABLED) {
            return null;
        }

        LocationManager locationManager = (LocationManager) context.getSystemService(Context.LOCATION_SERVICE);
        Location gpsLocation = null;
        try {
            gpsLocation = locationManager.getLastKnownLocation(LocationManager.GPS_PROVIDER);
        } catch (SecurityException e) {
            MoPubLog.d("Failed to retrieve GPS location: access appears to be disabled.");
        } catch (IllegalArgumentException e) {
            MoPubLog.d("Failed to retrieve GPS location: device has no GPS provider.");
        }

        Location networkLocation = null;
        try {
            networkLocation = locationManager.getLastKnownLocation(LocationManager.NETWORK_PROVIDER);
        } catch (SecurityException e) {
            MoPubLog.d("Failed to retrieve network location: access appears to be disabled.");
        } catch (IllegalArgumentException e) {
            MoPubLog.d("Failed to retrieve network location: device has no network provider.");
        }

        if (gpsLocation == null && networkLocation == null) {
            return null;
        }
        else if (gpsLocation != null && networkLocation != null) {
            if (gpsLocation.getTime() > networkLocation.getTime()) result = gpsLocation;
            else result = networkLocation;
        }
        else if (gpsLocation != null) result = gpsLocation;
        else result = networkLocation;

        // Truncate latitude/longitude to the number of digits specified by locationPrecision.
        if (locationAwareness == LocationAwareness.TRUNCATED) {
            double lat = result.getLatitude();
            double truncatedLat = BigDecimal.valueOf(lat)
                    .setScale(locationPrecision, BigDecimal.ROUND_HALF_DOWN)
                    .doubleValue();
            result.setLatitude(truncatedLat);

            double lon = result.getLongitude();
            double truncatedLon = BigDecimal.valueOf(lon)
                    .setScale(locationPrecision, BigDecimal.ROUND_HALF_DOWN)
                    .doubleValue();
            result.setLongitude(truncatedLon);
        }

        return result;
    }
}
