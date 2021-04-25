//
// Created by tbwsl on 4/25/21.
//

#ifndef INFORF_RFMETRICINTERFACE_H
#define INFORF_RFMETRICINTERFACE_H
#include "PllTree.hpp"
#include <allocator/alignedallocator.hpp>
#include <data-structures/table_config.hpp>
using hasher_type    = utils_tm::hash_tm::murmur2_hash;
using allocator_type = growt::AlignedAllocator<>;

class RfMetricInterface {
  // This should be our class which we expose to others. Like our commandline
  // module
  void performStuffOnTree(PllTree* t);

};

#endif // INFORF_RFMETRICINTERFACE_H
