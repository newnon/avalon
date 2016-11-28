package com.mopub.nativeads;

import android.widget.ImageView;

import com.mopub.common.util.MoPubLog;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;

import static com.mopub.nativeads.NativeResponse.Parameter.CALL_TO_ACTION;
import static com.mopub.nativeads.NativeResponse.Parameter.CLICK_DESTINATION;
import static com.mopub.nativeads.NativeResponse.Parameter.CLICK_TRACKER;
import static com.mopub.nativeads.NativeResponse.Parameter.ICON_IMAGE;
import static com.mopub.nativeads.NativeResponse.Parameter.IMPRESSION_TRACKER;
import static com.mopub.nativeads.NativeResponse.Parameter.MAIN_IMAGE;
import static com.mopub.nativeads.NativeResponse.Parameter.TEXT;
import static com.mopub.nativeads.NativeResponse.Parameter.TITLE;
import static com.mopub.nativeads.NativeResponse.Parameter.isImageKey;
import static java.util.Map.Entry;

public final class NativeResponse {
    enum Parameter {
        IMPRESSION_TRACKER("imptracker", true),
        CLICK_TRACKER("clktracker", true),

        TITLE("title", false),
        TEXT("text", false),
        MAIN_IMAGE("mainimage", false),
        ICON_IMAGE("iconimage", false),

        CLICK_DESTINATION("clk", false),
        FALLBACK("fallback", false),
        CALL_TO_ACTION("ctatext", false),
        STAR_RATING("starrating", false);

        private final String name;
        private final boolean required;

        Parameter(final String name, boolean required) {
            this.name = name;
            this.required = required;
        }

        static Parameter from(final String name) {
            for (final Parameter parameter : Parameter.values()) {
                if (parameter.name.equals(name)) {
                    return parameter;
                }
            }

            return null;
        }

        static boolean isImageKey(final String name) {
            return name != null && name.toLowerCase().endsWith("image");
        }

        static Set<String> requiredKeys = new HashSet<String>();
        static {
            for (final Parameter parameter : Parameter.values()) {
                if (parameter.required) {
                    requiredKeys.add(parameter.name);
                }
            }
        }
    }

    private String mMainImageUrl;
    private String mIconImageUrl;
    private List<String> mImpressionTrackers;
    private String mClickTracker;
    private String mClickDestinationUrl;
    private String mCallToAction;
    private String mTitle;
    private String mText;
    private boolean mRecordedImpression;
    private final Map<String, Object> mExtras;
    private boolean mIsDestroyed;

    NativeResponse(final JSONObject jsonObject) throws IllegalArgumentException {
        mExtras = new HashMap<String, Object>();
        mImpressionTrackers = new ArrayList<String>();

        if (!containsRequiredKeys(jsonObject)) {
            throw new IllegalArgumentException("JSONObject did not contain required keys.");
        }

        Iterator<String> keys = jsonObject.keys();
        while (keys.hasNext()) {
            final String key = keys.next();
            final Parameter parameter = Parameter.from(key);

            if (parameter != null) {
                try {
                    addInstanceVariable(parameter, jsonObject.opt(key));
                } catch (ClassCastException e) {
                    throw new IllegalArgumentException("JSONObject key (" + key + ") contained unexpected value.");
                }
            } else {
                addExtra(key, jsonObject.opt(key));
            }
        }
    }

    @Override
    public String toString() {
        StringBuilder stringBuilder = new StringBuilder();

        stringBuilder.append(TITLE.name).append(":").append(mTitle).append("\n");
        stringBuilder.append(TEXT.name).append(":").append(mText).append("\n");
        stringBuilder.append(ICON_IMAGE.name).append(":").append(mIconImageUrl).append("\n");
        stringBuilder.append(MAIN_IMAGE.name).append(":").append(mMainImageUrl).append("\n");
        stringBuilder.append(IMPRESSION_TRACKER.name).append(":").append(mImpressionTrackers).append("\n");
        stringBuilder.append(CLICK_TRACKER.name).append(":").append(mClickTracker).append("\n");
        stringBuilder.append(CLICK_DESTINATION.name).append(":").append(mClickDestinationUrl).append("\n");
        stringBuilder.append(CALL_TO_ACTION.name).append(":").append(mCallToAction).append("\n");
        stringBuilder.append("recordedImpression").append(":").append(mRecordedImpression).append("\n");
        stringBuilder.append("extras").append(":").append(mExtras);

        return stringBuilder.toString();
    }

    public void destroy() {
        mIsDestroyed = true;
        mExtras.clear();
    }

