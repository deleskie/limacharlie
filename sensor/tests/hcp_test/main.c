#include <rpal/rpal.h>
#include <librpcm/librpcm.h>
#include <networkLib/networkLib.h>
#include <cryptoLib/cryptoLib.h>
#include <Basic.h>

#include <../lib/rpHostCommonPlatformLib/_private.h>
#include <rpHostCommonPlatformLib/rTags.h>

#define RPAL_FILE_ID     92

RU16 g_server_port = 9199;
RU8 g_key[ CRYPTOLIB_SYM_KEY_SIZE ] = { 0 };
RU8 g_iv[ CRYPTOLIB_SYM_IV_SIZE ] = { 0 };
rEvent g_serverStop = NULL;
rEvent g_isClean = NULL;

RU8 g_test_priv[ CRYPTOLIB_ASYM_KEY_SIZE_PRI ] = {
    0x30, 0x82, 0x04, 0xa2, 0x02, 0x01, 0x00, 0x02, 0x82, 0x01, 0x01, 0x00,
    0xe2, 0x0d, 0x17, 0x5e, 0x09, 0xdf, 0xe3, 0x1a, 0x98, 0xbe, 0x00, 0x2b,
    0x56, 0x3d, 0xce, 0x2d, 0xc9, 0x2b, 0x05, 0xaf, 0x11, 0x7d, 0xaf, 0xfd,
    0x8f, 0x4f, 0x19, 0x4d, 0x10, 0x29, 0xfd, 0xa4, 0x23, 0x30, 0x5a, 0x2e,
    0x68, 0x97, 0xa8, 0x33, 0xef, 0x54, 0x8f, 0xb3, 0xe8, 0x8b, 0xca, 0x0d,
    0xc8, 0xa7, 0x5a, 0xc8, 0x6b, 0xdc, 0xe2, 0xbe, 0x40, 0x35, 0x9c, 0xb0,
    0xbd, 0xf5, 0xd8, 0xdc, 0xe9, 0xe6, 0x78, 0x37, 0x80, 0xe7, 0x04, 0x86,
    0x9c, 0x1c, 0xf0, 0xdc, 0xe9, 0x22, 0x2c, 0x7d, 0xbd, 0x06, 0xbf, 0xa9,
    0x6b, 0xd6, 0xc3, 0x89, 0x67, 0x74, 0x1d, 0xae, 0xa2, 0xd6, 0x57, 0x57,
    0xfe, 0xe5, 0xf5, 0xb7, 0x22, 0x0b, 0xf2, 0x27, 0x0e, 0xf7, 0x59, 0xaf,
    0xa5, 0x7b, 0xf1, 0x3a, 0x9c, 0xa2, 0xbf, 0x2a, 0x8e, 0xcd, 0x2e, 0x2c,
    0x3f, 0xa1, 0x79, 0x4e, 0xeb, 0xd1, 0xb2, 0xbb, 0xad, 0xa1, 0xfd, 0x32,
    0xc5, 0x76, 0x24, 0x9c, 0x00, 0x38, 0x32, 0x83, 0xd8, 0x5a, 0x69, 0xe6,
    0x92, 0x2c, 0xb8, 0x0c, 0x77, 0x9c, 0x77, 0x05, 0x2a, 0x6b, 0x35, 0xd7,
    0x76, 0x93, 0x4e, 0x77, 0x75, 0x97, 0x27, 0x8c, 0xa5, 0xa6, 0xb0, 0x61,
    0xd4, 0xed, 0x53, 0xc3, 0x31, 0x89, 0x8b, 0xc5, 0xe8, 0x35, 0x6e, 0x43,
    0x1a, 0x45, 0x57, 0xd4, 0x14, 0x27, 0xe6, 0xad, 0x83, 0xbc, 0xaf, 0xf5,
    0x9e, 0xbb, 0x8b, 0xbf, 0xee, 0xc2, 0x0c, 0xe3, 0xc5, 0xb9, 0x75, 0x03,
    0x10, 0x4f, 0x53, 0x2b, 0xd3, 0xe8, 0x6b, 0xf7, 0x96, 0x3f, 0x5b, 0x35,
    0x38, 0x06, 0x4e, 0x92, 0xb4, 0x2b, 0xfc, 0x69, 0xcf, 0xdb, 0xcc, 0xc5,
    0x66, 0x41, 0xa7, 0xad, 0xb8, 0x77, 0x3b, 0x8a, 0xf4, 0xc3, 0xf0, 0xa2,
    0x7b, 0x76, 0xb9, 0xfd, 0xf1, 0xc5, 0xed, 0x7e, 0xe5, 0xf9, 0x5f, 0x7c,
    0x4d, 0x3c, 0xbe, 0x95, 0x02, 0x03, 0x01, 0x00, 0x01, 0x02, 0x82, 0x01,
    0x00, 0x4f, 0xab, 0x81, 0x82, 0xd9, 0x44, 0x19, 0x4c, 0xb4, 0x6c, 0xb2,
    0xae, 0x93, 0x74, 0xb1, 0x08, 0x1e, 0x10, 0x95, 0xae, 0x40, 0x21, 0xe6,
    0x37, 0x1d, 0x9d, 0x29, 0x33, 0xa9, 0xf0, 0xc2, 0xbe, 0x68, 0xd0, 0x8f,
    0xd8, 0x2f, 0x70, 0xd4, 0x0d, 0x89, 0xda, 0x69, 0xd3, 0x64, 0xb0, 0x70,
    0x04, 0x2f, 0xa6, 0x1c, 0x2e, 0xeb, 0x25, 0x2c, 0x9d, 0x15, 0x10, 0x50,
    0x2b, 0x52, 0xa8, 0x25, 0x48, 0xa6, 0xff, 0x5d, 0x7b, 0x62, 0x69, 0x09,
    0x85, 0x4f, 0xdc, 0x3b, 0x60, 0x00, 0xa2, 0xd4, 0xbf, 0x74, 0x31, 0xbe,
    0xb7, 0x3d, 0x94, 0xb1, 0xd1, 0xe9, 0xae, 0x59, 0x0f, 0xc5, 0xc0, 0x79,
    0x84, 0x40, 0x08, 0x58, 0x73, 0xb5, 0x02, 0x34, 0xd4, 0x63, 0x47, 0xf6,
    0x4a, 0x1b, 0xae, 0xf5, 0x34, 0x4d, 0x24, 0x66, 0xef, 0xde, 0xe8, 0x9d,
    0xcf, 0x66, 0x95, 0xc8, 0xa5, 0x72, 0x23, 0xb4, 0x34, 0x1b, 0xb6, 0x30,
    0x1a, 0xc7, 0x17, 0xaf, 0x58, 0xa2, 0x38, 0xee, 0x32, 0x51, 0x72, 0x44,
    0xa0, 0xd5, 0x91, 0x3b, 0xca, 0x82, 0xd0, 0x79, 0xcc, 0x52, 0xab, 0x38,
    0x4d, 0xcb, 0xa9, 0xb2, 0x83, 0xf3, 0x2b, 0x58, 0xb0, 0x9d, 0x63, 0x27,
    0x31, 0xb8, 0xd0, 0xde, 0x62, 0xd6, 0xbb, 0xd8, 0xb0, 0xbd, 0x15, 0x45,
    0xc1, 0xc8, 0xed, 0xa0, 0xf1, 0x02, 0x2b, 0xe7, 0x8b, 0x3c, 0x3d, 0x21,
    0xc3, 0x2e, 0x4b, 0x0b, 0xef, 0x9e, 0xf9, 0x3e, 0xd1, 0xf8, 0x10, 0x72,
    0xbf, 0x5f, 0x03, 0x56, 0x74, 0x02, 0x5a, 0xaf, 0xb8, 0x27, 0x25, 0x5e,
    0x24, 0x1a, 0x26, 0xe0, 0x4c, 0x5c, 0x80, 0x56, 0x42, 0x08, 0x8e, 0x6f,
    0xd2, 0xf0, 0x38, 0xe1, 0x7d, 0xc3, 0x99, 0xef, 0xaa, 0x39, 0xb6, 0x92,
    0x36, 0x11, 0x80, 0x9e, 0xf5, 0xd5, 0x84, 0xd1, 0x61, 0x1a, 0x68, 0xf8,
    0x8e, 0x6e, 0xc7, 0x17, 0xc1, 0x02, 0x81, 0x81, 0x00, 0xf8, 0x02, 0xc3,
    0x61, 0x03, 0x5a, 0x8b, 0xe5, 0xec, 0x82, 0xcb, 0x89, 0x33, 0xa8, 0xc5,
    0xeb, 0xd2, 0xc3, 0x7f, 0xd6, 0xc1, 0xd5, 0x16, 0xfc, 0x2e, 0x7c, 0x18,
    0x52, 0xa2, 0xb4, 0xdd, 0x54, 0x03, 0xee, 0xa3, 0xc8, 0x68, 0xbb, 0xc3,
    0x1a, 0x69, 0x90, 0x7a, 0x85, 0xd1, 0x82, 0xd4, 0x13, 0x09, 0xea, 0x86,
    0x17, 0x69, 0x3f, 0x86, 0xfa, 0xfa, 0x31, 0x71, 0x55, 0x5f, 0xa3, 0xc6,
    0x97, 0xf5, 0xa1, 0x0a, 0x8d, 0xce, 0x6b, 0x09, 0x7c, 0xdf, 0xe8, 0x6e,
    0x4d, 0xa8, 0x9c, 0xff, 0x82, 0x09, 0x45, 0x3d, 0xa9, 0x65, 0x6c, 0xbd,
    0xa7, 0x87, 0xc2, 0xb9, 0xdb, 0x78, 0xc6, 0xa7, 0x61, 0x6a, 0xa5, 0x56,
    0xd6, 0xc9, 0x7a, 0x40, 0x5a, 0x64, 0xe7, 0xd0, 0x9c, 0x19, 0xfc, 0xf8,
    0x82, 0xdf, 0x6b, 0x09, 0xb5, 0xbb, 0xf7, 0xfe, 0x43, 0x99, 0xa2, 0x9a,
    0x77, 0xd0, 0x63, 0xde, 0x2d, 0x02, 0x81, 0x81, 0x00, 0xe9, 0x55, 0x3c,
    0x83, 0x51, 0xf6, 0x76, 0x81, 0x42, 0x5a, 0x01, 0x66, 0x10, 0xd6, 0x5e,
    0x96, 0x1c, 0x27, 0x91, 0xda, 0x88, 0x6a, 0x55, 0xfc, 0x1f, 0x56, 0xc1,
    0x34, 0xe5, 0x57, 0x77, 0xf8, 0x39, 0x13, 0x3f, 0x62, 0x31, 0xec, 0xfb,
    0x5d, 0xfc, 0x14, 0xa9, 0x51, 0xb6, 0x22, 0x9b, 0xfb, 0xf4, 0x1a, 0xaf,
    0x86, 0x14, 0x1c, 0xf5, 0x81, 0x16, 0xa0, 0xad, 0x6f, 0x54, 0x59, 0x7b,
    0x7b, 0xc3, 0x0e, 0xd2, 0x07, 0x83, 0x2b, 0x9d, 0x1c, 0xd4, 0x74, 0xa0,
    0xa8, 0x0a, 0x96, 0xfa, 0xb7, 0x75, 0x93, 0xc0, 0x92, 0x85, 0x2b, 0xd7,
    0xdb, 0x66, 0x98, 0x18, 0xb3, 0xfc, 0xa2, 0x8c, 0x22, 0x68, 0xe3, 0x1c,
    0x57, 0xa2, 0xeb, 0xc8, 0x29, 0xd7, 0x46, 0xc3, 0xa9, 0xd5, 0xd3, 0x45,
    0xbe, 0x41, 0xff, 0x7d, 0xef, 0xe4, 0xd2, 0xc1, 0xbf, 0x3c, 0x2d, 0xc5,
    0xee, 0xd6, 0xa6, 0x0b, 0x09, 0x02, 0x81, 0x80, 0x14, 0x5a, 0x15, 0xb2,
    0x66, 0x56, 0x0d, 0xd8, 0xe6, 0x59, 0xad, 0x5e, 0x2a, 0xf1, 0x14, 0x9c,
    0xcf, 0x84, 0xdf, 0x7e, 0xbe, 0x15, 0x40, 0x77, 0x9d, 0x50, 0xed, 0xa6,
    0x0a, 0x58, 0x11, 0x39, 0x8d, 0xc1, 0x0d, 0x8b, 0xaa, 0x9c, 0x8d, 0xdc,
    0x23, 0x01, 0x8a, 0x2c, 0x65, 0x0e, 0xf9, 0x5c, 0x3e, 0x9a, 0x12, 0x3a,
    0xe5, 0x96, 0x22, 0xfb, 0x81, 0x80, 0x81, 0x2e, 0x62, 0x54, 0xcc, 0x3d,
    0x38, 0x18, 0xe9, 0x06, 0xce, 0x03, 0x96, 0x99, 0xba, 0xbe, 0x16, 0x1f,
    0x25, 0x51, 0x0c, 0xde, 0x9c, 0xe8, 0x23, 0xb1, 0x3c, 0xf3, 0x79, 0x74,
    0xbf, 0xfd, 0x48, 0xc7, 0x2b, 0xd8, 0x11, 0x70, 0x3b, 0x12, 0x2a, 0x58,
    0x21, 0xdc, 0x78, 0x5f, 0x72, 0x9c, 0x5b, 0x50, 0x14, 0x09, 0xe0, 0x52,
    0xcd, 0x06, 0xb9, 0x9b, 0x42, 0x3b, 0x8d, 0x8f, 0xfa, 0x91, 0x8f, 0x5f,
    0x79, 0xa8, 0x58, 0x59, 0x02, 0x81, 0x80, 0x45, 0xc0, 0x14, 0x29, 0xd1,
    0x6b, 0x92, 0xbb, 0x4b, 0x21, 0x6a, 0xbf, 0x9f, 0x12, 0xe6, 0x0d, 0x8b,
    0x77, 0x0c, 0xb3, 0xbe, 0x09, 0xc3, 0x90, 0x6d, 0xb1, 0xda, 0x69, 0xbc,
    0x91, 0x2c, 0x2c, 0xc8, 0xab, 0x07, 0x4c, 0x8c, 0x4e, 0x22, 0x4a, 0x53,
    0x1b, 0xbe, 0x0d, 0xce, 0x1b, 0x84, 0x73, 0x89, 0xe2, 0x1c, 0x4c, 0x41,
    0x60, 0xf2, 0xc6, 0xd7, 0x03, 0xcb, 0x34, 0xb7, 0x8b, 0x54, 0x4d, 0x87,
    0xde, 0xbb, 0x4c, 0x41, 0x9a, 0xdd, 0x09, 0x8c, 0x3d, 0x1c, 0xf5, 0x7e,
    0xcc, 0xe9, 0x3f, 0xc4, 0x67, 0x19, 0xae, 0xf3, 0x1e, 0x4b, 0x15, 0xac,
    0x12, 0x95, 0x77, 0xe0, 0x72, 0x9a, 0xdf, 0x82, 0x69, 0x39, 0xc4, 0xbc,
    0xd0, 0x2c, 0xf2, 0xcd, 0x06, 0xbb, 0x60, 0xde, 0xa9, 0x9c, 0x74, 0xa6,
    0x78, 0x00, 0xa6, 0xa0, 0xc9, 0x9b, 0xdf, 0x55, 0xd6, 0xfd, 0x0f, 0x4b,
    0x9d, 0x31, 0x39, 0x02, 0x81, 0x80, 0x7a, 0xdb, 0x6f, 0xbc, 0x64, 0x75,
    0xf4, 0x27, 0x13, 0xf7, 0x7f, 0xd0, 0x35, 0x7f, 0x75, 0x65, 0x46, 0x4e,
    0xfa, 0xb7, 0xe1, 0xb4, 0xb7, 0x54, 0xcb, 0x9a, 0x4f, 0x4e, 0x66, 0xe4,
    0xb6, 0x3d, 0x35, 0x91, 0x63, 0x28, 0x73, 0x98, 0x3a, 0xa5, 0xa6, 0x39,
    0x53, 0x0f, 0x21, 0x06, 0x3e, 0xaa, 0xa6, 0x06, 0x80, 0x61, 0x69, 0xdd,
    0xfb, 0x89, 0xd1, 0xd7, 0x70, 0x49, 0xf4, 0x19, 0x54, 0x5b, 0x89, 0xa5,
    0x20, 0x18, 0xeb, 0x1f, 0xd5, 0x81, 0x46, 0xdb, 0x6b, 0xda, 0x47, 0x7e,
    0x3f, 0xa2, 0x6c, 0x8f, 0xad, 0xef, 0x21, 0x49, 0x0c, 0x43, 0x95, 0xef,
    0x7c, 0x1c, 0x37, 0xf2, 0x24, 0xda, 0xe2, 0xbd, 0x86, 0x65, 0x3d, 0x03,
    0x03, 0x56, 0xd5, 0x9d, 0x63, 0xab, 0x60, 0x1b, 0xb0, 0x58, 0xc5, 0xbb,
    0xb4, 0x11, 0xd7, 0xd4, 0x32, 0x6c, 0xea, 0x9f, 0xf7, 0xa5, 0x19, 0xc6,
    0x40, 0xf4 };

