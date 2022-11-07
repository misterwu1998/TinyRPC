#if !defined(_Cerealization_hpp)
#define _Cerealization_hpp

/// 0 - "portable binary"
/// 1 - "JSON"
/// 2 - "XML"
#define CEREAL_ARCHIVE_TYPE 1

#include <iostream>
#if CEREAL_ARCHIVE_TYPE==0
#include "cereal/archives/portable_binary.hpp"
#elif CEREAL_ARCHIVE_TYPE==1
#include "cereal/archives/json.hpp"
#elif CEREAL_ARCHIVE_TYPE==2
#include "cereal/archives/xml.hpp"
#endif

namespace TRPC
{
  template<typename Serializable>
  void deserialize(std::istream& i, Serializable& r){
#if CEREAL_ARCHIVE_TYPE==0
    cereal::PortableBinaryInputArchive a(i, cereal::PortableBinaryInputArchive::Options::LittleEndian());
#elif CEREAL_ARCHIVE_TYPE==1
    cereal::JSONInputArchive a(i);
#elif CEREAL_ARCHIVE_TYPE==2
    cereal::XMLInputArchive a(i);
#endif
    a(r);
  }

  template<typename Serializable>
  void serialize(Serializable& r, std::ostream& o){
#if CEREAL_ARCHIVE_TYPE==0
    cereal::PortableBinaryOutputArchive a(o, cereal::PortableBinaryOutputArchive::Options::LittleEndian());
#elif CEREAL_ARCHIVE_TYPE==1
    cereal::JSONOutputArchive a(o);
#elif CEREAL_ARCHIVE_TYPE==2
    cereal::XMLOutputArchive a(o);
#endif
    a(r);
  }

} // namespace TRPC

#endif // _Cerealization_hpp

#if CEREAL_ARCHIVE_TYPE==0
#elif CEREAL_ARCHIVE_TYPE==1
#elif CEREAL_ARCHIVE_TYPE==2
#endif
