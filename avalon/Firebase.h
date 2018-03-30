//
//  Firebase.hpp
//  Firebase
//
//  Created by Vladimir Shlobin on 29.03.2018.
//

#ifndef Firebase_hpp
#define Firebase_hpp

#include <map>
#include <string>

/// @brief Namespace that encompasses all Firebase APIs.
namespace avalon {

/// @brief Firebase Analytics API.
///
/// See <a href="/docs/analytics">the developer guides</a> for general
/// information on using Firebase Analytics in your apps.
namespace firebase {

/// @brief Initialize the Analytics API.
///
/// This must be called prior to calling any other methods in the
/// firebase::analytics namespace.
///
/// @param[in] app Default @ref firebase::App instance.
///
/// @see firebase::App::GetInstance().
void initialize();

/// @brief Terminate the Analytics API.
///
/// Cleans up resources associated with the API.
void terminate();

/// @brief Sets whether analytics collection is enabled for this app on this
/// device.
///
/// This setting is persisted across app sessions. By default it is enabled.
///
/// @param[in] enabled true to enable analytics collection, false to disable.
void setAnalyticsCollectionEnabled(bool enabled);

/// @brief Log an event with one string parameter.
///
/// @param[in] name Name of the event to log. Should contain 1 to 40
/// alphanumeric characters or underscores. The name must start with an
/// alphabetic character. Some event names are reserved.
/// @if cpp_examples
/// See @ref event_names (%event_names.h) for the list of reserved event names.
/// @endif
/// The "firebase_" prefix is reserved and should not be used. Note that event
/// names are case-sensitive and that logging two events whose names differ
/// only in case will result in two distinct events.
/// @param[in] parameter_name Name of the parameter to log.
/// For more information, see @ref Parameter.
/// @param[in] parameter_value Value of the parameter to log.
///
/// @if cpp_examples
/// @see LogEvent(const char*, const Parameter*, size_t)
/// @endif
void logEvent(const std::string &name, const std::map<std::string, std::string> &params);

/// @brief Set a user property to the given value.
///
/// Properties associated with a user allow a developer to segment users
/// into groups that are useful to their application.  Up to 25 properties
/// can be associated with a user.
///
/// Suggested property names are listed @ref user_property_names
/// (%user_property_names.h) but you're not limited to this set. For example,
/// the "gamertype" property could be used to store the type of player where
/// a range of values could be "casual", "mid_core", or "core".
///
/// @param[in] name Name of the user property to set.  This must be a
/// combination of letters and digits (matching the regular expression
/// [a-zA-Z0-9] between 1 and 40 characters long starting with a letter
/// [a-zA-Z] character.
/// @param[in] property Value to set the user property to.  Set this
/// argument to NULL or nullptr to remove the user property.  The value can be
/// between 1 to 100 characters long.
void setUserProperty(const std::string &name, const std::string &property);

/// @brief Sets the user ID property.
///
/// This feature must be used in accordance with
/// <a href="https://www.google.com/policies/privacy">Google's Privacy
/// Policy</a>
///
/// @param[in] user_id The user ID associated with the user of this app on this
/// device.  The user ID must be non-empty and no more than 100 characters long.
/// Setting user_id to NULL or nullptr removes the user ID.
void setUserId(const std::string &userId);

/// @brief Sets the minimum engagement time required before starting a session.
///
/// @note The default value is 10000 (10 seconds).
///
/// @param milliseconds The minimum engagement time required to start a new
/// session.
void setMinimumSessionDuration(int64_t milliseconds);

/// @brief Sets the duration of inactivity that terminates the current session.
///
/// @note The default value is 1800000 (30 minutes).
///
/// @param milliseconds The duration of inactivity that terminates the current
/// session.
void setSessionTimeoutDuration(int64_t milliseconds);

/// @brief Sets the current screen name and screen class, which specifies the
/// current visual context in your app. This helps identify the areas in your
/// app where users spend their time and how they interact with your app.
///
/// @param screen_name The name of the current screen. Set to nullptr to clear
/// the current screen name. Limited to 100 characters.
/// @param screen_class The name of the screen class. If you specify nullptr for
/// this, it will use the default. On Android, the default is the class name of
/// the current Activity. On iOS, the default is the class name of the current
/// UIViewController. Limited to 100 characters.
void setCurrentScreen(const std::string &screenName);

}  // namespace analytics
}  // namespace firebase

#endif /* Firebase_hpp */