RU32
    threadServer
    (
        RPVOID ctx
    )
{
    RpHcp_ModuleId moduleId = 0;
    rList messages = NULL;
    rpHCPContext hcpCtx = { 0 };
    NetLibTcpConnection serverSock = 0;
    NetLibTcpConnection clientSock = 0;

    UNREFERENCED_PARAMETER( ctx );
    if( 0 != ( serverSock = NetLib_TcpListen( "localhost", g_server_port ) ) )
    {
        while( !rEvent_wait( g_serverStop, 0 ) )
        {
            if( 0 != ( clientSock = NetLib_TcpAccept( serverSock, g_serverStop, 0 ) ) )
            {
                hcpCtx.session.symSendCtx = CryptoLib_symEncInitContext( g_key, g_iv );
                hcpCtx.session.symRecvCtx = CryptoLib_symDecInitContext( g_key, g_iv );
                hcpCtx.cloudConnection = clientSock;

                while( recvFrame( &hcpCtx, &moduleId, &messages, 2 ) )  // Tested function
                {
                    sendFrame( &hcpCtx, moduleId, messages, TRUE );   // Tested function
                    rList_free( messages );
                }

                NetLib_TcpDisconnect( clientSock );

                CryptoLib_symFreeContext( hcpCtx.session.symSendCtx );
                CryptoLib_symFreeContext( hcpCtx.session.symRecvCtx );
            }
        }

        NetLib_TcpDisconnect( serverSock );
    }

    rEvent_set( g_isClean );
    
    return 0;
}

