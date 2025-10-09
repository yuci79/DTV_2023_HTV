/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Defines the helper functions for IPC.
 */

#ifndef CABOT_VEWD_COMMON_IPC_BROWSER_VEWDAURORAIPCHELPER_HPP_INCLUDED
#define CABOT_VEWD_COMMON_IPC_BROWSER_VEWDAURORAIPCHELPER_HPP_INCLUDED

#include <string>

/***
 * @brief Helper function to assist in making specific rpc calls to the aurora
 *        process.
 * 
 * @param function_name The name of the rpc function to call.
 * @param args A set of arguments to pass to the rpc function.
 * 
 * @return the value returned by the remote function call.
 *         If the rpc fails an exception will be thrown.
 */
template<typename Return, typename... Args> inline
Return
vewdAuroraRpcCall(const std::string& function_name, Args&&... args);

/***
 * @brief Helper function to assist in making specific rpc calls to the aurora
 *        process that have output parameters in the interface function call.
 * 
 * @param output_params The destination for the remote interface function output params.
 * @param function_name The name of the rpc function to call.
 * @param args A set of arguments to pass to the rpc function.
 * 
 * @return the value returned by the remote interface function call.
 *         If the rpc fails an exception will be thrown.
 */
template<typename Return, typename Value, typename AuroraOutputParams, typename... Args> inline
Return
vewdAuroraRpcCall(AuroraOutputParams& output_params, const std::string& function_name, Args&&... args);

/***
 * @brief Helper function to assist in making specific rpc calls to the aurora
 *        process.
 * 
 * @param function_name The name of the rpc function to call.
 * @param args A set of arguments to pass to the rpc function.
 * 
 * @return Returns void. If the rpc fails an exception will be thrown.
 */
template<typename... Args>
void
vewdAuroraRpcCall(const std::string& function_name, Args&&... args);

#include "VewdAuroraIpcHelper.inl"

#endif
