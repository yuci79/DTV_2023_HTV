/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Defines the helper functions for Nebula IPC.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_EXTERNAL_NEBULAIPCHELPER_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_EXTERNAL_NEBULAIPCHELPER_HPP_INCLUDED

#include <string>

/***
 * @brief Helper function to assist in making nebula specific rpc calls to the aurora
 *        process.
 * 
 * @param function_name The name of the rpc function to call.
 * @param args A set of arguments to pass to the rpc function.
 * 
 * @return the value returned by the remote NEBULA_* function call.
 *         If the rpc fails an exception will be thrown.
 */
template<typename Return, typename... Args> inline
Return
nebulaRpcCall(const std::string& function_name, Args&&... args);

/***
 * @brief Helper function to assist in making nebula specific rpc calls to the aurora
 *        process that have output parameters in the NEBULA_* call.
 * 
 * @param output_params The destination for the remote NEBULA_* output params.
 * @param function_name The name of the rpc function to call.
 * @param args A set of arguments to pass to the rpc function.
 * 
 * @return the value returned by the remote NEBULA_* function call.
 *         If the rpc fails an exception will be thrown.
 */
template<typename Return, typename Value, typename NebulaOutputParams, typename... Args> inline
Return
nebulaRpcCall(NebulaOutputParams& output_params, const std::string& function_name, Args&&... args);

/***
 * @brief Helper function to assist in making nebula specific rpc calls to the aurora
 *        process.
 * 
 * @param function_name The name of the rpc function to call.
 * @param args A set of arguments to pass to the rpc function.
 * 
 * @return Returns void. If the rpc fails an exception will be thrown.
 */
template<typename... Args>
void
nebulaRpcCall(const std::string& function_name, Args&&... args);

#include "NebulaIpcHelper.inl"

#endif
