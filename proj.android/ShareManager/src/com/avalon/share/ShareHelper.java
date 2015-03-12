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

package com.avalon.share;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.net.Uri;
import android.support.v4.app.ShareCompat;
import android.support.v4.content.FileProvider;
import android.view.View;

import org.cocos2dx.lib.Cocos2dxHelper;

public abstract class ShareHelper
{
	private static final Activity activity = Cocos2dxHelper.getActivity();
	private static String _hashTag = "";
	
	private static void copyData(InputStream in, OutputStream out) throws IOException {
        byte[] buffer = new byte[1024];
        int read;
        while((read = in.read(buffer)) != -1){
          out.write(buffer, 0, read);
        }
    }
    
    private static boolean copyFile(File src, File dst){
    	InputStream in = null;
        OutputStream out = null;
        try {
            in = new FileInputStream(src);
            out = new FileOutputStream(dst);
            copyData(in, out);
            in.close();
            in = null;
            out.flush();
            out.close();
            out = null;
            return true;
          } catch(Exception e) {
              e.printStackTrace();
              return false;
          }
    }
    
    private static boolean copyAsset(String fromAssetPath, File toFile) {
        InputStream in = null;
        OutputStream out = null;
        try {
          in = activity.getAssets().open(fromAssetPath);
          out = new FileOutputStream(toFile);
          copyData(in, out);
          in.close();
          in = null;
          out.flush();
          out.close();
          out = null;
          return true;
        } catch(Exception e) {
            e.printStackTrace();
            return false;
        }
    }
    
    private static void internalShareFile(String text, String longText, File file)
    {
    	// let the FileProvider generate an URI for this private file
    	final Uri uri = FileProvider.getUriForFile(activity, activity.getPackageName()+".fileprovider", file);
    	String resultText = text+ " " + "https://play.google.com/store/apps/details?id="+ activity.getPackageName();
    	if(_hashTag.length() != 0)
    		resultText = resultText + " #" + _hashTag;
    	// create an intent, so the user can choose which application he/she wants to use to share this file
    	final Intent intent = ShareCompat.IntentBuilder.from(activity)
    			.setType("image/*")
    			//.setSubject(this.getString(R.string.share_subject))
    			.setStream(uri)
    			.setText(resultText)
    			//.setChooserTitle(R.string.share_title)
    			.createChooserIntent()
    			.addFlags(Intent.FLAG_ACTIVITY_CLEAR_WHEN_TASK_RESET)
    			.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);

    	activity.startActivity(intent);
    }
    
    public static void setHashTag(String hashTag)
    {
    	_hashTag = hashTag;
    }
    
    public static void shareFile(String text, String longText, String path)
    {

    	// try to copy file to default share location
    	final File inputFile = new File(path);
    	final File tempFile = new File(activity.getCacheDir(), "share.jpg");
    	if(inputFile.exists())
    	{
    		copyFile(inputFile, tempFile);
    	}
    	else
    	{
    		copyAsset(path, tempFile);
    	}
    	internalShareFile(text, longText, tempFile);
    }
}
