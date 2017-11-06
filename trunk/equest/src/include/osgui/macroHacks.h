#ifndef __HEADER_GUARD__MACROHACKS_H__
#define __HEADER_GUARD__MACROHACKS_H__

#define META_Widget(library, name) \
	virtual osg::Object* cloneType() const { return new name (); } \
	virtual osg::Object* clone(const osg::CopyOp& copyop) const { return new name (*this,copyop); } \
	virtual bool isSameKindAs(const osg::Object* obj) const { return dynamic_cast<const name *>(obj)!=NULL; } \
	virtual const char* libraryName() const { return #library; }\
	virtual const char* className() const { return "Widget"; }


#define META_Window(library, name) \
	virtual osg::Object* cloneType() const { return new name (); } \
	virtual osg::Object* clone(const osg::CopyOp& copyop) const { return new name (*this,copyop); } \
	virtual bool isSameKindAs(const osg::Object* obj) const { return dynamic_cast<const name *>(obj)!=NULL; } \
	virtual const char* libraryName() const { return #library; }\
	virtual const char* className() const { return "Window"; }

#define META_ObjectBase(library, name) \
	virtual osg::Object* cloneType() const { return new name (); } \
	virtual osg::Object* clone(const osg::CopyOp& copyop) const { return new name (*this,copyop); } \
	virtual bool isSameKindAs(const osg::Object* obj) const { return dynamic_cast<const name *>(obj)!=NULL; }

#endif
