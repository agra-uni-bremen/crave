// Copyright 2014 The CRAVE developers. All rights reserved.//

#pragma once

namespace crave {

    class ComplexityEstimationVisitor : NodeVisitor {
    public:
        ComplexityEstimationVisitor();
        ComplexityEstimationVisitor(const ComplexityEstimationVisitor& orig);
        virtual ~ComplexityEstimationVisitor();
    private:

    };
}
