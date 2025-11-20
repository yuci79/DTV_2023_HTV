/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Implementation for IPC helper functions for nebula_eit_query.
 */

#include "NebulaEitQueryIpcTypesExternal.hpp"
#include <algorithm>
#include <cstring>

void populateDescriptors(const std::vector<std::vector<char>>& descriptors,
                         NEBULA_Descriptor** output_descriptors,
                         unsigned int& count)
{
    *output_descriptors = nullptr;
    count = static_cast<unsigned int>(descriptors.size());
    if (count > 0)
    {
        *output_descriptors = new NEBULA_Descriptor[descriptors.size()];
        NEBULA_Descriptor* dvb_descriptor = *output_descriptors;
        std::for_each(descriptors.begin(), descriptors.end(),
            [&](const std::vector<char>& descriptor)
            {
                dvb_descriptor->length = static_cast<frost_uint>(descriptor.size());
                dvb_descriptor->body = static_cast<char*>(malloc(sizeof(char) * descriptor.size()));
                std::copy(descriptor.begin(),
                        descriptor.begin() + descriptor.size(),
                        dvb_descriptor->body);
                dvb_descriptor++;
            });
    }
}

void populateCrids(const std::vector<std::pair<NEBULA_CridType, std::string>>& crids,
                   int& count,
                   NEBULA_Crid** output_crids)
{
    *output_crids = nullptr;
    count = static_cast<int>(crids.size());
    if (count > 0)
    {
        *output_crids = reinterpret_cast<NEBULA_Crid*>(malloc(count * sizeof(NEBULA_Crid)));

        NEBULA_Crid* crid = *output_crids;
        std::for_each(crids.begin(),crids.end(),
            [&](const std::pair<NEBULA_CridType, std::string>& crid_pair)
            {
                crid->type = crid_pair.first;
                const std::string& url = crid_pair.second;
                crid->url = reinterpret_cast<char*>(malloc(sizeof(char) * (url.length() + 1)));
                strcpy(crid->url, url.c_str());
                crid++;                       
            });
    }
}