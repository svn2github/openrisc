
// DO NOT EDIT THIS FILE - it is machine generated -*- c++ -*-

#ifndef __javax_imageio_metadata_IIOMetadataFormatImpl$IIOMetadataNodeAttrEnumerated__
#define __javax_imageio_metadata_IIOMetadataFormatImpl$IIOMetadataNodeAttrEnumerated__

#pragma interface

#include <javax/imageio/metadata/IIOMetadataFormatImpl$IIOMetadataNodeAttr.h>
#include <gcj/array.h>

extern "Java"
{
  namespace javax
  {
    namespace imageio
    {
      namespace metadata
      {
          class IIOMetadataFormatImpl;
          class IIOMetadataFormatImpl$IIOMetadataNodeAttrEnumerated;
      }
    }
  }
  namespace org
  {
    namespace w3c
    {
      namespace dom
      {
          class Element;
      }
    }
  }
}

class javax::imageio::metadata::IIOMetadataFormatImpl$IIOMetadataNodeAttrEnumerated : public ::javax::imageio::metadata::IIOMetadataFormatImpl$IIOMetadataNodeAttr
{

public:
  IIOMetadataFormatImpl$IIOMetadataNodeAttrEnumerated(::javax::imageio::metadata::IIOMetadataFormatImpl *, ::org::w3c::dom::Element *, ::java::lang::String *, jint, jboolean, ::java::lang::String *, ::java::util::List *);
  virtual JArray< ::java::lang::Object * > * getEnumerations();
public: // actually protected
  ::java::util::List * __attribute__((aligned(__alignof__( ::javax::imageio::metadata::IIOMetadataFormatImpl$IIOMetadataNodeAttr)))) enumeratedValues;
public: // actually package-private
  ::javax::imageio::metadata::IIOMetadataFormatImpl * this$0;
public:
  static ::java::lang::Class class$;
};

#endif // __javax_imageio_metadata_IIOMetadataFormatImpl$IIOMetadataNodeAttrEnumerated__
