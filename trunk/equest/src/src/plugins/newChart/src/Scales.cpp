#include "NewChartPCH.h"
#include "Scales.h"

void Scales::merge( const Scales& scales )
{
    UTILS_ASSERT(initialized || scales.isInitialized());
    if (initialized && scales.isInitialized()) {
        xMin = (std::min)(xMin, scales.getXMin());
        xMax = (std::max)(xMax, scales.getXMax());
        yMin = (std::min)(yMin, scales.getYMin());
        yMax = (std::max)(yMax, scales.getYMax());
    } else if (scales.isInitialized()) {
        xMin = scales.getXMin();
        xMax = scales.getXMax();
        yMin = scales.getYMin();
        yMax = scales.getYMax();
        initialized = true;
    } else if (!initialized || !scales.isInitialized()) {
        throw loglib::runtime_error("Both scales uninitialized");
    }
}