RBOOL
    getConnectionToServer
    (
        rpHCPContext* hcpCtx
    )
{
    RBOOL isConnected = FALSE;

    hcpCtx->cloudConnection = NetLib_TcpConnect( "localhost", g_server_port );
    CU_ASSERT_NOT_EQUAL_FATAL( hcpCtx->cloudConnection, 0 );

    hcpCtx->session.symRecvCtx = CryptoLib_symDecInitContext( g_key, g_iv );
    hcpCtx->session.symSendCtx = CryptoLib_symEncInitContext( g_key, g_iv );

    CU_ASSERT_NOT_EQUAL_FATAL( hcpCtx->session.symRecvCtx, NULL );
    CU_ASSERT_NOT_EQUAL_FATAL( hcpCtx->session.symSendCtx, NULL );

    isConnected = TRUE;

    return isConnected;
}

RVOID
    closeConnectionToServer
    (
        rpHCPContext* hcpCtx
    )
{
    CryptoLib_symFreeContext( hcpCtx->session.symRecvCtx );
    CryptoLib_symFreeContext( hcpCtx->session.symSendCtx );

    NetLib_TcpDisconnect( hcpCtx->cloudConnection );
}

void test_memoryLeaks(void)
{
    RU32 memUsed = 0;

    rpal_Context_cleanup();

    memUsed = rpal_memory_totalUsed();

    CU_ASSERT_EQUAL( memUsed, 0 );

    if( 0 != memUsed )
    {
        rpal_debug_critical( "Memory leak: %d bytes.\n", memUsed );
        printf( "\nMemory leak: %d bytes.\n", memUsed );

        rpal_memory_findMemory();
    }
}

