/**
 * Copyright 2021 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Nebula IPC implementation of nebula_eit_query.
 */

#include "nebula_src/adaptation_layer/ipc/NebulaEitQueryIpcTypes.hpp"
#include "NebulaEitQueryIpcTypesExternal.hpp"
#include "NebulaIpcHelper.hpp"

extern "C"
frost_int NEBULA_GetNumProgrammes(frost_int channel_id,
                                  frost_bool now_next_only,
                                  frost_uint start_time,
                                  frost_uint interval_in_seconds)
{
    return nebulaRpcCall<frost_int>(IPC_NAME(NEBULA_GetNumProgrammes),
                                    channel_id,
                                    now_next_only,
                                    start_time,
                                    interval_in_seconds);
}

extern "C"
frost_bool NEBULA_GetProgramInfo(frost_int program_id, NEBULA_Program* program)
{
    return nebulaRpcCall<frost_bool, MP_NEBULA_Program>(*program,
                                                 IPC_NAME(NEBULA_GetProgramInfo),
                                                 program_id);
}

extern "C"
frost_int NEBULA_GetProgramIDByIndex(frost_int index, frost_int channel_id)
{
    return nebulaRpcCall<frost_int>(IPC_NAME(NEBULA_GetProgramIDByIndex),
                                    index, channel_id);
}

extern "C"
frost_bool NEBULA_GetDescriptors(frost_int program_id,
                                 unsigned char tag,
                                 unsigned char tag_extension,
                                 frost_bool has_tag_extension,
                                 int private_data_specifier,
                                 frost_bool has_private_data_specifier,
                                 NEBULA_Descriptor **dvb_descriptors,
                                 unsigned int *count)
{
    std::vector<std::vector<char>> descriptors;
    const frost_bool result = nebulaRpcCall<frost_bool, std::vector<std::vector<char>>>(descriptors,
                                IPC_NAME(NEBULA_GetDescriptors),
                                program_id,
                                tag,
                                tag_extension,
                                has_tag_extension,
                                private_data_specifier,
                                has_private_data_specifier);

    populateDescriptors(descriptors, dvb_descriptors, *count);

    return result;
}

extern "C"
frost_bool NEBULA_ProgrammeIs3D(NEBULA_Program* programme)
{
    const bool has_programme = (programme != nullptr);
    const auto data = std::make_pair(has_programme,
                            has_programme ? MP_NEBULA_Program(*programme) : MP_NEBULA_Program());
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_ProgrammeIs3D), data);
}

extern "C"
frost_int NEBULA_GetGuideDaysAvailable()
{
    return nebulaRpcCall<frost_int>(IPC_NAME(NEBULA_GetGuideDaysAvailable));
}

extern "C"
void NEBULA_GetProgrammeCRIDs(NEBULA_Program* programme, int* count, NEBULA_Crid** crids)
{
    const bool has_programme = (programme != nullptr);
    const auto data = std::make_pair(has_programme,
                            has_programme ? MP_NEBULA_Program(*programme) : MP_NEBULA_Program());

    const auto crid_vector = nebulaRpcCall<std::vector<std::pair<NEBULA_CridType, std::string>>>
                            (IPC_NAME(NEBULA_GetProgrammeCRIDs), data);

    populateCrids(crid_vector, *count, crids);
}