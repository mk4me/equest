#include "CommunicationPCH.h"
#include <plugins/hmdbCommunication/DataStatus.h>

using namespace hmdbCommunication;

const DataStatus DataStatus::PartialStatus = DataStatus(PartiallyLocal, PartiallyLoaded, PartiallyValid);

DataStatus::DataStatus(const DataStatus & status) : storage_(status.storage_),
usage_(status.usage_), validity_(status.validity_)
{

}

DataStatus::DataStatus(const DataStorage storage, const DataUsage usage,
	const DataValidity validity) : storage_(storage), usage_(usage),
	validity_(validity)
{

}

DataStatus::DataStatus(const DataStorage storage)
	: storage_(storage), usage_(UnknownUsage),
	validity_(UnknownValidity)
{

}

DataStatus::DataStatus(const DataUsage usage)
	: storage_(UnknownStorage), usage_(usage),
	validity_(UnknownValidity)
{

}

DataStatus::DataStatus(const DataValidity validity)
	: storage_(UnknownStorage), usage_(UnknownUsage),
	validity_(validity)
{

}

DataStatus::~DataStatus()
{

}

const DataStatus::DataStorage DataStatus::storage() const
{
	return storage_;
}

const DataStatus::DataUsage DataStatus::usage() const
{
	return usage_;
}

const DataStatus::DataValidity DataStatus::validity() const
{
	return validity_;
}

void DataStatus::setStorage(const DataStorage storage)
{
	storage_ = storage;
}

void DataStatus::setUsage(const DataUsage usage)
{
	usage_ = usage;
}

void DataStatus::setValidity(const DataValidity validity)
{
	validity_ = validity;
}

const DataStatus::DataStorage DataStatus::merge(const DataStorage storage)
{
	storage_ |= storage;
	return storage_;
}

const DataStatus::DataUsage DataStatus::merge(const DataUsage usage)
{
	usage_ |= usage;
	return usage_;
}

const DataStatus::DataValidity DataStatus::merge(const DataValidity validity)
{
	validity_ |= validity;
	return validity_;
}

const DataStatus & DataStatus::merge(const DataStatus & status)
{
	merge(status.storage());
	merge(status.usage());
	merge(status.validity());
	return *this;
}

const bool DataStatus::filter(const DataStatus & status, const DataStatus & fstatus)
{
	return (((fstatus.storage() == DataStatus::UnknownStorage) || (status.storage() & fstatus.storage())) &&
		((fstatus.usage() == DataStatus::UnknownUsage) || (status.usage() & fstatus.usage())) &&
		((fstatus.validity() == DataStatus::UnknownValidity) || (status.validity() & fstatus.validity())));
}

/*
DataStatus::operator DataStorage()
{
	return storage();
}

DataStatus::operator DataUsage()
{
	return usage();
}

DataStatus::operator DataValidity()
{
	return validity();
}
*/

bool operator==(const DataStatus & a, const DataStatus & b)
{
	return ((a.storage() == b.storage())
		&& (a.usage() == b.usage())
		&& (a.validity() == b.validity()));
}

bool operator!=(const DataStatus & a, const DataStatus & b)
{
	return !(a == b);
}

bool DataStatus::operator<(const DataStatus & b) const
{
	bool ret = true;

	if (storage() == b.storage()){
		if (usage() == b.usage()){
			if (validity() >= b.validity()){
				ret = false;
			}
		}
		else if (usage() > b.usage()){
			ret = false;
		}
	}
	else if (storage() > b.storage()){
		ret = false;
	}

	return ret;
}

DataStatus::DataStorage operator|(DataStatus::DataStorage l, DataStatus::DataStorage r)
{
	return static_cast<DataStatus::DataStorage>((int)l | (int)r);
}

DataStatus::DataStorage & operator|=(DataStatus::DataStorage & l, DataStatus::DataStorage r)
{
	return l = (l | r);
}

DataStatus::DataUsage operator|(DataStatus::DataUsage l, DataStatus::DataUsage r)
{
	return static_cast<DataStatus::DataUsage>((int)l | (int)r);
}

DataStatus::DataUsage & operator|=(DataStatus::DataUsage & l, DataStatus::DataUsage r)
{
	return l = (l | r);
}

DataStatus::DataValidity operator|(DataStatus::DataValidity l, DataStatus::DataValidity r)
{
	return static_cast<DataStatus::DataValidity>((int)l | (int)r);
}

DataStatus::DataValidity & operator|=(DataStatus::DataValidity & l, DataStatus::DataValidity r)
{
	return l = (l | r);
}

DataStatus operator|(const DataStatus & l, const DataStatus & r)
{
	return DataStatus(l.storage() | r.storage(), l.usage() | r.usage(),
		l.validity() | r.validity());
}

DataStatus operator|(const DataStatus & l, const DataStatus::DataStorage & r)
{
	DataStatus ds(l);
	ds.merge(r);
	return ds;
}

DataStatus & operator|=(DataStatus & l, const DataStatus::DataStorage & r)
{
	l.merge(r);
	return l;
}

DataStatus operator|(const DataStatus & l, const DataStatus::DataUsage & r)
{
	DataStatus ds(l);
	ds.merge(r);
	return ds;
}

DataStatus & operator|=(DataStatus & l, const DataStatus::DataUsage & r)
{
	l.merge(r);
	return l;
}

DataStatus operator|(const DataStatus & l, const DataStatus::DataValidity & r)
{
	DataStatus ds(l);
	ds.merge(r);
	return ds;
}

DataStatus & operator|=(DataStatus & l, const DataStatus::DataValidity & r)
{
	l.merge(r);
	return l;
}

DataStatus & operator|=(DataStatus & l, const DataStatus & r)
{
	l.merge(r);
	return l;
}