void test_create_dummy_cloud( void )
{
    rThread hThread = NULL;

    g_isClean = rEvent_create( TRUE );
    CU_ASSERT_NOT_EQUAL_FATAL( g_isClean, NULL );
    g_serverStop = rEvent_create( TRUE );
    CU_ASSERT_NOT_EQUAL( g_serverStop, NULL );
    CU_ASSERT_TRUE_FATAL( CryptoLib_init() );

    // Setup cloud connectivity
    CU_ASSERT_TRUE_FATAL( CryptoLib_genRandomBytes( g_key, sizeof( g_key ) ) );
    CU_ASSERT_TRUE_FATAL( CryptoLib_genRandomBytes( g_iv, sizeof( g_iv ) ) );

    hThread = rpal_thread_new( threadServer, NULL );
    rpal_thread_free( hThread );
    rpal_thread_sleep( 2000 );
}

void test_destroy_dummy_cloud( void )
{
    rEvent_set( g_serverStop );
    CryptoLib_deinit();
    rEvent_wait( g_isClean, RINFINITE );
    rEvent_free( g_isClean );
    g_isClean = NULL;
    rEvent_free( g_serverStop );
    g_serverStop = NULL;
}

void test_frames( void )
{
    RpHcp_ModuleId moduleId = 1;
    RpHcp_ModuleId outMod = 0;
    rList messages = NULL;
    rList outMessages = NULL;
    RPU8 garbage = NULL;
    RU32 garbageMaxSize = 1024;
    RU32 garbageSize = 0;
    RU32 garbageLoops = 100;
    rBlob blob = NULL;
    
    // Create and test frames
    messages = rList_new( 1, RPCM_STRINGA );
    CU_ASSERT_NOT_EQUAL_FATAL( messages, NULL );

    CU_ASSERT_TRUE_FATAL( rList_addSTRINGA( messages, "str1" ) );
    CU_ASSERT_TRUE_FATAL( rList_addSTRINGA( messages, "str2" ) );
    CU_ASSERT_TRUE_FATAL( rList_addSTRINGA( messages, "str3" ) );
    CU_ASSERT_TRUE_FATAL( rList_addSTRINGA( messages, "str4" ) );

    blob = wrapFrame( moduleId, messages, TRUE );
    CU_ASSERT_NOT_EQUAL_FATAL( blob, NULL );

    CU_ASSERT( unwrapFrame( blob, &outMod, &outMessages ) );
    rpal_blob_free( blob );
    CU_ASSERT_EQUAL( outMod, moduleId );
    CU_ASSERT_NOT_EQUAL_FATAL( outMessages, NULL );

    CU_ASSERT_EQUAL( rList_getNumElements( outMessages ), rList_getNumElements( messages ) );
    CU_ASSERT_TRUE( rList_isEqual( messages, outMessages ) );

    rList_free( messages );
    rList_free( outMessages );
    
    // Fuzz the unwrapping function.
    for( garbageLoops = garbageLoops; 0 != garbageLoops; garbageLoops-- )
    {
        garbageSize = ( rpal_rand() % garbageMaxSize ) + 1;
        garbage = rpal_memory_alloc( garbageSize );
        CU_ASSERT_NOT_EQUAL_FATAL( garbage, NULL );
        CU_ASSERT_TRUE( CryptoLib_genRandomBytes( garbage, garbageSize ) );
        
        blob = rpal_blob_createFromBuffer( garbage, garbageSize );
        CU_ASSERT_NOT_EQUAL_FATAL( blob, NULL );

        CU_ASSERT_FALSE( unwrapFrame( blob, &outMod, &outMessages ) );

        rpal_blob_free( blob );
    }
}

