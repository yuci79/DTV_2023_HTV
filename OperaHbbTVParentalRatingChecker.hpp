/*
 * Copyright 2017 Cabot Communications.  All rights reserved.
 *
 * @file 
 * @brief Handles management of parental ratings for HbbTV applications
 */

#ifndef CABOT_VEWD_INTEGRATION_CORE_OPERAHBBTVPARENTALRATINGCHECKER_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_CORE_OPERAHBBTVPARENTALRATINGCHECKER_HPP_INCLUDED

#include "eclipse/core/ParentalRating.hpp"

#include "nebula/core/ait_hub/AitSubtable.hpp"

#include "utilities_public/ExplodingBoolean.hpp"
#include <vector>
#include <map>

class AnyWebBrowserUIController;

class OperaHbbTVParentalRatingChecker
{
public:
    /**
     * @brief Holds all of the "scheme/rating code"" pairs found for a single application
     */
    typedef std::vector<std::pair<UString, ParentalRatingCode>> VectorOfParentalRatings;

    /**
     * @brief Maps the vector of parental ratings to the parrent HbbTV application ID
     *
     * @note An URL+ID hash key might avoid potential false positive results
     */
    typedef std::map<int, VectorOfParentalRatings> ApplicationParentalRating;
    
public:
    /**
     * @brief Ctor for OperaHbbTVParentalRatingChecker
     *
     */
    OperaHbbTVParentalRatingChecker();

    /**
     * @brief Dtor for OperaHbbTVParentalRatingChecker
     */
    ~OperaHbbTVParentalRatingChecker();

    /**
     * @brief Initialise the parental rating checker
     *
     * @param ui_controller Specifies the UI controller the checker uses to create UI prompts
     *
     * @return true if the checker was initialised successfully
     */
    ExplodingBoolean initialise(AnyWebBrowserUIController* ui_controller);
 
    /**
     * @brief Verify if the specified app violates the parental rating threshold
     * set for the terminal
     *
     * @param app_id The ID of the Hbb Application to verify
     *
     * @param app_url The URL of the Hbb Application to verify.
     * Used to differentiate between applications that might share the same ID
     *
     * @return true if the application does violate the parental rating settings
     */
    bool isRatingBlocked(unsigned short app_id, const char* app_url);

    /**
     * @brief Verify if the specified query data violates the parental rating
     *
     * @param app_url The URL of the Hbb Application to verify.
     *
     * @param query_data The query data from within <event> element from the mpd
     * As per ETSI TS 103 285 - 9.1.2.1/2/3
     *
     * @return true if the application does violate the parental rating settings
     */
    bool isRatingBlocked(const char* app_url, std::string const& query_data);

    /**
     * @brief Start the normal parental rating authorisation process (e.g. displays a pin prompt)
     *
     * @param app_id The ID of the Hbb Application to verify
     *
     * @param app_url The URL of the Hbb Application to verify.
     * Used to differentiate between applications that might share the same ID
     *
     * @return true if the authorisation process was successful (e.g. the correct pin was entered and verified)
     *
     * @note this is a blocking call. It will only return once the authorisation process finishes.
     */
    bool authorise(unsigned short app_id, const char* app_url);

    /**
     * @brief Start the normal parental rating authorisation process (e.g. displays a pin prompt)
     *
     * @param app_url The URL of the Hbb Application used to log if the authorisation fails
     *
     * @return true if the authorisation process was successful (e.g. the correct pin was entered
     * and verified)
     *
     * @note this is a blocking call. It will only return once the authorisation process finishes.
     */
    bool authorise(const char* app_url);

    /**
     * @brief Match and save the specified parental rating information with the application
     * id and url passed in the parameters
     *
     * @param app_id The ID of the Hbb Application to verify
     *
     * @param app_url The URL of the Hbb Application to verify.
     * Used to differentiate between applications that might share the same ID
     *
     * @param rating The parental rating age value
     *
     * @param scheme The parental rating scheme used
     *
     * @param country The country used for parental rating matching
     */
    void processRating(unsigned short app_id,
                       const char* app_url,
                       const std::string &rating,
                       const std::string &scheme,
                       const std::string &country);

private:
    AnyWebBrowserUIController* m_ui_controller;
    ApplicationParentalRating  m_parental_rating;

    bool isRatingBlocked(ParentalRatingCode rating_code, const char* scheme);
};

#endif //CABOT_VEWD_INTEGRATION_CORE_OPERAHBBTVPARENTALRATINGCHECKER_HPP_INCLUDED
