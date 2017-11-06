#include <dataaccessorlib/FunctionFeature.h>

using namespace dataaccessor;

const utils::shared_ptr<FunctionFeature> FunctionFeature::functionFeature = utils::make_shared<FunctionFeature>(true);
const utils::shared_ptr<FunctionFeature> FunctionFeature::nonFunctionFeature = utils::make_shared<FunctionFeature>(false);

utils::shared_ptr<FunctionFeature> FunctionFeature::feature(const bool isFunc)
{
	return ((isFunc == true) ? functionFeature : nonFunctionFeature);
}
