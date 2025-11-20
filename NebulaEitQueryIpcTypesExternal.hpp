/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for helper functions for Nebula IPC Types for
 *        nebula_eit_query.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_EXTERNAL_NEBULAEITQUERYIPCTYPESEXTERNAL_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_EXTERNAL_NEBULAEITQUERYIPCTYPESEXTERNAL_HPP_INCLUDED

#include "nebula/adaptation_layer/nebula_eit_query.h"
#include <string>
#include <vector>

/**
 * @brief Populate SI descriptors
 *
 * @param descriptors       The vector of descriptors.
 * @param dvb_descriptors   [out] The array of descriptors.
 * @param count             [out] The number of descriptors.
 */
void populateDescriptors(const std::vector<std::vector<char>>& descriptors,
                         NEBULA_Descriptor** output_descriptors,
                         unsigned int& count);

/**
 * @brief Populate HD simulcast alternatives.
 *
 * @param crid_vector       The vector of programmes.
 * @param count             [out] The number of programmes.
 * @param crids             [out] The array of crids.
 */
void populateCrids(const std::vector<std::pair<NEBULA_CridType, std::string>>& crids,
                   int& count,
                   NEBULA_Crid** output_crids);

#endif
