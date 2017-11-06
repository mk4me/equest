#include "CoreLibPCH.h"
#include <corelib/Version.h>
#include <sstream>

using namespace core;

static const Version::VersionNumberType maxVersionValue = 1000;
static const Version::VersionNumberType minVersionValue = 0;

const bool _verify(const Version::VersionNumberType max,
	const Version::VersionNumberType min,
	const Version::VersionNumberType val)
{
	return (min <= val) && (val < max);
}



const Version::VersionNumberType Version::Traits::maxMajor()
{
	return maxVersionValue;
}

const Version::VersionNumberType Version::Traits::minMajor()
{
	return minVersionValue;
}

const bool Version::Traits::verifyMajor(const VersionNumberType val)
{
	return _verify(maxMajor(), minMajor(), val);
}

const Version::VersionNumberType Version::Traits::maxMinor()
{
	return maxVersionValue;
}

const Version::VersionNumberType Version::Traits::minMinor()
{
	return minVersionValue;
}

const bool Version::Traits::verifyMinor(const VersionNumberType val)
{
	return _verify(maxMinor(), minMinor(), val);
}

const Version::VersionNumberType Version::Traits::maxPatch()
{
	return maxVersionValue;
}

const Version::VersionNumberType Version::Traits::minPatch()
{
	return minVersionValue;
}

const bool Version::Traits::verifyPatch(const VersionNumberType val)
{
	return _verify(maxPatch(), minPatch(), val);
}

const bool Version::Traits::verify(const Version & version)
{
	return (verifyMajor(version.major()) &&
		verifyMinor(version.minor()) &&
		verifyPatch(version.patch()));
}

Version::Version(const VersionNumberType major,
	const VersionNumberType minor,
	const VersionNumberType patch)
	: major_(major), minor_(minor), patch_(patch)
{

}

Version::~Version()
{

}

const Version::VersionNumberType Version::major() const
{
	return major_;
}

const Version::VersionNumberType Version::minor() const
{
	return minor_;
}

const Version::VersionNumberType Version::patch() const
{
	return patch_;
}

const std::string Version::formatedVersion(const Version & version,
	const std::string & separator)
{
	std::stringstream ss;

	ss << version.major_ << separator << version.minor_ << separator << version.patch_;

	return ss.str();
}

const bool Version::operator<(const Version & version)
{
	return (maxVersionValue * maxVersionValue * major_ + maxVersionValue * minor_ + patch_) <
		(maxVersionValue * maxVersionValue * version.major_ + maxVersionValue * version.minor_ + version.patch_);
}

const bool Version::operator==(const Version & version)
{
	return (major_ == version.major_ &&
		minor_ == version.minor_ &&
		patch_ == version.patch_);
}

const bool Version::operator<=(const Version & version)
{
	return ((*this < version) || (*this == version));
}

const bool Version::operator>(const Version & version)
{
	return !(*this <= version);
}

const bool Version::operator>=(const Version & version)
{
	return !(*this < version);
}

const bool Version::isAtLeast(const Version & version)
{
	return (*this >= version);
}

ExtendedVersion::ExtendedVersion(const VersionNumberType major,
	const VersionNumberType minor,
	const VersionNumberType patch)
	: Version(major, minor, patch)
{

}

ExtendedVersion::~ExtendedVersion()
{

}

const bool ExtendedVersion::hasFeature(const std::string & featureName) const
{
	return false;
}