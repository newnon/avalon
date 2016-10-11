/*
 * Copyright (c) 2014 Sergey Perepelitsa
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

package com.avalon.systeminfo;

import org.cocos2dx.lib.Cocos2dxHelper;

import android.app.Activity;
import android.os.Build;
import android.util.DisplayMetrics;

public abstract class SystemInfoHelper
{
	private static final Activity activity = Cocos2dxHelper.getActivity();
	
    public static String getDeviceModel()
    {
		String manufacturer = Build.MANUFACTURER;
		String model = Build.MODEL;
		if (model.startsWith(manufacturer)) {
			return model;
		}
		return manufacturer + " " + model;
    }
    
    public static String getIDFA()
    {
    	return "";
    }
    
    public static String getOSVersion()
    {
    	return android.os.Build.VERSION.RELEASE;
    }
    
    public static boolean isTablet()
    {
    	DisplayMetrics metrics = new DisplayMetrics();
    	activity.getWindowManager().getDefaultDisplay().getMetrics(metrics);

    	float yInches = metrics.heightPixels / metrics.densityDpi;
    	float xInches = metrics.widthPixels / metrics.densityDpi;
    	double diagonalInches = Math.sqrt(xInches*xInches + yInches*yInches);
    	if (diagonalInches>=6.5){
        	return true;
    	}else{
    	    return false;
    	}
    }
}
