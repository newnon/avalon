package com.mopub.nativeads;

import android.location.Location;

public final class RequestParameters {
    private final String mKeywords;
    private final Location mLocation;

    public final static class Builder {
        private String keywords;
        private Location location;

        public final Builder keywords(String keywords) {
            this.keywords = keywords;
            return this;
        }

        public final Builder location(Location location) {
            this.location = location;
            return this;
        }

        public final RequestParameters build() {
            return new RequestParameters(this);
        }
    }

    private RequestParameters(Builder builder) {
        mKeywords = builder.keywords;
        mLocation = builder.location;
    }

    public final String getKeywords() {
        return mKeywords;
    }

    public final Location getLocation() {
        return mLocation;
    }
}
