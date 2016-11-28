package com.mopub.nativeads;

import java.util.HashMap;
import java.util.Map;

public final class ViewBinder {
    public final static class Builder {
        private final int layoutId;
        private int titleId;
        private int textId;
        private int callToActionId;
        private int mainImageId;
        private int iconImageId;
        private Map<String, Integer> extras;

        public Builder(final int layoutId) {
            this.layoutId = layoutId;
            this.extras = new HashMap<String, Integer>();
        }

        public final Builder titleId(final int titleId) {
            this.titleId = titleId;
            return this;
        }

        public final Builder textId(final int textId) {
            this.textId = textId;
            return this;
        }

        public final Builder callToActionId(final int callToActionId) {
            this.callToActionId = callToActionId;
            return this;
        }

        public final Builder mainImageId(final int mainImageId) {
            this.mainImageId = mainImageId;
            return this;
        }

        public final Builder iconImageId(final int iconImageId) {
            this.iconImageId = iconImageId;
            return this;
        }

        public final Builder addExtras(final Map<String, Integer> resourceIds) {
            this.extras = new HashMap<String, Integer>(resourceIds);
            return this;
        }

        public final Builder addExtra(final String key, final int resourceId) {
            this.extras.put(key, resourceId);
            return this;
        }

        public final ViewBinder build() {
            return new ViewBinder(this);
        }
    }

    final int layoutId;
    final int titleId;
    final int textId;
    final int callToActionId;
    final int mainImageId;
    final int iconImageId;
    final Map<String, Integer> extras;

    private ViewBinder(final Builder builder) {
        this.layoutId = builder.layoutId;
        this.titleId = builder.titleId;
        this.textId = builder.textId;
        this.callToActionId = builder.callToActionId;
        this.mainImageId = builder.mainImageId;
        this.iconImageId = builder.iconImageId;
        this.extras = builder.extras;
    }
}
