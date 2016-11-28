package com.mopub.nativeads;

public enum NativeErrorCode {
    EMPTY_AD_RESPONSE("Server returned empty response."),
    INVALID_JSON("Unable to parse JSON response from server."),
    IMAGE_DOWNLOAD_FAILURE("Unable to download images associated with ad."),
    INVALID_REQUEST_URL("Invalid request url."),
    UNEXPECTED_RESPONSE_CODE("Received unexpected response code from server."),
    SERVER_ERROR_RESPONSE_CODE("Server returned erroneous response code."),
    CONNECTION_ERROR("Network is unavailable."),
    UNSPECIFIED("Unspecified error occurred.");

    private final String message;

    private NativeErrorCode(String message) {
        this.message = message;
    }

    @Override
    public final String toString() {
        return message;
    }
}
