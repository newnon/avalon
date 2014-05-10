package com.avalon.GoogleAnalytics;

import java.lang.Thread.UncaughtExceptionHandler;
import java.util.HashMap;
import java.util.HashSet;

import org.cocos2dx.lib.Cocos2dxHelper;

import com.google.android.gms.analytics.ExceptionReporter;
import com.google.android.gms.analytics.GoogleAnalytics;
import com.google.android.gms.analytics.HitBuilders;
import com.google.android.gms.analytics.Logger;
import com.google.android.gms.analytics.Tracker;

import android.app.Activity;
import android.util.SparseArray;

public abstract class GoogleAnalyticsHelper
{
	private static int _dispatchPeriod = 1800;
	private static final Activity activity = Cocos2dxHelper.getActivity();
	private static final GoogleAnalytics _analytics = GoogleAnalytics.getInstance(activity);
	private static HashMap<String,Tracker> _trackers = new HashMap<String,Tracker>();
	private static double _sampleRate = 100.0;
	private static UncaughtExceptionHandler _uncaughtExceptionHandler = null;
	private static UncaughtExceptionHandler _defaultExceptionHandler = null;
	private static HashMap<Tracker,SparseArray<String>> _customDimensions = new HashMap<Tracker,SparseArray<String>>();
	private static HashMap<Tracker,SparseArray<Float>> _customMetrics = new HashMap<Tracker,SparseArray<Float>>();
	private static HashSet<Tracker> _newSessions = new HashSet<Tracker>();
	
	final private static int kGAILogLevelNone = 0;
	final private static int kGAILogLevelError = 1;
	final private static int kGAILogLevelWarning = 2;
	final private static int kGAILogLevelInfo = 3;
	final private static int kGAILogLevelVerbose = 4;
	
	
	public static void setParameter(Tracker tracker, String name, String value)
    {
		tracker.set(name, value);
    }

	public static String getParameter(Tracker tracker, String name)
    {
		return tracker.get(name);
    }

	public static void setSampleRate(Tracker tracker, float value)
    {
		_sampleRate = value;
		tracker.setSampleRate(value);
    }

    public static float getSampleRate(Tracker tracker)
    {
    	return (float) _sampleRate;
    }
    
    public static void setScreenName(Tracker tracker, String name)
    {
    	tracker.setScreenName(name);
    }
    
    public static void setCustomDimension(Tracker tracker, int index, String value)
    { 
    	SparseArray<String> dimensions = _customDimensions.get(tracker);
    	if(dimensions == null)
    	{
    		dimensions = new SparseArray<String>();
    		_customDimensions.put(tracker, dimensions);
    	}
    	dimensions.put(index, value);
    }
    public static void setCustomMetric(Tracker tracker, int index, float value)
    {
    	SparseArray<Float> metrics = _customMetrics.get(tracker);
    	if(metrics == null)
    	{
    		metrics = new SparseArray<Float>();
    		_customMetrics.put(tracker, metrics);
    	}
    	metrics.put(index, value);
    }
    
    public static void setNewSession(Tracker tracker)
    {
    	_newSessions.add(tracker);
    }
    
    public static void sendAppView(Tracker tracker)
    {
    	HitBuilders.AppViewBuilder builder = new HitBuilders.AppViewBuilder();
    	SparseArray<String> dimensions = _customDimensions.get(tracker);
    	if(dimensions != null)
    	{
    		for(int i = 0; i < dimensions.size(); i++) 
    			builder.setCustomDimension(dimensions.keyAt(i), dimensions.valueAt(i));
    		_customDimensions.remove(tracker);
    	}
    	SparseArray<Float> metrics = _customMetrics.get(tracker);
    	if(metrics != null)
    	{
    		for(int i = 0; i < metrics.size(); i++) 
    			builder.setCustomMetric(metrics.keyAt(i), metrics.valueAt(i));
    		_customMetrics.remove(tracker);
    	}
    	if(_newSessions.contains(tracker))
    	{
    		builder.setNewSession();
    		_newSessions.remove(tracker);
    	}
    		
    	tracker.send(builder.build());
    }

    public static void sendEvent(Tracker tracker, String category, String action, String label, long value)
    {
    	HitBuilders.EventBuilder builder = new HitBuilders.EventBuilder();
    	SparseArray<String> dimensions = _customDimensions.get(tracker);
    	if(dimensions != null)
    	{
    		for(int i = 0; i < dimensions.size(); i++) 
    			builder.setCustomDimension(dimensions.keyAt(i), dimensions.valueAt(i));
    		_customDimensions.remove(tracker);
    	}
    	SparseArray<Float> metrics = _customMetrics.get(tracker);
    	if(metrics != null)
    	{
    		for(int i = 0; i < metrics.size(); i++) 
    			builder.setCustomMetric(metrics.keyAt(i), metrics.valueAt(i));
    		_customMetrics.remove(tracker);
    	}
    	if(_newSessions.contains(tracker))
    	{
    		builder.setNewSession();
    		_newSessions.remove(tracker);
    	}
    	tracker.send(builder.setCategory(category)
        .setAction(action)
        .setLabel(label)
        .setValue(value)
        .build());
    }

