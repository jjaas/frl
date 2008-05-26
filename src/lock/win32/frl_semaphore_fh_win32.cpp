#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "lock/frl_semaphore_fn.h"
#include "frl_exception.h"

namespace frl
{
namespace lock
{
void semaphoreInit( SemaphoreDescriptor &semaphore, Long value )
{
	FRL_EXCEPT_GUARD_EX( FRL_FUNCTION_NAME );
	if ( semaphore.semaphore != INVALID_HANDLE_VALUE )
		FRL_THROW( FRL_STR( "Semaphore already initialize." ) );
	semaphore.semaphore = ::CreateSemaphore( NULL, ( LONG )value, 0x7fffffff, NULL );
	if( semaphore.semaphore == INVALID_HANDLE_VALUE )
		FRL_THROW( FRL_STR( "Error initialize semaphore." ) );
	semaphore.curValue = value;
}

void semaphoreIsValid( SemaphoreDescriptor &semaphore )
{
	FRL_EXCEPT_GUARD_EX( FRL_FUNCTION_NAME );
	if ( semaphore.semaphore == INVALID_HANDLE_VALUE )
		FRL_THROW( FRL_STR( "Semaphore not valid. Maybe semaphore not initialize." ) );
}

void semaphoreDestroy( SemaphoreDescriptor &semaphore )
{
	FRL_EXCEPT_GUARD_EX( FRL_FUNCTION_NAME );
	semaphoreIsValid( semaphore );
	if( ! CloseHandle( semaphore.semaphore ) )
		FRL_THROW( FRL_STR( "Error semaphore destroy." ) );
	semaphore.semaphore = INVALID_HANDLE_VALUE;
}

Long semaphoreGetValue( SemaphoreDescriptor &semaphore )
{
	FRL_EXCEPT_GUARD_EX( FRL_FUNCTION_NAME );
	semaphoreIsValid( semaphore );
	return semaphore.curValue;
}

void semaphorePost( SemaphoreDescriptor &semaphore )
{
	FRL_EXCEPT_GUARD_EX( FRL_FUNCTION_NAME );
	semaphoreIsValid( semaphore );
	::ReleaseSemaphore( semaphore.semaphore, 1, NULL ); // FIXME analyze return value
	++semaphore.curValue;
}

void semaphoreWait( SemaphoreDescriptor &semaphore )
{
	FRL_EXCEPT_GUARD_EX( FRL_FUNCTION_NAME );
	semaphoreIsValid( semaphore );
	DWORD result = ::WaitForSingleObject( semaphore.semaphore, INFINITE );
	switch( result )
	{
	case WAIT_OBJECT_0:
		--semaphore.curValue;
		break;
	case WAIT_TIMEOUT:
		break;
	case WAIT_ABANDONED:
		break;
	default:
		FRL_THROW( FRL_STR( "Error wait semaphore." ) ); // FIXME analyze GetLastError()
	}
}

Bool semaphoreTryWait( SemaphoreDescriptor &semaphore )
{
	FRL_EXCEPT_GUARD_EX( FRL_FUNCTION_NAME );
	semaphoreIsValid( semaphore );
	DWORD result = ::WaitForSingleObject( semaphore.semaphore, 0 );
	switch( result )
	{
	case WAIT_OBJECT_0:
		--semaphore.curValue;
		return True;
		break;
	case WAIT_TIMEOUT:
		return False;
		break;
	case WAIT_ABANDONED:
		return False;
		break;
	default:
		FRL_THROW( FRL_STR( "Unknown error try-wait semaphore." ) ); // FIXME analyze GetLastError()
	}
}

Bool semaphoreTimedWait( SemaphoreDescriptor &semaphore, TimeOut time_ )
{
	FRL_EXCEPT_GUARD_EX( FRL_FUNCTION_NAME );
	semaphoreIsValid( semaphore );
	DWORD result = ::WaitForSingleObject( semaphore.semaphore, time_ );
	switch( result )
	{
	case WAIT_OBJECT_0:
		--semaphore.curValue;
		return True;
		break;
	case WAIT_TIMEOUT:
		return False;
		break;
	case WAIT_ABANDONED:
		return False;
		break;
	default:
		FRL_THROW( FRL_STR( "Unknown error timed wait semaphore." ) );  // FIXME analyze GetLastError()
	}
}

} // namespace lock
} // FatRat Library

#endif // FRL_PLATFORM_WIN32