void test_exchange_frames( void )
{
    rpHCPContext hcpCtx = { 0 };
    RpHcp_ModuleId moduleId = 1;
    RpHcp_ModuleId outMod = 0;
    rList messages = NULL;
    rList outMessages = NULL;
    RU8 garbage[ 1024 ] = { 0 };
    RU32 garbageSize = 0;
    RU32 tmpFrameSize = 0;
    RU32 garbageLoops = 20;
    
    // Connect to the fake server
    getConnectionToServer( &hcpCtx );

    // Create and test frames
    messages = rList_new( 1, RPCM_STRINGA );
    CU_ASSERT_NOT_EQUAL_FATAL( messages, NULL );

    CU_ASSERT_TRUE_FATAL( rList_addSTRINGA( messages, "str1" ) );
    CU_ASSERT_TRUE_FATAL( rList_addSTRINGA( messages, "str2" ) );
    CU_ASSERT_TRUE_FATAL( rList_addSTRINGA( messages, "str3" ) );
    CU_ASSERT_TRUE_FATAL( rList_addSTRINGA( messages, "str4" ) );

    CU_ASSERT_TRUE( sendFrame( &hcpCtx, moduleId, messages, TRUE ) );
    
    CU_ASSERT_TRUE( recvFrame( &hcpCtx, &outMod, &outMessages, 5 ) );
    CU_ASSERT_EQUAL( outMod, moduleId );

    CU_ASSERT_EQUAL( rList_getNumElements( outMessages ), rList_getNumElements( messages ) );
    CU_ASSERT_TRUE( rList_isEqual( messages, outMessages ) );
    
    rList_free( messages );
    rList_free( outMessages );

    closeConnectionToServer( &hcpCtx );

    // Send over garbage and check we don't crash or get anything back
    for( garbageLoops = garbageLoops; 0 != garbageLoops; garbageLoops-- )
    {
        getConnectionToServer( &hcpCtx );

        garbageSize = ( rpal_rand() % sizeof( garbage - 1 ) ) + 1;
        CU_ASSERT_TRUE( CryptoLib_genRandomBytes( garbage, garbageSize ) );
        CU_ASSERT_TRUE( NetLib_TcpSend( hcpCtx.cloudConnection, garbage, garbageSize, NULL ) );
        CU_ASSERT_FALSE( NetLib_TcpReceive( hcpCtx.cloudConnection, &tmpFrameSize, sizeof( tmpFrameSize ), NULL, 1 ) );

        closeConnectionToServer( &hcpCtx );
    }
}

