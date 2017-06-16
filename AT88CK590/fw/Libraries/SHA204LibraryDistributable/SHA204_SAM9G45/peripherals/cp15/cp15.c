/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support 
 * ----------------------------------------------------------------------------
 * Copyright (c) 2008, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

//-----------------------------------------------------------------------------
// Reg Reads                    Writes
//----------------------------------------------------------------------------
// 0   ID code                  Unpredictable
// 0   cache type               Unpredictable
// 0   TCM status               Unpredictable
// 1   Control                  Control
// 2   Translation table base   Translation table base
// 3   Domain access control    Domain access control
// 4                                                       (Reserved)    
// 5   Data fault status        Data fault status
// 5   Instruction fault status Instruction fault status
// 6   Fault address            Fault address
// 7   cache operations         cache operations
// 8   Unpredictable            TLB operations
// 9   cache lockdown           cache lockdown
// 9   TCM region               TCM region
// 10  TLB lockdown             TLB lockdown
// 11                                                      (Reserved) 
// 12                                                      (Reserved) 
// 13  FCSE PID                 FCSE PID
// 13  Context ID               Context ID
// 14                                                      (Reserved)             
// 15  Test configuration       Test configuration
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//         Headers
//-----------------------------------------------------------------------------

#include <board.h>

#ifdef CP15_PRESENT

#include <utility/trace.h>
#include "cp15.h"

#if defined(__ICCARM__)
#include <intrinsics.h>
#endif


//-----------------------------------------------------------------------------
//         Macros
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//         Defines
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//         Global functions
//-----------------------------------------------------------------------------


///////////////////////////////////////////////////////////////////////////////
/// CP15 c1
/// * I cache
/// * D cache
///////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
/// Check Instruction cache
/// \return 0 if I_cache disable, 1 if I_cache enable
//------------------------------------------------------------------------------
unsigned int CP15_IsIcacheEnabled(void)
{
    unsigned int control;

    control = CP15_ReadControl();
    return ((control & (1 << CP15_I_BIT)) != 0);
} 

//------------------------------------------------------------------------------
/// Enable Instruction cache
//------------------------------------------------------------------------------
void CP15_EnableIcache(void)
{
    unsigned int control;

    control = CP15_ReadControl();

    // Check if cache is disabled
    if ((control & (1 << CP15_I_BIT)) == 0) {

        control |= (1 << CP15_I_BIT);
        CP15_WriteControl(control);        
        TRACE_INFO("I cache enabled.\n\r");
    }
#if !defined(OP_BOOTSTRAP_on)
    else {

        TRACE_INFO("I cache is already enabled.\n\r");
    }
#endif
}

//------------------------------------------------------------------------------
/// Disable Instruction cache
//------------------------------------------------------------------------------
void CP15_DisableIcache(void)
{
    unsigned int control;

    control = CP15_ReadControl();

    // Check if cache is enabled
    if ((control & (1 << CP15_I_BIT)) != 0) {

        control &= ~(1 << CP15_I_BIT);
        CP15_WriteControl(control);        
        TRACE_INFO("I cache disabled.\n\r");
    }
    else {

        TRACE_INFO("I cache is already disabled.\n\r");
    }
} 

//------------------------------------------------------------------------------
/// Check MMU
/// \return 0 if MMU disable, 1 if MMU enable
//------------------------------------------------------------------------------
unsigned int CP15_IsMMUEnabled(void)
{
    unsigned int control;

    control = CP15_ReadControl();
    return ((control & (1 << CP15_M_BIT)) != 0);
} 

//------------------------------------------------------------------------------
/// Enable MMU
//------------------------------------------------------------------------------
void CP15_EnableMMU(void)
{
    unsigned int control;

    control = CP15_ReadControl();

    // Check if MMU is disabled
    if ((control & (1 << CP15_M_BIT)) == 0) {

        control |= (1 << CP15_M_BIT);
        CP15_WriteControl(control);        
        TRACE_INFO("MMU enabled.\n\r");
    }
    else {

        TRACE_INFO("MMU is already enabled.\n\r");
    }
}