    public static void sendException(Tracker tracker, String description, boolean fatal)
    {
    	HitBuilders.ExceptionBuilder builder = new HitBuilders.ExceptionBuilder();
    	SparseArray<String> dimensions = _customDimensions.get(tracker);
    	if(dimensions != null)
    	{
    		for(int i = 0; i < dimensions.size(); i++) 
    			builder.setCustomDimension(dimensions.keyAt(i), dimensions.valueAt(i));
    		_customDimensions.remove(tracker);
    	}
    	SparseArray<Float> metrics = _customMetrics.get(tracker);
    	if(metrics != null)
    	{
    		for(int i = 0; i < metrics.size(); i++) 
    			builder.setCustomMetric(metrics.keyAt(i), metrics.valueAt(i));
    		_customMetrics.remove(tracker);
    	}
    	if(_newSessions.contains(tracker))
    	{
    		builder.setNewSession();
    		_newSessions.remove(tracker);
    	}
    	tracker.send(builder.setDescription(description)
    	.setFatal(fatal)
    	.build());
    }

    public static void sendItem(Tracker tracker, String transactionId, String name, String sku, String category, double price, long quantity, String currencyCode)
    {
    	HitBuilders.ItemBuilder builder = new HitBuilders.ItemBuilder();
    	SparseArray<String> dimensions = _customDimensions.get(tracker);
    	if(dimensions != null)
    	{
    		for(int i = 0; i < dimensions.size(); i++) 
    			builder.setCustomDimension(dimensions.keyAt(i), dimensions.valueAt(i));
    		_customDimensions.remove(tracker);
    	}
    	SparseArray<Float> metrics = _customMetrics.get(tracker);
    	if(metrics != null)
    	{
    		for(int i = 0; i < metrics.size(); i++) 
    			builder.setCustomMetric(metrics.keyAt(i), metrics.valueAt(i));
    		_customMetrics.remove(tracker);
    	}
    	if(_newSessions.contains(tracker))
    	{
    		builder.setNewSession();
    		_newSessions.remove(tracker);
    	}
    	tracker.send(builder.setTransactionId(transactionId)
    	.setName(name)
    	.setSku(sku)
    	.setCategory(category)
    	.setPrice(price)
    	.setQuantity(quantity)
    	.setCurrencyCode(currencyCode)
    	.build());
    }

    public static void sendSocial(Tracker tracker, String network, String action, String target)
    {
    	HitBuilders.SocialBuilder builder = new HitBuilders.SocialBuilder();
    	SparseArray<String> dimensions = _customDimensions.get(tracker);
    	if(dimensions != null)
    	{
    		for(int i = 0; i < dimensions.size(); i++) 
    			builder.setCustomDimension(dimensions.keyAt(i), dimensions.valueAt(i));
    		_customDimensions.remove(tracker);
    	}
    	SparseArray<Float> metrics = _customMetrics.get(tracker);
    	if(metrics != null)
    	{
    		for(int i = 0; i < metrics.size(); i++) 
    			builder.setCustomMetric(metrics.keyAt(i), metrics.valueAt(i));
    		_customMetrics.remove(tracker);
    	}
    	if(_newSessions.contains(tracker))
    	{
    		builder.setNewSession();
    		_newSessions.remove(tracker);
    	}
    	tracker.send(builder.setNetwork(network)
    	.setAction(action)
    	.setTarget(target)
    	.build());
    }

    public static void sendTiming(Tracker tracker, String category, long intervalMillis, String name, String label)
    {
    	HitBuilders.TimingBuilder builder = new HitBuilders.TimingBuilder();
    	SparseArray<String> dimensions = _customDimensions.get(tracker);
    	if(dimensions != null)
    	{
    		for(int i = 0; i < dimensions.size(); i++) 
    			builder.setCustomDimension(dimensions.keyAt(i), dimensions.valueAt(i));
    		_customDimensions.remove(tracker);
    	}
    	SparseArray<Float> metrics = _customMetrics.get(tracker);
    	if(metrics != null)
    	{
    		for(int i = 0; i < metrics.size(); i++) 
    			builder.setCustomMetric(metrics.keyAt(i), metrics.valueAt(i));
    		_customMetrics.remove(tracker);
    	}
    	if(_newSessions.contains(tracker))
    	{
    		builder.setNewSession();
    		_newSessions.remove(tracker);
    	}
    	tracker.send(builder.setCategory(category)
    	.setValue(intervalMillis)
    	.setVariable(name)
    	.setLabel(label)
    	.build());
    }