    /**
     * Getters
     */
    public String getMainImageUrl() {
        return mMainImageUrl;
    }

    public void loadMainImage(final ImageView imageView) {
        loadImageView(mMainImageUrl, imageView);
    }

    public String getIconImageUrl() {
        return mIconImageUrl;
    }

    public void loadIconImage(final ImageView imageView) {
        loadImageView(mIconImageUrl, imageView);
    }

    public List<String> getImpressionTrackers() {
        return mImpressionTrackers;
    }

    public String getClickTracker() {
        return mClickTracker;
    }

    public String getClickDestinationUrl() {
        return mClickDestinationUrl;
    }

    public String getCallToAction() {
        return mCallToAction;
    }

    public String getTitle() {
        return mTitle;
    }

    public String getSubtitle() {
        return mText;
    }

    public boolean getRecordedImpression() {
        return mRecordedImpression;
    }

    public Object getExtra(final String key) {
        return mExtras.get(key);
    }

    public Map<String, Object> getExtras() {
        return new HashMap<String, Object>(mExtras);
    }

    public void loadExtrasImage(final String key, final ImageView imageView) {
        Object object = mExtras.get(key);
        if (object != null && object instanceof String) {
            final String imageUrl = (String) mExtras.get(key);
            loadImageView(imageUrl, imageView);
        }
    }

    private void loadImageView(final String url, final ImageView imageView) {
        ImageViewService.loadImageView(url, imageView);
    }

    public boolean isDestroyed() {
        return mIsDestroyed;
    }

    List<String> getExtrasImageUrls() {
        final List<String> extrasBitmapUrls = new ArrayList<String>(mExtras.size());

        for (final Entry<String, Object> entry : mExtras.entrySet()) {
            if (isImageKey(entry.getKey()) && entry.getValue() instanceof String) {
                extrasBitmapUrls.add((String) entry.getValue());
            }
        }

        return extrasBitmapUrls;
    }

    List<String> getAllImageUrls() {
        final List<String> imageUrls = new ArrayList<String>();
        if (mMainImageUrl != null) {
            imageUrls.add(mMainImageUrl);
        }
        if (mIconImageUrl != null) {
            imageUrls.add(mIconImageUrl);
        }

        imageUrls.addAll(getExtrasImageUrls());
        return imageUrls;
    }

    /**
     * Setters
     */

    void recordImpression() {
        mRecordedImpression = true;
    }

    private boolean containsRequiredKeys(final JSONObject jsonObject) {
        final Set<String> keys = new HashSet<String>();

        final Iterator<String> jsonKeys = jsonObject.keys();
        while (jsonKeys.hasNext()) {
            keys.add(jsonKeys.next());
        }

        return keys.containsAll(Parameter.requiredKeys);
    }

    private void addInstanceVariable(final Parameter key, final Object value) throws ClassCastException {
        try {
            switch (key) {
                case MAIN_IMAGE:
                    mMainImageUrl = (String) value;
                    break;
                case ICON_IMAGE:
                    mIconImageUrl = (String) value;
                    break;
                case IMPRESSION_TRACKER:
                    addImpressionTrackers(value);
                    break;
                case CLICK_TRACKER:
                    mClickTracker = (String) value;
                    break;
                case CLICK_DESTINATION:
                    mClickDestinationUrl = (String) value;
                    break;
                case CALL_TO_ACTION:
                    mCallToAction = (String) value;
                    break;
                case TITLE:
                    mTitle = (String) value;
                    break;
                case TEXT:
                    mText = (String) value;
                    break;
                default:
                    MoPubLog.d("Unable to add JSON key to internal mapping: " + key.name);
                    break;
            }
        } catch (ClassCastException e) {
            if (!key.required) {
                MoPubLog.d("Ignoring class cast exception for optional defined key: " + key.name);
            } else {
                throw e;
            }
        }
    }

    private void addExtra(final String key, final Object value) {
        mExtras.put(key, value);
    }

    private void addImpressionTrackers(final Object impressionTrackers) throws ClassCastException {
        if (!(impressionTrackers instanceof JSONArray)) {
            throw new ClassCastException("Expected impression trackers of type JSONArray.");
        }

        final JSONArray trackers = (JSONArray) impressionTrackers;

        for (int i = 0; i < trackers.length(); i++) {
            try {
                mImpressionTrackers.add(trackers.getString(i));
            } catch (JSONException e) {
                // This will only occur if we access a non-existent index in JSONArray.
                MoPubLog.d("Unable to parse impression trackers.");
            }
        }
    }
}
