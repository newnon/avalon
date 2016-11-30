package com.avalon.gamecenter;

import android.preference.PreferenceManager.OnActivityResultListener;
import android.util.Log;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;

import org.cocos2dx.lib.Cocos2dxActivity;
import org.cocos2dx.lib.Cocos2dxHelper;

import com.google.android.gms.games.Games;

public abstract class GameCenter
{
    private static GameHelper mHelper = null;
    private static final String TAG = "avalon.GameCenter";
    private static final Cocos2dxActivity activity = (Cocos2dxActivity) Cocos2dxActivity.getContext();
    private final static int REQUEST_ACHIEVEMENTS = 9003;
    private final static int REQUEST_LEADERBOARD = 9004;

    /**
     *
     * Interface used for C++
     *
     */
     
    public static void login()
    {
    	activity.runOnUiThread(new Runnable() {
            public void run() {
                getGameHelper().beginUserInitiatedSignIn();
            }
        });
    }

    public static void logout()
    {
        if (mHelper == null) {
            return;
        }

        activity.runOnUiThread(new Runnable() {
            public void run() {
                getGameHelper().signOut();
            }
        });
    }

    public static boolean isloggedIn()
    {
        return (mHelper != null && getGameHelper().isSignedIn());
    }
    
    public static java.lang.String getPlayerId() {
    	if (getGameHelper().isSignedIn()) {
    		return Games.getCurrentAccountName(getGameHelper().getApiClient());
    	}
    	
    	return "";
    }
    
    public static boolean showGameCenter()
    {
        return false;
    }

    public static boolean showAchievements()
    {
    	if (!isloggedIn()) {
        	activity.runOnUiThread(new Runnable() {
                public void run() {
                	AlertDialog.Builder alert = new AlertDialog.Builder(Cocos2dxHelper.getActivity());
       			 	//alert.setTitle("Error");
                	alert.setMessage("Authentication is required. You need to sign into your Google Account.");

                	alert.setPositiveButton("OK", new DialogInterface.OnClickListener() {
                		public void onClick(DialogInterface dialog, int which) {
                			// Do nothing but close the dialog
                			dialog.dismiss();
                		}
                	});

                	alert.create().show();
                }
            });
            return false;
        }
        
        activity.runOnUiThread(new Runnable() {
            public void run() {
                activity.startActivityForResult(Games.Achievements.getAchievementsIntent(getGameHelper().getApiClient()),
                    REQUEST_ACHIEVEMENTS
                );
            }
        });
        return true;
    }

    public static void postAchievement(final String idName, int percentComplete)
    {
        if (!isloggedIn() || percentComplete < 100) {
            return;
        }

        activity.runOnUiThread(new Runnable() {
            public void run() {
                int rId = getResourceId(idName);
                if (rId > 0) {
                    String googleId = activity.getResources().getString(rId);
                    Games.Achievements.unlock(getGameHelper().getApiClient(),googleId);
                }
            }
        });
    }

    public static void clearAllAchievements()
    {
        Log.v(TAG, "clearAllAchievements is not available on this platform");
    }

    public static boolean showScores()
    {
        if (!isloggedIn()) {
        	activity.runOnUiThread(new Runnable() {
                public void run() {
                	AlertDialog.Builder alert = new AlertDialog.Builder(Cocos2dxHelper.getActivity());
       			 	//alert.setTitle("Error");
                	alert.setMessage("Authentication is required. You need to sign into your Google Account.");

                	alert.setPositiveButton("OK", new DialogInterface.OnClickListener() {
                		public void onClick(DialogInterface dialog, int which) {
                			// Do nothing but close the dialog
                			dialog.dismiss();
                		}
                	});

                	alert.create().show();
                }
            });
            return false;
        }
        
        activity.runOnUiThread(new Runnable() {
            public void run() {
                activity.startActivityForResult(
                	Games.Leaderboards.getAllLeaderboardsIntent(getGameHelper().getApiClient()),
                    REQUEST_LEADERBOARD
                );
            }
        });
        return true;
    }

    public static void postScore(final String idName, final int score)
    {
        if (!isloggedIn()) {
            return;
        }

        activity.runOnUiThread(new Runnable() {
            public void run() {
                int rId = getResourceId(idName);
                if (rId > 0) {
                    String googleId = activity.getResources().getString(rId);
                    Games.Leaderboards.submitScore(getGameHelper().getApiClient(), googleId, score);
                }
            }
        });
    }

    public static void clearAllScores()
    {
        Log.v(TAG, "clearAllScores is not available on this platform");
    }

    /**
     *
     * Public interface to integrate this into the main activity
     *
     */
     
    public static void onStart()
    {
        getGameHelper().onStart(activity);
    }

    public static void onStop()
    {
        if (mHelper == null) {
            return;
        }

        getGameHelper().onStop();
    }

    public static void onActivityResult(int requestCode, int resultCode, Intent data)
    {
        if (mHelper == null) {
            return;
        }

        getGameHelper().onActivityResult(requestCode, resultCode, data);
    }

    /**
     *
     * Private helper methods
     *
     */

    private static int getResourceId(String idName)
    {
        int rId = activity.getResources().getIdentifier(idName, "string", activity.getPackageName());
        if (rId == 0) {
            Log.v(TAG, "Unable to find resource ID for string " + idName);
        }
        return rId;

    }

    @SuppressWarnings("deprecation")
	private static GameHelper getGameHelper()
    {
        if (mHelper == null) {
            mHelper = new GameHelper(activity, GameHelper.CLIENT_GAMES);
            mHelper.enableDebugLog(true, TAG);
            
            Cocos2dxHelper.addOnActivityResultListener(new OnActivityResultListener(){
                @Override
                public boolean onActivityResult(int requestCode, int resultCode, Intent data) {
                    GameCenter.onActivityResult(requestCode, resultCode, data);
                    return true;
                }
            });

            activity.runOnUiThread(new Runnable() {
                public void run() {
                    mHelper.setup(
                        new GameHelper.GameHelperListener() {
                            @Override
                            public void onSignInFailed() {
                                Log.v(TAG, "onSignInFailed");
                            }

                            @Override
                            public void onSignInSucceeded() {
                                Log.v(TAG, "onSignInSucceeded");
                            }
                        }
                    );
                }
            });
        }

        return mHelper;
    }
}