void test_module_load_bad( void )
{
    rpHCPContext ctx = { 0 };
    rSequence cmd = NULL;
    RpHcp_ModuleId modId = 0;
    RPU8 buffer = NULL;
    RU32 bufferSize = 0;
    RPU8 signature = NULL;
    RU32 sigSize = 0;
    RU32 garbageLoops = 1000;

    CU_ASSERT_FALSE( loadModule( &ctx, NULL ) );
    CU_ASSERT_FALSE( loadModule( NULL, cmd ) );

    // Fuzz signature verification
    for( garbageLoops = garbageLoops; 0 != garbageLoops; garbageLoops-- )
    {
        modId = (RU8)rpal_rand();
        bufferSize = ( rpal_rand() % ( 50 * 1024 ) ) + 1;
        sigSize = ( rpal_rand() % ( 10 * 1024 ) ) + 1;
        buffer = rpal_memory_alloc( bufferSize );
        signature = rpal_memory_alloc( sigSize );

        CU_ASSERT_NOT_EQUAL_FATAL( buffer, NULL );
        CU_ASSERT_NOT_EQUAL_FATAL( signature, NULL );

        CU_ASSERT_FATAL( CryptoLib_genRandomBytes( buffer, bufferSize ) );
        CU_ASSERT_FATAL( CryptoLib_genRandomBytes( signature, sigSize ) );

        cmd = rSequence_new();
        CU_ASSERT_NOT_EQUAL_FATAL( cmd, NULL );
        CU_ASSERT_FATAL( rSequence_addRU8( cmd, RP_TAGS_HCP_MODULE_ID, modId ) );
        CU_ASSERT_FATAL( rSequence_addBUFFER( cmd, RP_TAGS_BINARY, buffer, bufferSize ) );
        CU_ASSERT_FATAL( rSequence_addBUFFER( cmd, RP_TAGS_SIGNATURE, signature, sigSize ) );

        CU_ASSERT_FALSE( loadModule( &ctx, cmd ) );

        rSequence_free( cmd );

        rpal_memory_free( buffer );
        rpal_memory_free( signature );
    }

    // Invalid signature size
    modId = (RU8)rpal_rand();
    bufferSize = ( rpal_rand() % ( 50 * 1024 ) ) + 1;
    sigSize = 3;
    buffer = rpal_memory_alloc( bufferSize );
    signature = rpal_memory_alloc( sigSize );

    CU_ASSERT_NOT_EQUAL_FATAL( buffer, NULL );
    CU_ASSERT_NOT_EQUAL_FATAL( signature, NULL );

    CU_ASSERT_FATAL( CryptoLib_genRandomBytes( buffer, bufferSize ) );
    CU_ASSERT_FATAL( CryptoLib_genRandomBytes( signature, sigSize ) );

    cmd = rSequence_new();
    CU_ASSERT_NOT_EQUAL_FATAL( cmd, NULL );
    CU_ASSERT_FATAL( rSequence_addRU8( cmd, RP_TAGS_HCP_MODULE_ID, modId ) );
    CU_ASSERT_FATAL( rSequence_addBUFFER( cmd, RP_TAGS_BINARY, buffer, bufferSize ) );
    CU_ASSERT_FATAL( rSequence_addBUFFER( cmd, RP_TAGS_SIGNATURE, signature, sigSize ) );

    CU_ASSERT_FALSE( loadModule( &ctx, cmd ) );

    rSequence_free( cmd );

    rpal_memory_free( buffer );
    rpal_memory_free( signature );

    // Fuzz module loading
    for( garbageLoops = 10; 0 != garbageLoops; garbageLoops-- )
    {
        modId = (RU8)rpal_rand();
        bufferSize = ( rpal_rand() % ( 50 * 1024 ) ) + 2;
        sigSize = CRYPTOLIB_SIGNATURE_SIZE;
        buffer = rpal_memory_alloc( bufferSize );
        signature = rpal_memory_alloc( sigSize );

        CU_ASSERT_NOT_EQUAL_FATAL( buffer, NULL );
        CU_ASSERT_NOT_EQUAL_FATAL( signature, NULL );

        CU_ASSERT_FATAL( CryptoLib_genRandomBytes( buffer, bufferSize ) );

        buffer[ 0 ] = 0x4D;
        buffer[ 1 ] = 0x5A;
        
        CU_ASSERT_FATAL( CryptoLib_sign( buffer, bufferSize, g_test_priv, signature ) );

        cmd = rSequence_new();
        CU_ASSERT_NOT_EQUAL_FATAL( cmd, NULL );
        CU_ASSERT_FATAL( rSequence_addRU8( cmd, RP_TAGS_HCP_MODULE_ID, modId ) );
        CU_ASSERT_FATAL( rSequence_addBUFFER( cmd, RP_TAGS_BINARY, buffer, bufferSize ) );
        CU_ASSERT_FATAL( rSequence_addBUFFER( cmd, RP_TAGS_SIGNATURE, signature, sigSize ) );

        CU_ASSERT_FALSE( loadModule( &ctx, cmd ) );

        rSequence_free( cmd );

        rpal_memory_free( buffer );
        rpal_memory_free( signature );
    }
}

