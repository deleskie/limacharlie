/*
 Copyright 2015 refractionPOINT
 
 Licensed under the Apache License, Version 2.0 ( the "License" );
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
 
 http ://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

#define RPAL_PLATFORM_MACOSX

#include <sys/systm.h>
#include <mach/mach_types.h>
#include <sys/kern_control.h>

#include <sys/types.h>

#include "collectors.h"
#include "helpers.h"


static struct kern_ctl_reg krnlCommsCtl = { 0 };
static kern_ctl_ref krnlCommsRef = { 0 };
static rMutex g_client_mutex = NULL;


kern_return_t hbs_kernel_acquisition_start(kmod_info_t * ki, void *d);
kern_return_t hbs_kernel_acquisition_stop(kmod_info_t *ki, void *d);

typedef struct
{
    int (*initializer)();
    int (*deinitializer)();
} CollectorContext;

#define _COLLECTOR_INIT(cId) { collector_ ## cId ## _initialize, collector_ ## cId ## _deinitialize }

//=========================================================================
//  Built-in Tasks
//=========================================================================
static
int
task_ping
(
 void* pArgs,
 int argsSize,
 void* pResult,
 uint32_t* resultSize
 )
{
    int ret = 0;
    
    if( NULL != pArgs &&
       sizeof(uint32_t) == argsSize &&
       NULL != pResult &&
       NULL != resultSize &&
       sizeof(uint32_t) == *resultSize &&
       ACQUISITION_COMMS_CHALLENGE == *(uint32_t*)pArgs )
    {
        *(uint32_t*)pResult = ACQUISITION_COMMS_RESPONSE;
        *resultSize = sizeof(uint32_t);
    }
    else
    {
        ret = EINVAL;
        rpal_debug_error( "invalid challenge: %p:%d / %p:%d",
                         pArgs,
                         argsSize,
                         pResult,
                         *resultSize );
    }
    
    return ret;
}

//=========================================================================
//  Dispatcher
//=========================================================================
static CollectorContext g_collectors[] = { _COLLECTOR_INIT( 1 ) };
static collector_task g_tasks[] = { task_ping,
    task_get_new_processes };

static
int
um_dispatcher
(
 int op,
 KernelAcqCommand* cmd
 )
{
    int error = 0;
    void* pLocalArgs = NULL;
    void* pLocalRes = NULL;
    uint32_t resSize = 0;
    
    if( NULL != cmd )
    {
        rpal_debug_info( "OP: %d, ARG: %p, ARGS: %d, RES: %p, RESS: %d",
                        op,
                        cmd->pArgs,
                        cmd->argsSize,
                        cmd->pResult,
                        cmd->resultSize );
        
        if( op >= ARRAY_N_ELEM( g_tasks ) )
        {
            rpal_debug_error( "invalid op specified: %d", op );
            error = EINVAL;
            return error;
        }
        
        resSize = cmd->resultSize;
        
        if( NULL != cmd->pArgs &&
           0 != cmd->argsSize )
        {
            pLocalArgs = rpal_memory_alloc( cmd->argsSize );
        }
        if( NULL != cmd->pResult &&
           0 != cmd->resultSize )
        {
            pLocalRes = rpal_memory_alloc( cmd->resultSize );
        }
        
        if( ( NULL != pLocalArgs ||
             NULL == cmd->pArgs ||
             0 == cmd->argsSize ) &&
           ( NULL != pLocalRes ||
            NULL == cmd->pResult ||
            0 == cmd->resultSize ) )
        {
            if( NULL == pLocalArgs ||
               0 == copyin( (user_addr_t)cmd->pArgs, pLocalArgs, cmd->argsSize ) )
            {
                error = g_tasks[ op ]( pLocalArgs, cmd->argsSize, pLocalRes, &resSize );
                
                if( 0 != error )
                {
                    rpal_debug_error( "op returned error: %d", error );
                }
                else
                {
                    rpal_debug_info( "op success" );
                    
                    if( NULL != cmd->pResult &&
                       0 != resSize &&
                       0 != copyout( pLocalRes, (user_addr_t)cmd->pResult, resSize ) )
                    {
                        rpal_debug_error( "error copying out results: %p %d", cmd->pResult, resSize );
                        error = ENOMEM;
                    }
                    else if( NULL != cmd->pSizeUsed &&
                            0 != copyout( &resSize, (user_addr_t)cmd->pSizeUsed, sizeof(uint32_t) ) )
                    {
                        rpal_debug_error( "error copying out size used: %p %d", cmd->pSizeUsed, resSize );
                        error = ENOMEM;
                    }
                }
            }
            else
            {
                rpal_debug_error( "error copying in arguments: %p %d", cmd->pArgs, cmd->argsSize );
                error = ENOMEM;
            }
        }
        else
        {
            rpal_debug_error( "could not allocate memory for arguments or results: %d / %d", cmd->argsSize, cmd->resultSize );
            error = ENOMEM;
        }
        
        if( NULL != pLocalArgs ) rpal_memory_free( pLocalArgs );
        if( NULL != pLocalRes ) rpal_memory_free( pLocalRes );
    }
    
    return error;
}

//=========================================================================
//  Kernel / User Comms
//=========================================================================
static
errno_t
comms_handle_send
(
 kern_ctl_ref ctlref,
 unsigned int unit,
 void *userdata,
 mbuf_t m,
 int flags
 )
{
    int error = EINVAL;
    
    return error;
}

static
errno_t
comms_handle_get
(
 kern_ctl_ref ctlref,
 unsigned int unit,
 void *userdata, int opt,
 void *data,
 size_t *len
 )
{
    int error = EINVAL;
    
    return error;
}

static
errno_t
comms_handle_set
(
 kern_ctl_ref ctlref,
 unsigned int unit,
 void *userdata,
 int opt,
 void *data,
 size_t len
 )
{
    int error = EINVAL;
    
    rpal_debug_info( "received request" );
    
    if( NULL != data &&
       sizeof(KernelAcqCommand) <= len )
    {
        rpal_debug_info( "calling dispatcher" );
        
        error = um_dispatcher( opt, data );
    }
    else
    {
        rpal_debug_critical( "not enough data for request" );
    }
    
    rpal_debug_info( "returned status: %d", error );
    
    return error;
}

static
errno_t
comms_handle_connect
(
 kern_ctl_ref ctlref,
 struct sockaddr_ctl *sac,
 void **unitinfo
 )
{
    rpal_debug_info( "received connection request, blocking until available" );
    rpal_mutex_lock( g_client_mutex );
    
    return (0);
}

static
errno_t
comms_handle_disconnect
(
 kern_ctl_ref ctlref,
 unsigned int unit,
 void *unitinfo
 )
{
    rpal_mutex_unlock( g_client_mutex );
    
    return 0;
}

//=========================================================================
//  Entry Points
//=========================================================================

kern_return_t hbs_kernel_acquisition_start(kmod_info_t * ki, void *d)
{
    errno_t error = 0;
    int i = 0;
    
    if( NULL == ( g_client_mutex = rpal_mutex_create() ) )
    {
        rpal_debug_critical( "could not create client mutex" );
    }
    
    rpal_debug_info( "Initializing collectors" );
    
    for( i = 0; i < ARRAY_N_ELEM( g_collectors ); i++ )
    {
        if( !g_collectors[ i ].initializer( d ) )
        {
            rpal_debug_critical( "error initializing collector %d", i );
            error = EBADEXEC;
            break;
        }
    }
    
    if( 0 != error )
    {
        for( i = i - 1; i > 0; i-- )
        {
            g_collectors[ i ].deinitializer();
        }
    }
    
    if( 0 == error )
    {
        rpal_debug_info( "collectors OK" );
        rpal_debug_info( "initializing KM/UM comms" );
        
        krnlCommsCtl.ctl_id = 0;
        krnlCommsCtl.ctl_unit = 0;
        strncpy( krnlCommsCtl.ctl_name, ACQUISITION_COMMS_NAME, sizeof(krnlCommsCtl.ctl_name) );
        krnlCommsCtl.ctl_flags = CTL_FLAG_PRIVILEGED;
        krnlCommsCtl.ctl_send = comms_handle_send;
        krnlCommsCtl.ctl_getopt = comms_handle_get;
        krnlCommsCtl.ctl_setopt = comms_handle_set;
        krnlCommsCtl.ctl_connect = comms_handle_connect;
        krnlCommsCtl.ctl_disconnect = comms_handle_disconnect;
        
        error = ctl_register( &krnlCommsCtl, &krnlCommsRef );
        if( 0 == error )
        {
            rpal_debug_info( "KM/UM comms initialized OK" );
        }
        else
        {
            rpal_debug_critical( "KM/UM comms initialize error: %d", error );
        }
    }
    
    return KERN_SUCCESS;
}

kern_return_t hbs_kernel_acquisition_stop(kmod_info_t *ki, void *d)
{
    errno_t error = 0;
    int i = 0;
    
    rpal_debug_info( "waiting until client is disconnected" );
    rpal_mutex_lock( g_client_mutex );
    
    rpal_debug_info( "stopping collectors" );
    for( i = 0; i < ARRAY_N_ELEM( g_collectors ); i++ )
    {
        if( !g_collectors[ i ].deinitializer() )
        {
            rpal_debug_critical( "error deinitializing collector %d", i );
        }
    }
    
    rpal_debug_info( "unregistering KM/UM comms" );
    error = ctl_deregister( krnlCommsRef );
    if( 0 == error )
    {
        rpal_debug_info( "KM/UM comms unregistered" );
    }
    else
    {
        rpal_debug_critical( "error unregistering KM/UM comms: %d", error );
    }
    
    rpal_mutex_free( g_client_mutex );
    g_client_mutex = NULL;
    
    return KERN_SUCCESS;
}
