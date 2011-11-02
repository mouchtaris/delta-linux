#include <tao/AnyTypeCode/Any_Dual_Impl_T.h>
#include <tao/AnyTypeCode/Any_SystemException.h>
// Specializations for CORBA::Exception Any operators.

#ifndef ACE_TRY_NEW_ENV
#define ACE_TRY_NEW_ENV try
#endif

#ifndef ACE_CATCHANY
#define ACE_CATCHANY catch (const CORBA::Exception&)
#endif

#ifndef ACE_TRY_CHECK
#define ACE_TRY_CHECK
#endif

#ifndef ACE_ENDTRY
#define ACE_ENDTRY
#endif

#ifndef ACE_ENV_ARG_PARAMETER
#define ACE_ENV_ARG_PARAMETER
#endif

namespace TAO
{
  template<>
  void
  Any_Dual_Impl_T<CORBA::Exception>::value (
      const CORBA::Exception & val
    )
  {
    this->value_ = val._tao_duplicate ();
  }

  template<>
  CORBA::Boolean
  Any_Dual_Impl_T<CORBA::Exception>::marshal_value (TAO_OutputCDR &cdr)
  {
    ACE_TRY_NEW_ENV
      {
        this->value_->_tao_encode (cdr
                                   ACE_ENV_ARG_PARAMETER);
        ACE_TRY_CHECK;

        return 1;
      }
    ACE_CATCHANY
      {
      }
    ACE_ENDTRY;

    return 0;
  }

  template<>
  CORBA::Boolean
  Any_Dual_Impl_T<CORBA::Exception>::demarshal_value (TAO_InputCDR &cdr)
  {
    ACE_TRY_NEW_ENV
      {
        this->value_->_tao_decode (cdr
                                   ACE_ENV_ARG_PARAMETER);
        ACE_TRY_CHECK;

        return 1;
      }
    ACE_CATCHANY
      {
      }
    ACE_ENDTRY;

    return 0;
  }

  // This should never get called since we don't have extraction operators
  // for CORBA::Exception, but it is here to sidestep the constructor call
  // in the unspecialized version that causes a problem with compilers that
  // require explicit instantiation
  template<>
  CORBA::Boolean
  Any_Dual_Impl_T<CORBA::Exception>::extract (
      const CORBA::Any &,
      _tao_destructor,
      CORBA::TypeCode_ptr,
      const CORBA::Exception *&
    )
  {
    return 0;
  }
}

// =======================================================================

// Insertion of CORBA::Exception - copying.
void
operator<<= (CORBA::Any &any, const CORBA::Exception &exception)
{
  TAO::Any_Dual_Impl_T<CORBA::Exception>::insert_copy (
      any,
      CORBA::Exception::_tao_any_destructor,
      exception._tao_type (),
      exception
    );
}

// Insertion of CORBA::Exception - non-copying.
void
operator<<= (CORBA::Any &any, CORBA::Exception *exception)
{
  TAO::Any_Dual_Impl_T<CORBA::Exception>::insert (
      any,
      CORBA::Exception::_tao_any_destructor,
      exception->_tao_type (),
      exception
    );
}

#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)

template class TAO::Any_Dual_Impl_T<CORBA::Exception>;

#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)

#pragma instantiate TAO::Any_Dual_Impl_T<CORBA::Exception>

#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */