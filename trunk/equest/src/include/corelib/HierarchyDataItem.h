/********************************************************************
	created:	2013/05/07
	created:	7:5:2013   14:28
	filename: 	HierarchyDataItem.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__HIERARCHYDATAITEM_H__
#define HEADER_GUARD_CORE__HIERARCHYDATAITEM_H__

#include <QtCore/QList>
#include <QtCore/QVariant>
#include <corelib/Variant.h>
#include <corelib/HierarchyHelper.h>
#include <corelib/HierarchyItem.h>
#include <corelib/IHierarchyDataItem.h>

namespace core {

    //! Klasa stanowi podstawowy element hierarchii danych.
    //! Jej podstawowym zadaniem jest dostarczenie danych i sposobu ich wizualizacji
    class CORELIB_EXPORT HierarchyDataItem : public HierarchyItem, public IHierarchyDataItem
    {
    public:
        //! 
        //! \param wrapper 
        //! \param icon 
        //! \param name 
        //! \param helper 
		HierarchyDataItem(VariantConstPtr wrapper,
            const QIcon& icon, const QString& name, const QString& description, HierarchyHelperPtr helper);

        //! 
        //! \param wrapper 
        //! \param icon 
        //! \param name 
        //! \param helpers 
		HierarchyDataItem(VariantConstPtr wrapper,
            const QIcon& icon, const QString& name, const QString& description, std::list<HierarchyHelperPtr> helpers);

        //! 
        //! \param icon 
        //! \param name 
        //! \param description 
        HierarchyDataItem(const QIcon& icon, const QString& name, const QString& description);
        
        //! 
        //! \param wrapper 
        //! \param icon 
        //! \param name 
		HierarchyDataItem(VariantConstPtr wrapper,
            const QIcon& icon, const QString& name, const QString& description);

        //! 
        //! \param wrapper 
        //! \param icon 
		HierarchyDataItem(VariantConstPtr wrapper, const QString& description,
            const QIcon& icon = QIcon());

        //! 
        //! \param icon 
        //! \param name 
        //! \param helper 
        HierarchyDataItem(const QIcon& icon, const QString& name, const QString& description, 
            HierarchyHelperPtr helper);

	    virtual ~HierarchyDataItem();

    public:
        //! 
        virtual std::list<HierarchyHelperPtr> getHelpers() const;
		//!
		virtual HierarchyHelperPtr getDefaultHelper() const;
        //! 
		virtual VariantConstPtr getData() const;
        //! 
        //! \param helper 
        void addHelper(HierarchyHelperPtr helper);

		void setDefaultHelper(const int idx);

        virtual IHierarchyItemPtr shallowCopy(bool withChildren) const;

    private:
        std::list<HierarchyHelperPtr> helpers;
		int defaultHelperIDX;
		VariantConstPtr data;
    };
    DEFINE_SMART_POINTERS(HierarchyDataItem)
}

#endif
