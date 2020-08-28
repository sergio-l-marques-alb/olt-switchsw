/** \file dnxc_verify.h
 *
 * Skip verify support.
 *
 */
#ifndef __DNXC_VERIFY_H__
#define __DNXC_VERIFY_H__

#define DNXC_VERIFICATION

#ifdef DNXC_VERIFICATION

extern int dnxc_verify_allowed[];

 /*
  * Get the value of the verification flag.
  */
#define DNXC_VERIFY_ALLOWED_GET(_unit)\
        dnxc_verify_allowed[unit]

/*
 * Invoke verification if it allowed.
 */
#define DNXC_VERIFY_INVOKE(_expr)\
    if (DNXC_VERIFY_ALLOWED_GET(unit)) \
    {\
        _expr;\
    }

#else /* DNXC_VERIFICATION */
#define DNXC_VERIFY_INVOKE(_expr)
#define DNXC_VERIFY_ALLOWED_GET(_unit) 0
#endif /* DNXC_VERIFICATION */

/**
* \brief
*   Initialize dnxc verify flags, by the "init_verify" soc property value.
*
*  \param [in] unit - Relevant unit.
*
*  \return
*    None
*
*  \remark
*    None
*  \see
*    None
*****************************************************/
void dnxc_verify_allowed_init(
    int unit);

/**
* \brief
*   Set dnxc_init_allowed flag to a specified value.
*
*  \param [in] unit - Relevant unit.
*  \param [in] value - Flag's new value.
*
*  \return
*    None
*
*  \remark
*    None
*  \see
*    None
*****************************************************/
void dnxc_verify_allowed_set(
    int unit,
    int value);

/**
* \brief
*   Return if dnx verify is allowed.
*
*  \param [in] unit - Relevant unit.
*
*  \return
*    None
*
*  \remark
*    None
*  \see
*    None
*****************************************************/
int dnxc_verify_allowed_get(
    int unit);

#endif /* __DNXC_VERIFY_H__ */