void test_module_unload_bad( void )
{
    rpHCPContext ctx = { 0 };
    rSequence cmd = NULL;
    RpHcp_ModuleId modId = 0;

    CU_ASSERT_FALSE( unloadModule( &ctx, NULL ) );
    CU_ASSERT_FALSE( unloadModule( NULL, cmd ) );

    modId = 0;
    cmd = rSequence_new();
    CU_ASSERT_NOT_EQUAL_FATAL( cmd, NULL );
    CU_ASSERT_FATAL( rSequence_addRU8( cmd, RP_TAGS_HCP_MODULE_ID, modId ) );
    CU_ASSERT_FALSE( unloadModule( &ctx, cmd ) );
    rSequence_free( cmd );

    modId = 1;
    cmd = rSequence_new();
    CU_ASSERT_NOT_EQUAL_FATAL( cmd, NULL );
    CU_ASSERT_FATAL( rSequence_addRU8( cmd, RP_TAGS_HCP_MODULE_ID, modId ) );
    CU_ASSERT_FALSE( unloadModule( &ctx, cmd ) );
    rSequence_free( cmd );

    modId = 0xFF;
    cmd = rSequence_new();
    CU_ASSERT_NOT_EQUAL_FATAL( cmd, NULL );
    CU_ASSERT_FATAL( rSequence_addRU8( cmd, RP_TAGS_HCP_MODULE_ID, modId ) );
    CU_ASSERT_FALSE( unloadModule( &ctx, cmd ) );
    rSequence_free( cmd );

}

void test_module_load_unload( void )
{
    rpHCPContext ctx = { 0 };
    rSequence cmd = NULL;
    RpHcp_ModuleId modId = 1;
    RPU8 buffer = NULL;
    RU32 bufferSize = 0;
    RU8 signature[ CRYPTOLIB_SIGNATURE_SIZE ] = { 0 };
    RU32 sigSize = CRYPTOLIB_SIGNATURE_SIZE;
#ifdef RPAL_PLATFORM_WINDOWS
    #ifdef RPAL_PLATFORM_64_BIT
        RPNCHAR testModulePath = _NC( "../../bin/windows/x64/Debug/rpHCP_TestModule.dll" );
    #else
        RPNCHAR testModulePath = _NC( "../../bin/windows/Win32/Debug/rpHCP_TestModule.dll" );
    #endif
#elif definde( RPAL_PLATFORM_MACOSX )
    #TODO(Add the OSX paths)
#endif

    CU_ASSERT_FATAL( rpal_file_read( testModulePath, &buffer, &bufferSize, FALSE ) );
    CU_ASSERT_FATAL( CryptoLib_sign( buffer, bufferSize, g_test_priv, signature ) );

    cmd = rSequence_new();
    CU_ASSERT_NOT_EQUAL_FATAL( cmd, NULL );
    CU_ASSERT_FATAL( rSequence_addRU8( cmd, RP_TAGS_HCP_MODULE_ID, modId ) );
    CU_ASSERT_FATAL( rSequence_addBUFFER( cmd, RP_TAGS_BINARY, buffer, bufferSize ) );
    CU_ASSERT_FATAL( rSequence_addBUFFER( cmd, RP_TAGS_SIGNATURE, signature, sigSize ) );
    CU_ASSERT_TRUE( loadModule( &ctx, cmd ) );
    CU_ASSERT_TRUE( unloadModule( &ctx, cmd ) );
    rSequence_free( cmd );

    rpal_memory_free( buffer );
}

