//
// Created by isndev on 12/15/18.
//

#ifndef QB_MODULE_IOPOLL_TYPES_H
#define QB_MODULE_IOPOLL_TYPES_H

namespace qbm {
    namespace iopoll {

        enum class ReturnValue : int {
            KO = 0,
            REPOLL,
            OK
        };

    }
}

#endif //QB_MODULE_IOPOLL_TYPES_H
