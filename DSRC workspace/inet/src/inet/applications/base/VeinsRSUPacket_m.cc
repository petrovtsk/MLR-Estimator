//
// Generated file, do not edit! Created by nedtool 5.5 from inet/applications/base/VeinsRSUPacket.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include "VeinsRSUPacket_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

namespace inet {

// forward
template<typename T, typename A>
std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec);

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// operator<< for std::vector<T>
template<typename T, typename A>
inline std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec)
{
    out.put('{');
    for(typename std::vector<T,A>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        if (it != vec.begin()) {
            out.put(','); out.put(' ');
        }
        out << *it;
    }
    out.put('}');
    
    char buf[32];
    sprintf(buf, " (size=%u)", (unsigned int)vec.size());
    out.write(buf, strlen(buf));
    return out;
}

Register_Class(VeinsRSUPacket)

VeinsRSUPacket::VeinsRSUPacket(const char *name, short kind) : ::omnetpp::cPacket(name,kind)
{
    this->sequenceNumber = 0;
    this->casOdoslania = 0;
    this->firstSent = 0;
    this->posX = 0;
    this->posY = 0;
    this->prikaz = 0;
    this->visID = 0;
    this->approaching = false;
}

VeinsRSUPacket::VeinsRSUPacket(const VeinsRSUPacket& other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

VeinsRSUPacket::~VeinsRSUPacket()
{
}

VeinsRSUPacket& VeinsRSUPacket::operator=(const VeinsRSUPacket& other)
{
    if (this==&other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void VeinsRSUPacket::copy(const VeinsRSUPacket& other)
{
    this->sequenceNumber = other.sequenceNumber;
    this->casOdoslania = other.casOdoslania;
    this->firstSent = other.firstSent;
    this->posX = other.posX;
    this->posY = other.posY;
    this->sequence = other.sequence;
    this->prikaz = other.prikaz;
    this->visID = other.visID;
    this->approaching = other.approaching;
}

void VeinsRSUPacket::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->sequenceNumber);
    doParsimPacking(b,this->casOdoslania);
    doParsimPacking(b,this->firstSent);
    doParsimPacking(b,this->posX);
    doParsimPacking(b,this->posY);
    doParsimPacking(b,this->sequence);
    doParsimPacking(b,this->prikaz);
    doParsimPacking(b,this->visID);
    doParsimPacking(b,this->approaching);
}

void VeinsRSUPacket::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->sequenceNumber);
    doParsimUnpacking(b,this->casOdoslania);
    doParsimUnpacking(b,this->firstSent);
    doParsimUnpacking(b,this->posX);
    doParsimUnpacking(b,this->posY);
    doParsimUnpacking(b,this->sequence);
    doParsimUnpacking(b,this->prikaz);
    doParsimUnpacking(b,this->visID);
    doParsimUnpacking(b,this->approaching);
}

long VeinsRSUPacket::getSequenceNumber() const
{
    return this->sequenceNumber;
}

void VeinsRSUPacket::setSequenceNumber(long sequenceNumber)
{
    this->sequenceNumber = sequenceNumber;
}

::omnetpp::simtime_t VeinsRSUPacket::getCasOdoslania() const
{
    return this->casOdoslania;
}

void VeinsRSUPacket::setCasOdoslania(::omnetpp::simtime_t casOdoslania)
{
    this->casOdoslania = casOdoslania;
}

::omnetpp::simtime_t VeinsRSUPacket::getFirstSent() const
{
    return this->firstSent;
}

void VeinsRSUPacket::setFirstSent(::omnetpp::simtime_t firstSent)
{
    this->firstSent = firstSent;
}

double VeinsRSUPacket::getPosX() const
{
    return this->posX;
}

void VeinsRSUPacket::setPosX(double posX)
{
    this->posX = posX;
}

double VeinsRSUPacket::getPosY() const
{
    return this->posY;
}

void VeinsRSUPacket::setPosY(double posY)
{
    this->posY = posY;
}

const char * VeinsRSUPacket::getSequence() const
{
    return this->sequence.c_str();
}

void VeinsRSUPacket::setSequence(const char * sequence)
{
    this->sequence = sequence;
}

int VeinsRSUPacket::getPrikaz() const
{
    return this->prikaz;
}

void VeinsRSUPacket::setPrikaz(int prikaz)
{
    this->prikaz = prikaz;
}

int VeinsRSUPacket::getVisID() const
{
    return this->visID;
}

