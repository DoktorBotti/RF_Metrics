//
// Created by tbwsl on 4/25/21.
//

#ifndef INFORF_RFMETRICINTERFACE_H
#define INFORF_RFMETRICINTERFACE_H

#include "PllTree.hpp"
#include <allocator/alignedallocator.hpp>
#include <data-structures/table_config.hpp>

using hasher_type = utils_tm::hash_tm::murmur2_hash;
using allocator_type = growt::AlignedAllocator<>;

class RfMetricInterface {
    struct Params {
        size_t threads;
        std::string input_file_path;
        std::string output_file_path;
        enum {RF, MCI, MSI, SPI} metric;
    };
    // This should be our class which we expose to others. Like our commandline
    // module
    void do_magical_high_performance_stuff(const Params& params);

};

#endif // INFORF_RFMETRICINTERFACE_H