//------------------------------------------------------------------------------
/// Disable MMU
//------------------------------------------------------------------------------
void CP15_DisableMMU(void)
{
    unsigned int control;

    control = CP15_ReadControl();

    // Check if MMU is enabled
    if ((control & (1 << CP15_M_BIT)) != 0) {

        control &= ~(1 << CP15_M_BIT);
        control &= ~(1 << CP15_C_BIT);
        CP15_WriteControl(control);        
        TRACE_INFO("MMU disabled.\n\r");
    }
    else {

        TRACE_INFO("MMU is already disabled.\n\r");
    }
}

//------------------------------------------------------------------------------
/// Check D_cache
/// \return 0 if D_cache disable, 1 if D_cache enable (with MMU of course)
//------------------------------------------------------------------------------
unsigned int CP15_IsDcacheEnabled(void)
{
    unsigned int control;

    control = CP15_ReadControl();
    return ((control & ((1 << CP15_C_BIT)||(1 << CP15_M_BIT))) != 0);
} 

//------------------------------------------------------------------------------
/// Enable Data cache
//------------------------------------------------------------------------------
void CP15_EnableDcache(void)
{
    unsigned int control;

    control = CP15_ReadControl();

    if( !CP15_IsMMUEnabled() ) {
        TRACE_ERROR("Do nothing: MMU not enabled\n\r");
    }
    else {
        // Check if cache is disabled
        if ((control & (1 << CP15_C_BIT)) == 0) {

            control |= (1 << CP15_C_BIT);
            CP15_WriteControl(control);        
            TRACE_INFO("D cache enabled.\n\r");
        }
        else {

            TRACE_INFO("D cache is already enabled.\n\r");
        }
    }
}

//------------------------------------------------------------------------------
/// Disable Data cache
//------------------------------------------------------------------------------
void CP15_DisableDcache(void)
{
    unsigned int control;

    control = CP15_ReadControl();

    // Check if cache is enabled
    if ((control & (1 << CP15_C_BIT)) != 0) {

        control &= ~(1 << CP15_C_BIT);
        CP15_WriteControl(control);        
        TRACE_INFO("D cache disabled.\n\r");
    }
    else {

        TRACE_INFO("D cache is already disabled.\n\r");
    }
}

//----------------------------------------------------------------------------
/// Lock I cache
/// \param I cache index
//----------------------------------------------------------------------------
void CP15_LockIcache(unsigned int index)
{
    unsigned int victim = 0;

    // invalidate all the cache (4 ways) 
    CP15_InvalidateIcache();
    
    // lockdown all the ways except this in parameter
    victim =  CP15_ReadIcacheLockdown();
    victim = 0;
    victim |= ~index;
    victim &= 0xffff;
    CP15_WriteIcacheLockdown(victim);
}

//----------------------------------------------------------------------------
/// Lock D cache
/// \param D cache way
//----------------------------------------------------------------------------
void CP15_LockDcache(unsigned int index)
{
    unsigned int victim = 0;

    // invalidate all the cache (4 ways)    
    CP15_InvalidateDcache();
    
    // lockdown all the ways except this in parameter    
    victim =  CP15_ReadDcacheLockdown();
    victim = 0;
    victim |= ~index;
    victim &= 0xffff;
    CP15_WriteDcacheLockdown(victim);
}

//----------------------------------------------------------------------------
/// Lock D cache
/// \param D cache way
//----------------------------------------------------------------------------
void CP15_ShutdownDcache(void)
{ 
    CP15_TestCleanInvalidateDcache();  
    CP15_DrainWriteBuffer();
    CP15_DisableDcache();
    CP15_InvalidateTLB();      
}

#endif // CP15_PRESENT