    public static void sendTransaction(Tracker tracker, String transactionId, String affiliation, double revenue, double tax, double shipping, String currencyCode)
    {
    	HitBuilders.TransactionBuilder builder = new HitBuilders.TransactionBuilder();
    	SparseArray<String> dimensions = _customDimensions.get(tracker);
    	if(dimensions != null)
    	{
    		for(int i = 0; i < dimensions.size(); i++) 
    			builder.setCustomDimension(dimensions.keyAt(i), dimensions.valueAt(i));
    		_customDimensions.remove(tracker);
    	}
    	SparseArray<Float> metrics = _customMetrics.get(tracker);
    	if(metrics != null)
    	{
    		for(int i = 0; i < metrics.size(); i++) 
    			builder.setCustomMetric(metrics.keyAt(i), metrics.valueAt(i));
    		_customMetrics.remove(tracker);
    	}
    	if(_newSessions.contains(tracker))
    	{
    		builder.setNewSession();
    		_newSessions.remove(tracker);
    	}
    	tracker.send(builder.setTransactionId(transactionId)
    	.setAffiliation(affiliation)
    	.setRevenue(revenue)
    	.setTax(tax)
    	.setShipping(shipping)
    	.setCurrencyCode(currencyCode)
    	.build());
    }
	
	public static void dispatchLocalHits()
	{
		_analytics.dispatchLocalHits();
	}
	public static boolean getAppOptOut()
	{
		return _analytics.getAppOptOut();
	}
	public static void setAppOptOut(boolean optOut)
	{
		_analytics.setAppOptOut(optOut);
	}
	public static boolean isDryRunEnabled()
	{
		return _analytics.isDryRunEnabled();
	}
	public static void setDryRun(boolean dryRun)
	{
		_analytics.setDryRun(dryRun);
	}
	public static boolean getTrackUncaughtExceptions()
	{
		return _uncaughtExceptionHandler != null;
	}
	
	public static void setTrackUncaughtExceptions(boolean value, Tracker tracker)
	{
	    if(value && _uncaughtExceptionHandler == null)
	    {
	    	_defaultExceptionHandler = Thread.getDefaultUncaughtExceptionHandler();
			_uncaughtExceptionHandler = new ExceptionReporter(tracker, _defaultExceptionHandler, activity);   
	    	Thread.setDefaultUncaughtExceptionHandler(_uncaughtExceptionHandler);
	    }
	    if(!value && _defaultExceptionHandler != null)
	    {
	    	Thread.setDefaultUncaughtExceptionHandler(_defaultExceptionHandler);
	    	_uncaughtExceptionHandler = null;
	    	_defaultExceptionHandler = null;
	    }
	}
	public static Tracker getTracker(String trackingId)
	{
		Tracker ret = _trackers.get(trackingId);
		if(ret != null)
			return ret;
		ret = _analytics.newTracker(trackingId);
		_trackers.put(trackingId, ret);
		return ret;
	}
	public static void removeTracker(String trackingId)
	{
		Tracker tracker = _trackers.get(trackingId);
		if(tracker != null)
		{
			_customDimensions.remove(tracker);
			_customMetrics.remove(tracker);
			_newSessions.remove(tracker);
			_trackers.remove(trackingId);
		}
	}
	public static int getLocalDispatchPeriod()
	{
		return _dispatchPeriod;
	}
	public static void setLocalDispatchPeriod(int dispatchPeriodInSeconds)
	{
		_dispatchPeriod = dispatchPeriodInSeconds;
		_analytics.setLocalDispatchPeriod(_dispatchPeriod);
	}
	
	public static void setLogLevel(int logLevel)
	{
		switch(logLevel)
		{
			case kGAILogLevelNone:
				_analytics.getLogger().setLogLevel(Logger.LogLevel.ERROR+1);
				break;
			case kGAILogLevelError:
				_analytics.getLogger().setLogLevel(Logger.LogLevel.ERROR);
				break;
			case kGAILogLevelWarning:
				_analytics.getLogger().setLogLevel(Logger.LogLevel.WARNING);
				break;
			case kGAILogLevelInfo:
				_analytics.getLogger().setLogLevel(Logger.LogLevel.INFO);
				break;
			case kGAILogLevelVerbose:
				_analytics.getLogger().setLogLevel(Logger.LogLevel.VERBOSE);
				break;
		}
	}
	public static int getLogLevel()
	{
		switch(_analytics.getLogger().getLogLevel())
		{
			case Logger.LogLevel.ERROR:
				return kGAILogLevelError;
			case Logger.LogLevel.WARNING:
				return kGAILogLevelWarning;
			case Logger.LogLevel.INFO:
				return kGAILogLevelInfo;
			case Logger.LogLevel.VERBOSE:
				return kGAILogLevelVerbose;
			default:
				return kGAILogLevelNone;
		}
	}
	
	public static void reportActivityStart()
	{
		_analytics.reportActivityStart(activity);
	}
	public static void reportActivityStop()
	{
		_analytics.reportActivityStop(activity);
	}
}