void test_store_conf( void )
{
    RPNCHAR tmpStore = _NC( "./__tmp_store" );

    rpHCPIdentStore ident = { 0 };
    RU8 token[ CRYPTOLIB_HASH_SIZE ] = { 0 };
    rpHCPContext hcpContext = { 0 };
    RPU8 garbage = NULL;
    RU32 garbageSize = 0;
    rFileInfo info = { 0 };

    // Set a reference value
    ident.agentId.architecture = (RU8)( -1 );
    ident.agentId.platform = (RU32)( -1 );
    CU_ASSERT_FATAL( CryptoLib_genRandomBytes( ident.agentId.ins_id, sizeof( ident.agentId.ins_id ) ) );
    CU_ASSERT_FATAL( CryptoLib_genRandomBytes( ident.agentId.org_id, sizeof( ident.agentId.org_id ) ) );
    CU_ASSERT_FATAL( CryptoLib_genRandomBytes( ident.agentId.sensor_id, sizeof( ident.agentId.sensor_id ) ) );
    CU_ASSERT_FATAL( CryptoLib_genRandomBytes( token, sizeof( token ) ) );
    ident.enrollmentTokenSize = sizeof( token );

    // Do a good read write
    CU_ASSERT_FATAL( saveHcpId( tmpStore, &ident, token, sizeof( token ) ) );
    CU_ASSERT( getStoreConf( tmpStore, &hcpContext ) ); // Platform and Arch get overwritten, that's normal
    CU_ASSERT_EQUAL( rpal_memory_memcmp( hcpContext.currentId.ins_id, ident.agentId.ins_id, sizeof( ident.agentId.ins_id ) ), 0 );
    CU_ASSERT_EQUAL( rpal_memory_memcmp( hcpContext.currentId.org_id, ident.agentId.org_id, sizeof( ident.agentId.org_id ) ), 0 );
    CU_ASSERT_EQUAL( rpal_memory_memcmp( hcpContext.currentId.sensor_id, ident.agentId.sensor_id, sizeof( ident.agentId.sensor_id ) ), 0 );
    CU_ASSERT_NOT_EQUAL( hcpContext.currentId.architecture, ident.agentId.architecture );
    CU_ASSERT_NOT_EQUAL( hcpContext.currentId.platform, ident.agentId.platform );
    CU_ASSERT_EQUAL( hcpContext.enrollmentTokenSize, sizeof( token ) );
    CU_ASSERT_EQUAL( rpal_memory_memcmp( hcpContext.enrollmentToken, token, sizeof( token ) ), 0 );
    rpal_memory_free( hcpContext.enrollmentToken );
    CU_ASSERT( rpal_file_delete( tmpStore, FALSE ) );

    // Do a bad write bad read, make sure it gets wiped
    garbageSize = 38;
    garbage = rpal_memory_alloc( garbageSize );
    CU_ASSERT_NOT_EQUAL_FATAL( garbage, NULL );
    CU_ASSERT_FATAL( rpal_file_write( tmpStore, garbage, garbageSize, TRUE ) );
    CU_ASSERT_FALSE( getStoreConf( tmpStore, &hcpContext ) );
    CU_ASSERT_FALSE( rpal_file_getInfo( tmpStore, &info ) );
    rpal_memory_free( garbage );

    // Validate the basics
    CU_ASSERT_FALSE( saveHcpId( NULL, &ident, token, sizeof( token ) ) );
    CU_ASSERT_FALSE( saveHcpId( tmpStore, NULL, token, sizeof( token ) ) );
    CU_ASSERT_FALSE( saveHcpId( tmpStore, &ident, NULL, sizeof( token ) ) );
    CU_ASSERT_FALSE( saveHcpId( tmpStore, &ident, token, 0 ) );
    CU_ASSERT_FALSE( getStoreConf( NULL, &hcpContext ) );
    CU_ASSERT_FALSE( getStoreConf( tmpStore, NULL ) );
}

int
    main
    (
        int argc,
        char* argv[]
    )
{
    int ret = 1;

    CU_pSuite suite = NULL;

    UNREFERENCED_PARAMETER( argc );
    UNREFERENCED_PARAMETER( argv );

    rpal_initialize( NULL, 1 );

    CU_initialize_registry();

    if( NULL != ( suite = CU_add_suite( "hcp", NULL, NULL ) ) )
    {
        if( NULL == CU_add_test( suite, "create_cloud", test_create_dummy_cloud ) ||
            NULL == CU_add_test( suite, "frames", test_frames ) ||
            NULL == CU_add_test( suite, "exchange_frames", test_exchange_frames ) ||
            NULL == CU_add_test( suite, "destroy_cloud", test_destroy_dummy_cloud ) ||
            NULL == CU_add_test( suite, "module_load_bad", test_module_load_bad ) ||
            NULL == CU_add_test( suite, "module_unload_bad", test_module_unload_bad ) ||
            NULL == CU_add_test( suite, "module_load_unload", test_module_load_unload ) ||
            NULL == CU_add_test( suite, "store_conf", test_store_conf ) ||
            NULL == CU_add_test( suite, "memoryLeaks", test_memoryLeaks ) )
        {
            ret = 0;
        }
    }

    CU_basic_run_tests();

    CU_cleanup_registry();

    rpal_Context_deinitialize();

    return ret;
}

