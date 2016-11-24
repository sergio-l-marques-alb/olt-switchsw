#include "k_private_base.h"

L7_BOOL
SafeMakeOctetString(OctetString **os_ptr, L7_uchar8 *string, L7_uint32 length)
{
  OctetString *temp_os_ptr;

  temp_os_ptr = MakeOctetString(string, length);

  if (temp_os_ptr != NULL)
  {
    FreeOctetString(*os_ptr);
    *os_ptr = temp_os_ptr;
    return L7_TRUE;
  }

  return L7_FALSE;
}

L7_BOOL
SafeMakeOctetStringFromText(OctetString **os_ptr, L7_uchar8 *string)
{
  OctetString *temp_os_ptr;

  temp_os_ptr = MakeOctetStringFromText(string);

  if (temp_os_ptr != NULL)
  {
    FreeOctetString(*os_ptr);
    *os_ptr = temp_os_ptr;
    return L7_TRUE;
  }

  return L7_FALSE;
}

L7_BOOL
SafeMakeOctetStringFromTextExact(OctetString **os_ptr, L7_uchar8 *string)
{
  return SafeMakeOctetString(os_ptr, string, strlen(string));
}

L7_BOOL
SafeMakeOIDFromDot(OID **oid_ptr, L7_uchar8 *string)
{
  OID *temp_oid_ptr;

  temp_oid_ptr = MakeOIDFromDot(string);

  if (temp_oid_ptr != NULL)
  {
    FreeOID(*oid_ptr);
    *oid_ptr = temp_oid_ptr;
    return L7_TRUE;
  }
  return L7_FALSE;
}

L7_BOOL
SafeMakeTimeTicksFromSeconds(L7_uint32 seconds, L7_uint32 *time_ticks)
{
    *time_ticks = seconds * 100;
    return L7_TRUE;
}

L7_BOOL
SafeMakeDateAndTime(OctetString **os_ptr, L7_uint32 time)
{
  OctetString *temp_os_ptr;

  struct tm *tm;

  tm = localtime((void *)&time);

  temp_os_ptr = MakeDateAndTime(tm);

  if (temp_os_ptr != NULL)
  {
    FreeOctetString(*os_ptr);
    *os_ptr = temp_os_ptr;
    return L7_TRUE;
  }
  return L7_FALSE;
}