void VeinsRSUPacket::setVisID(int visID)
{
    this->visID = visID;
}

bool VeinsRSUPacket::getApproaching() const
{
    return this->approaching;
}

void VeinsRSUPacket::setApproaching(bool approaching)
{
    this->approaching = approaching;
}

class VeinsRSUPacketDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    VeinsRSUPacketDescriptor();
    virtual ~VeinsRSUPacketDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(VeinsRSUPacketDescriptor)

VeinsRSUPacketDescriptor::VeinsRSUPacketDescriptor() : omnetpp::cClassDescriptor("inet::VeinsRSUPacket", "omnetpp::cPacket")
{
    propertynames = nullptr;
}

VeinsRSUPacketDescriptor::~VeinsRSUPacketDescriptor()
{
    delete[] propertynames;
}

bool VeinsRSUPacketDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<VeinsRSUPacket *>(obj)!=nullptr;
}

const char **VeinsRSUPacketDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *VeinsRSUPacketDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int VeinsRSUPacketDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 9+basedesc->getFieldCount() : 9;
}

unsigned int VeinsRSUPacketDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<9) ? fieldTypeFlags[field] : 0;
}

const char *VeinsRSUPacketDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "sequenceNumber",
        "casOdoslania",
        "firstSent",
        "posX",
        "posY",
        "sequence",
        "prikaz",
        "visID",
        "approaching",
    };
    return (field>=0 && field<9) ? fieldNames[field] : nullptr;
}

int VeinsRSUPacketDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='s' && strcmp(fieldName, "sequenceNumber")==0) return base+0;
    if (fieldName[0]=='c' && strcmp(fieldName, "casOdoslania")==0) return base+1;
    if (fieldName[0]=='f' && strcmp(fieldName, "firstSent")==0) return base+2;
    if (fieldName[0]=='p' && strcmp(fieldName, "posX")==0) return base+3;
    if (fieldName[0]=='p' && strcmp(fieldName, "posY")==0) return base+4;
    if (fieldName[0]=='s' && strcmp(fieldName, "sequence")==0) return base+5;
    if (fieldName[0]=='p' && strcmp(fieldName, "prikaz")==0) return base+6;
    if (fieldName[0]=='v' && strcmp(fieldName, "visID")==0) return base+7;
    if (fieldName[0]=='a' && strcmp(fieldName, "approaching")==0) return base+8;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *VeinsRSUPacketDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "long",
        "simtime_t",
        "simtime_t",
        "double",
        "double",
        "string",
        "int",
        "int",
        "bool",
    };
    return (field>=0 && field<9) ? fieldTypeStrings[field] : nullptr;
}

const char **VeinsRSUPacketDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *VeinsRSUPacketDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int VeinsRSUPacketDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    VeinsRSUPacket *pp = (VeinsRSUPacket *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *VeinsRSUPacketDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    VeinsRSUPacket *pp = (VeinsRSUPacket *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string VeinsRSUPacketDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    VeinsRSUPacket *pp = (VeinsRSUPacket *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getSequenceNumber());
        case 1: return simtime2string(pp->getCasOdoslania());
        case 2: return simtime2string(pp->getFirstSent());
        case 3: return double2string(pp->getPosX());
        case 4: return double2string(pp->getPosY());
        case 5: return oppstring2string(pp->getSequence());
        case 6: return long2string(pp->getPrikaz());
        case 7: return long2string(pp->getVisID());
        case 8: return bool2string(pp->getApproaching());
        default: return "";
    }
}

bool VeinsRSUPacketDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    VeinsRSUPacket *pp = (VeinsRSUPacket *)object; (void)pp;
    switch (field) {
        case 0: pp->setSequenceNumber(string2long(value)); return true;
        case 1: pp->setCasOdoslania(string2simtime(value)); return true;
        case 2: pp->setFirstSent(string2simtime(value)); return true;
        case 3: pp->setPosX(string2double(value)); return true;
        case 4: pp->setPosY(string2double(value)); return true;
        case 5: pp->setSequence((value)); return true;
        case 6: pp->setPrikaz(string2long(value)); return true;
        case 7: pp->setVisID(string2long(value)); return true;
        case 8: pp->setApproaching(string2bool(value)); return true;
        default: return false;
    }
}

const char *VeinsRSUPacketDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

void *VeinsRSUPacketDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    VeinsRSUPacket *pp = (VeinsRSUPacket *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

} // namespace inet

